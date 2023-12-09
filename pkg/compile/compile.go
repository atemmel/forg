package compile

import (
	"context"
	"fmt"
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
	ctx context.Context
	results chan error
	step int
	stepMutex sync.Mutex
	target *Target
	units chan Unit
}

func Compile(t *Target) error {
	nUnits := len(t.Units)

	ctx, cancel := context.WithCancel(context.Background())
	results := make(chan error, nUnits)
	units := make(chan Unit, nUnits)

	//TODO: cancel on failed build
	_ = cancel

	compileCtx := compileCtx{
		ctx: ctx,
		results: results,
		step: 0,
		stepMutex: sync.Mutex{},
		target: t,
		units: units,
	}

	// create worker pool
	nCpu := runtime.NumCPU()
	for i := 0; i < nCpu; i++ {
		go startWorker(&compileCtx)
	}

	// queue data for pool
	go queueWork(&compileCtx)

	// wait for workerpool to be done
	for i := 0; i < nUnits; i++ {
		err := <- compileCtx.results
		if err != nil {
			panic(err)
		}
	}
	errf("[100%%]\n")
	return nil
}

func queueWork(compileCtx *compileCtx) {
	for _, unit := range compileCtx.target.Units {
		compileCtx.units <- unit
	}
	close(compileCtx.units)
}

func startWorker(ctx *compileCtx) {
	for unit := range ctx.units {
		logCompilation(ctx, unit)
		ctx.results <- compileUnit(unit)
	}
}

func logCompilation(ctx *compileCtx, unit Unit) {
	ctx.stepMutex.Lock()
	defer ctx.stepMutex.Unlock()

	nUnits := len(ctx.target.Units)
	step := ctx.step

	filename := path.Base(unit.Path)
	progress := int(math.Round(float64(step) / float64(nUnits) * 100))
	errf("[%3d%%] %s\n", progress, filename)
	ctx.step++
}

func compileUnit(unit Unit) error {
	cmd := exec.Command("g++", "-c", unit.Path)
	cmd.Stdout = os.Stdout
	cmd.Stderr = os.Stderr
	return cmd.Run()
}

func errf(str string, args ...any) {
	fmt.Fprintf(os.Stderr, str, args...)
}
