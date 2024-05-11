package main

import (
	"embed"
	"flag"
	"forg/pkg/compile"
	"forg/pkg/log"
	"forg/pkg/util"
	"os"
	"path/filepath"
)

var (
	workingDirectory = "."

	//go:embed template/*
	templateProject embed.FS
)

func init() {
	flag.StringVar(&workingDirectory, "p", ".", "Set path to project to operate on")
	flag.Parse()
	if workingDirectory == "." {
		var err error
		workingDirectory, err = os.Getwd()
		log.Assert(err)
	}
}

func initCmd() {
	//TODO: check for existing project before creating a new one
	err := os.Mkdir(workingDirectory, 0o755)
	if os.IsNotExist(err) {
		log.Assert(err)
	}
	entries, err := templateProject.ReadDir("template")
	log.Assert(err)

	for _, entry := range entries {
		if entry.Type().IsRegular() {
			inPath := filepath.Join("template", entry.Name())
			outPath := filepath.Join(workingDirectory, entry.Name())
			bytes, err := templateProject.ReadFile(inPath)
			log.Assert(err)
			err = os.WriteFile(outPath, bytes, 0o644)
			if os.IsExist(err) {
				log.Assert(err)
			}
		}
	}

	absPath := workingDirectory
	if p, e := filepath.Abs(absPath); e == nil {
		absPath = p
	}

	log.Stderr("Initialized new project in %s\n", absPath)
}

func buildCmd() {
	t, err := compile.NewTarget(workingDirectory)
	log.Assert(err)
	log.Assert(compile.Compile(t))
}

func runCmd() {
	t, err := compile.NewTarget(workingDirectory)
	log.Assert(err)
	log.Assert(compile.Compile(t))

	log.Assert(util.Run([]string{
		t.OutputPath,
	}))
}

func main() {
	op := flag.Arg(0)
	if op == "" {
		log.Stderr("No operation specified...\n")
	}

	switch op {
	case "init":
		workingDirectory = util.Either(flag.Arg(1), workingDirectory)
		initCmd()
	case "build":
		buildCmd()
	case "run":
		runCmd()
	}
}
