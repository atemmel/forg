package compile

import (
	"context"
	"forg/pkg/log"
	"forg/pkg/util"
	"math"
	"os"
	"os/exec"
	"path"
	"path/filepath"
	"runtime"
	"strings"
)

type Unit struct {
	Path string
}

type Flags struct {
	CompileFlags string `yaml:"compile-flags"`
	LinkerFlags  string `yaml:"linker-flags"`
}

type Opts struct {
	ProjectName  string
	OutputPath   string
	ProjectDir   string
	BuildDir     string
	Units        []Unit
	Target       string
	KeepStderr   bool
	CompileFlags []string
	LinkerFlags  []string
}

type compileCtx struct {
	ctx          context.Context
	results      chan result
	step         int
	opts         *Opts
	units        chan Unit
	vendored     []string
	compileFlags []string
	linkerFlags  []string
}

type result struct {
	unit   Unit
	err    error
	stderr string
}

func recurseFind(dir string, ext string) ([]string, error) {
	files := []string{}
	err := filepath.Walk(dir, func(path string, f os.FileInfo, err error) error {
		if filepath.Ext(path) == ext {
			files = append(files, path)
		}
		return nil
	})
	return files, err
}

func NewOpts(workingDir string, compileflags, linkerFlags []string) (*Opts, error) {
	absWorkingDir, err := filepath.Abs(workingDir)
	if err != nil {
		return nil, err
	}
	files, err := recurseFind(absWorkingDir, ".cpp")
	if err != nil {
		return nil, err
	}

	units := make([]Unit, len(files))
	for i := range files {
		units[i] = Unit{
			Path: files[i],
		}
	}

	projectName := filepath.Base(absWorkingDir)
	buildDir := workingDir + "/build"

	return &Opts{
		ProjectName:  projectName,
		ProjectDir:   absWorkingDir,
		OutputPath:   buildDir + "/" + projectName,
		Units:        units,
		BuildDir:     buildDir,
		CompileFlags: compileflags,
		LinkerFlags:  linkerFlags,
	}, nil
}

func Compile(o *Opts) error {
	_, err := os.Stat(o.ProjectDir)
	if os.IsNotExist(err) {
		return err
	}
	_ = os.Mkdir(o.BuildDir, 0o755)

	nUnits := len(o.Units)
	if nUnits == 0 {
		return nil
	}

	ctx, cancel := context.WithCancel(context.Background())

	compileCtx := compileCtx{
		ctx:          ctx,
		results:      make(chan result, nUnits),
		step:         0,
		opts:         o,
		units:        make(chan Unit, nUnits),
		vendored:     make([]string, 0, 16),
		compileFlags: o.CompileFlags,
		linkerFlags:  o.LinkerFlags,
	}

	entries, err := os.ReadDir("./vendor")
	if err == nil && len(entries) > 0 {
		for _, entry := range entries {
			if entry.IsDir() {
				compileCtx.vendored = append(compileCtx.vendored, "./vendor/"+entry.Name())
			}
		}
	}

	// create worker pool
	nCpu := runtime.NumCPU()
	for i := 0; i < min(nUnits, nCpu); i++ {
		go startWorker(&compileCtx)
	}

	// queue data for pool
	go queueWork(&compileCtx)

	// wait for workerpool to be done
	for i := 0; i < nUnits; i++ {
		result := <-compileCtx.results
		if result.err != nil {
			log.Stderr("Error occured, waiting for unfinished jobs...\n")
			log.Stderr(result.stderr)
			cancel()
			return result.err
		}

		logCompilation(&compileCtx, result.unit)
	}

	cancel()

	err, stderr := linkTarget(&compileCtx)
	if err == nil {
		log.Verbose("[100%%] link\n")
	}
	if stderr != "" {
		log.Stderr(stderr)
	} else if err != nil {
		log.Stderr("%v\n", err)
	}
	return err
}

func queueWork(compileCtx *compileCtx) {
	for _, unit := range compileCtx.opts.Units {
		compileCtx.units <- unit
	}
	close(compileCtx.units)
}

