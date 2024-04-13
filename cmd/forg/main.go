package main

import (
	_ "embed"
	"flag"
	"forg/pkg/log"
	"os"
	"path/filepath"
)

var (
	workingDirectory = "."

	//go:embed embed/main.cpp
	mainBoilerplate string
)

func init() {
	flag.StringVar(&workingDirectory, "p", ".", "Set path to project to operate on")
	flag.Parse()
}

func initCmd() {
	err := os.WriteFile(filepath.Join(workingDirectory, "main.cpp"), []byte(mainBoilerplate), 0o644)
	log.Assert(err)

	absPath := workingDirectory
	if p, e := filepath.Abs(absPath); e == nil {
		absPath = p
	}

	log.Stderr("Initialized new project in %s", absPath)
}

func main() {
	op := flag.Arg(0)
	if op == "" {
		log.Stderr("No operation specified...\n")
	}

	switch op {
	case "init":
		initCmd()
	}
}
