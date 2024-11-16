package main

import (
	"embed"
	"fmt"
	"forg/pkg/compile"
	"forg/pkg/log"
	"forg/pkg/util"
	"os"
	"path/filepath"

	oglog "log"

	"github.com/urfave/cli/v2"
)

var (
	workingDirectory = "."

	//go:embed template/*
	templateProject embed.FS
)

func init() {
	var err error
	workingDirectory, err = os.Getwd()
	log.AssertErr(err)
}

func initCmd(ctx *cli.Context) error {
	workingDirectory = util.Either(ctx.String("path"), workingDirectory)
	//TODO: check for existing project before creating a new one
	err := os.Mkdir(workingDirectory, 0o755)
	if os.IsNotExist(err) {
		log.AssertErr(err)
	}
	entries, err := templateProject.ReadDir("template")
	log.AssertErr(err)

	for _, entry := range entries {
		if entry.Type().IsRegular() {
			inPath := filepath.Join("template", entry.Name())
			outPath := filepath.Join(workingDirectory, entry.Name())
			bytes, err := templateProject.ReadFile(inPath)
			log.AssertErr(err)
			err = os.WriteFile(outPath, bytes, 0o644)
			if os.IsExist(err) {
				log.AssertErr(err)
			}
		}
	}

	absPath := workingDirectory
	if p, e := filepath.Abs(absPath); e == nil {
		absPath = p
	}

	log.Stderr("Initialized new project in %s\n", absPath)
	return nil
}

func buildCmd(ctx *cli.Context) error {
	t, err := compile.NewTarget(workingDirectory)
	log.AssertErr(err)
	log.AssertErr(compile.Compile(t))
	return nil
}

func runCmd(ctx *cli.Context) error {
	t, err := compile.NewTarget(workingDirectory)
	log.AssertErr(err)
	log.AssertErr(compile.Compile(t))

	log.AssertErr(util.Run([]string{
		t.OutputPath,
	}))
	return nil
}

func main() {
	app := &cli.App{
		Name:  "forg",
		Usage: "Build games",
		Action: func(*cli.Context) error {
			fmt.Println("boom! I say!")
			return nil
		},
		Commands: []*cli.Command{
			{
				Name:   "build",
				Usage:  "build project",
				Action: buildCmd,
			},
			{
				Name:   "run",
				Usage:  "run project",
				Action: runCmd,
			},
			{
				Name:   "init",
				Usage:  "init new project",
				Action: initCmd,
			},
		},
		Flags: []cli.Flag{
			&cli.StringFlag{
				Name:    "path",
				Aliases: []string{"p"},
				Usage:   "set path to project to operate on",
			},
		},
	}

	if err := app.Run(os.Args); err != nil {
		oglog.Fatal(err)
	}

	/*
		switch op {
		case "init":
			initCmd()
		case "build":
			buildCmd()
		case "run":
			runCmd()
		}
	*/
}
