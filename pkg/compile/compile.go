package compile

import (
	"context"
	"fmt"
	"forg/pkg/fetch"
	"forg/pkg/log"
	"forg/pkg/util"
	"math"
	"os"
	"os/exec"
	"path"
	"path/filepath"
	"runtime"
)

type Unit struct {
	Path string
}

type Opts struct {
	ProjectName string
	OutputPath  string
	ProjectDir  string
	BuildDir    string
	Units       []Unit
	Target      string
}

type compileCtx struct {
	ctx     context.Context
	results chan result
	step    int
	opts    *Opts
	units   chan Unit
}

type result struct {
	unit Unit
	err  error
}

func NewOpts(workingDir string) (*Opts, error) {
	absWorkingDir, err := filepath.Abs(workingDir)
	if err != nil {
		return nil, err
	}
	files, err := filepath.Glob(absWorkingDir + "/*.cpp")
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
		ProjectName: projectName,
		ProjectDir:  absWorkingDir,
		OutputPath:  buildDir + "/" + projectName,
		Units:       units,
		BuildDir:    buildDir,
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
		ctx:     ctx,
		results: make(chan result, nUnits),
		step:    0,
		opts:    o,
		units:   make(chan Unit, nUnits),
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
			cancel()
			return result.err
		}

		logCompilation(&compileCtx, result.unit)
	}

	cancel()

	err = linkTarget(&compileCtx)
	if err == nil {
		log.Stderr("[100%%] link\n")
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
			compileCtx.results <- result{
				err:  compileUnit(compileCtx, unit),
				unit: unit,
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

func compileUnit(compileCtx *compileCtx, unit Unit) error {
	log.Verbose("Compiling: %v", unit)
	base := filepath.Base(unit.Path)
	fullpath := filepath.Join(compileCtx.opts.BuildDir, base+".o")

	args := []string{
		"c++",
		"-c", unit.Path,
		"-o", fullpath,
	}

	if runtime.GOOS != "windows" {
		args = append(args, "-I/usr/local/include")
	}

	if "" != compileCtx.opts.Target {
		include := fmt.Sprintf("-I%s", fetch.GetIncludeDirFromCleanTarget(util.CleanTarget(compileCtx.opts.Target)))
		args = append(args, "-target", compileCtx.opts.Target, include)
	}

	log.Verbose("Compiling using: zig %v", args)
	cmd := exec.CommandContext(compileCtx.ctx, "zig", args...)
	cmd.Stdout = os.Stdout
	cmd.Stderr = os.Stderr
	return cmd.Run()
}

func linkTarget(compileCtx *compileCtx) error {
	glob, err := filepath.Glob(compileCtx.opts.BuildDir + "/*.o")
	if err != nil {
		return err
	}
	args := []string{
		"zig",
		"c++",
		"-o",
		compileCtx.opts.OutputPath,
	}
	args = append(args, glob...)
	if runtime.GOOS != "windows" {
		args = append(args, "-L/usr/local/lib")
	}
	args = append(args, fmt.Sprintf("-L%s", fetch.GetLibDirFromCleanTarget(util.CleanTarget(compileCtx.opts.Target))))
	args = append(args, "-lforg", "-lraylib")

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

	if "" != compileCtx.opts.Target {
		args = append(args, "-target", compileCtx.opts.Target)
	} else { // native build
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
	}

	log.Verbose("Linking using: %v", args)

	return util.Run(args)
}
