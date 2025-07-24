package main

import (
	"embed"
	"flag"
	"fmt"
	"forg/pkg/compile"
	"forg/pkg/log"
	"forg/pkg/util"
	oglog "log"
	"os"
	"path/filepath"

	"github.com/urfave/cli/v2" // imports as package "cli"
)

var (
	workingDirectory = "."
	wantsHelp        = false

	//go:embed template/*
	templateProject embed.FS
)

func init() {
	flag.StringVar(&workingDirectory, "p", ".", "Set path to project to operate on")
	flag.BoolVar(&wantsHelp, "h", false, "Show this help text")
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

	app := &cli.App{
		Name:  "forg",
		Usage: "game dev toolkit",
		Action: func(*cli.Context) error {
			fmt.Println("boom! I say!")
			return nil
		},
		Commands: []*cli.Command{
			{
				Name:    "append",
				Aliases: []string{"a"},
				Usage:   "add a task to the list",
				Action: func(cCtx *cli.Context) error {
					fmt.Println("added task: ", cCtx.Args().First())
					return nil
				},
			},
		},
	}

	cli.AppHelpTemplate = "forg\n\n" + cli.AppHelpTemplate

	if err := app.Run(os.Args); err != nil {
		oglog.Fatal(err)
	}

	/*
		op := flag.Arg(0)

		if wantsHelp {
			flag.Usage()
			return
		}

		if op == "" {
			log.Stderr("No operation specified, use -h for help\n")
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
	*/
}
