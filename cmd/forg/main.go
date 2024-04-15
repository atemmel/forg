package main

import (
	_ "embed"
	"flag"
	"forg/pkg/compile"
	"forg/pkg/log"
	"os"
	"path/filepath"
)

var (
	workingDirectory = "."

	//go:embed embed/main.cpp
	mainBoilerplate []byte
)

func init() {
	flag.StringVar(&workingDirectory, "p", ".", "Set path to project to operate on")
	flag.Parse()
}

func initCmd() {
	//TODO: check for existing project before creating a new one
	err := os.WriteFile(filepath.Join(workingDirectory, "main.cpp"), mainBoilerplate, 0o644)
	log.Assert(err)

	absPath := workingDirectory
	if p, e := filepath.Abs(absPath); e == nil {
		absPath = p
	}

	log.Stderr("Initialized new project in %s\n", absPath)
}

func buildCmd() {
	buildDir := filepath.Join(workingDirectory, "build")
	_ = os.Mkdir(buildDir, 0o755)
	glob := filepath.Join(workingDirectory, "*.cpp")
	files, err := filepath.Glob(glob)
	log.Assert(err)
	units := make([]compile.Unit, len(files))
	for i := range files {
		units[i] = compile.Unit{
			Path: files[i],
		}
	}
	err = compile.Compile(&compile.Target{
		Units:    units,
		BuildDir: buildDir,
	})
	log.Assert(err)
}

func main() {
	op := flag.Arg(0)
	if op == "" {
		log.Stderr("No operation specified...\n")
	}

	switch op {
	case "init":
		initCmd()
	case "build":
		buildCmd()
	}
}