func startWorker(compileCtx *compileCtx) {
	for {
		select {
		case unit := <-compileCtx.units:
			if unit.Path == "" { // sometimes this happens, no clue why
				continue
			}
			err, output := compileUnit(compileCtx, unit)
			compileCtx.results <- result{
				err:    err,
				unit:   unit,
				stderr: output,
			}
		case _ = <-compileCtx.ctx.Done():
			return
		}
	}
}

func logCompilation(compileCtx *compileCtx, unit Unit) {
	nUnits := len(compileCtx.opts.Units)
	step := compileCtx.step

	filename := path.Base(unit.Path)
	progress := int(math.Round(float64(step+1) / float64(nUnits+1) * 100))
	//TODO: prefix with \033[2K\r
	log.Verbose("[%3d%%] %s\n", progress, filename)
	compileCtx.step++
}

func compileUnit(compileCtx *compileCtx, unit Unit) (error, string) {
	log.Verbose("Compiling: %v\n", unit)
	base := filepath.Base(unit.Path)
	fullpath := filepath.Join(compileCtx.opts.BuildDir, base+".o")

	args := []string{
		"c++",
		"-c", unit.Path,
		"-o", fullpath,
	}

	args = append(args, compileCtx.compileFlags...)

	for _, dir := range compileCtx.vendored {
		innerDirs, err := os.ReadDir(dir)
		if err != nil || len(innerDirs) <= 0 {
			continue
		}

		for _, sub := range innerDirs {
			if sub.IsDir() && sub.Name() == "include" {
				args = append(args, "-I"+dir+"/include")
			}
		}
	}

	if "" != compileCtx.opts.Target {
		args = append(args, "-target", compileCtx.opts.Target)
	}

	log.Verbose("Compiling using: zig %v\n", args)
	cmd := exec.CommandContext(compileCtx.ctx, "zig", args...)
	if compileCtx.opts.KeepStderr {
		bytes, err := cmd.CombinedOutput()
		return err, string(bytes)
	}
	cmd.Stderr = os.Stderr
	cmd.Stdout = os.Stdout
	return cmd.Run(), ""
}

func linkTarget(compileCtx *compileCtx) (error, string) {
	glob, err := filepath.Glob(compileCtx.opts.BuildDir + "/*.o")
	if err != nil {
		return err, ""
	}
	args := []string{
		"c++",
		"-o", compileCtx.opts.OutputPath,
	}
	args = append(args, compileCtx.linkerFlags...)
	args = append(args, glob...)
	if "" == compileCtx.opts.Target {
		args = append(args, "-L/usr/local/lib")
		args = append(args, "-L/usr/lib")
	}

	for _, dir := range compileCtx.vendored {
		innerDirs, err := os.ReadDir(dir)
		if err != nil || len(innerDirs) <= 0 {
			continue
		}

		for _, sub := range innerDirs {
			if sub.IsDir() && sub.Name() == "lib" {
				args = append(args, "-L"+dir+"/lib")
			}
		}
	}

	/*
		fetch.FetchLibIfNotLocallyResolved(fetch.Opts{
			Filename:  "libraylib",
			Target:    compileCtx.opts.Target,
			UrlPrefix: "https://github.com/raysan5/raylib/releases/download/5.5",
			UrlFilename: fetch.Platform{
				Linux:   "raylib-5.5_linux_amd64",
				Windows: "raylib-5.5_win64_mingw-w64",
			},
			ArchiveKind: fetch.Platform{
				Linux:   ".tar.gz",
				Windows: ".zip",
			},
		})
	*/

	linkers := []string{
		"mold",
		"lld",
	}

	for _, ld := range linkers {
		if util.InPath(ld) {
			args = append(args, "-fuse-ld="+ld)
			break
		}
	}

	log.Verbose("Linking using: %v\n", strings.Join(args, " "))

	cmd := exec.Command("zig", args...)
	if compileCtx.opts.KeepStderr {
		bytes, err := cmd.CombinedOutput()
		return err, string(bytes)
	}
	cmd.Stdout = os.Stdout
	cmd.Stderr = os.Stderr
	return cmd.Run(), ""
}
