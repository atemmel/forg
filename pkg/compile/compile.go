package compile

import (
	"context"
	"forg/pkg/log"
	"math"
	"os"
	"os/exec"
	"path"
	"runtime"
	"sync"
)

type Unit struct {
	Path string
}

type Target struct {
	Units []Unit
}

type compileCtx struct {
	ctx       context.Context
	results   chan error
	step      int
	stepMutex sync.Mutex
	target    *Target
	units     chan Unit
}

func Compile(t *Target) error {
	nUnits := len(t.Units)

	ctx, cancel := context.WithCancel(context.Background())

	compileCtx := compileCtx{
		ctx:       ctx,
		results:   make(chan error, nUnits),
		step:      0,
		stepMutex: sync.Mutex{},
		target:    t,
		units:     make(chan Unit, nUnits),
	}

	// create worker pool
	nCpu := runtime.NumCPU()
	for i := 0; i < nCpu; i++ {
		go startWorker(&compileCtx)
	}

	// queue data for pool
	go queueWork(&compileCtx)

	var err error

	// wait for workerpool to be done
	for i := 0; i < nUnits; i++ {
		err = <-compileCtx.results
		if err != nil {
			log.Error("Error occured, waiting for unfinished jobs...\n")
			cancel()
			break
		}
	}
	cancel()

	if err != nil {
		return err
	}

	log.Error("[100%%]\n")
	return nil
}

func queueWork(compileCtx *compileCtx) {
	for _, unit := range compileCtx.target.Units {
		compileCtx.units <- unit
	}
	close(compileCtx.units)
}

func startWorker(compileCtx *compileCtx) {
	for unit := range compileCtx.units {
		if compileCtx.ctx.Err() != nil {
			compileCtx.results <- nil
		} else {
			logCompilation(compileCtx, unit)
			compileCtx.results <- compileUnit(compileCtx, unit)
		}
	}
}

func logCompilation(compileCtx *compileCtx, unit Unit) {
	compileCtx.stepMutex.Lock()
	defer compileCtx.stepMutex.Unlock()
	if compileCtx.ctx.Err() != nil {
		return
	}

	nUnits := len(compileCtx.target.Units)
	step := compileCtx.step

	filename := path.Base(unit.Path)
	progress := int(math.Round(float64(step) / float64(nUnits) * 100))
	//TODO: prefix with \033[2K\r
	log.Error("[%3d%%] %s\n", progress, filename)
	compileCtx.step++
}

func compileUnit(compileCtx *compileCtx, unit Unit) error {
	cmd := exec.CommandContext(compileCtx.ctx, "g++", "-c", unit.Path)
	cmd.Stdout = os.Stdout
	cmd.Stderr = os.Stderr
	return cmd.Run()
}
