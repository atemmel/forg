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
	"sync"
)

type Unit struct {
	Path string
}

type Target struct {
	ProjectName string
	OutputPath  string
	ProjectDir  string
	BuildDir    string
	Units       []Unit
}

type compileCtx struct {
	ctx       context.Context
	results   chan result
	step      int
	stepMutex sync.Mutex
	target    *Target
	units     chan Unit
}

type result struct {
	unit Unit
	err  error
}

func NewTarget(workingDir string) (*Target, error) {
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

	return &Target{
		ProjectName: projectName,
		ProjectDir:  absWorkingDir,
		OutputPath:  buildDir + "/" + projectName,
		Units:       units,
		BuildDir:    buildDir,
	}, nil
}

func Compile(t *Target) error {
	_, err := os.Stat(t.ProjectDir)
	if os.IsNotExist(err) {
		return err
	}
	_ = os.Mkdir(t.BuildDir, 0o755)

	nUnits := len(t.Units)
	if nUnits == 0 {
		return nil
	}

	ctx, cancel := context.WithCancel(context.Background())

	compileCtx := compileCtx{
		ctx:       ctx,
		results:   make(chan result, nUnits),
		step:      0,
		stepMutex: sync.Mutex{},
		target:    t,
		units:     make(chan Unit, nUnits),
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
	for _, unit := range compileCtx.target.Units {
		compileCtx.units <- unit
	}
	close(compileCtx.units)
}

func startWorker(compileCtx *compileCtx) {
	for {
		select {
		case unit := <-compileCtx.units:
			//logCompilation(compileCtx, unit)
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
	compileCtx.stepMutex.Lock()
	defer compileCtx.stepMutex.Unlock()

	nUnits := len(compileCtx.target.Units)
	step := compileCtx.step

	filename := path.Base(unit.Path)
	progress := int(math.Round(float64(step+1) / float64(nUnits+1) * 100))
	//TODO: prefix with \033[2K\r
	log.Stderr("[%3d%%] %s\n", progress, filename)
	compileCtx.step++
}

func compileUnit(compileCtx *compileCtx, unit Unit) error {
	base := filepath.Base(unit.Path)
	fullpath := filepath.Join(compileCtx.target.BuildDir, base+".o")
	cmd := exec.CommandContext(compileCtx.ctx, "g++", "-c", unit.Path, "-o", fullpath)
	cmd.Stdout = os.Stdout
	cmd.Stderr = os.Stderr
	return cmd.Run()
}

func linkTarget(compileCtx *compileCtx) error {
	glob, err := filepath.Glob(compileCtx.target.BuildDir + "/*.o")
	if err != nil {
		return err
	}
	args := []string{
		"g++",
		"-o",
		compileCtx.target.OutputPath,
	}
	return util.Run(append(args, glob...))
}
