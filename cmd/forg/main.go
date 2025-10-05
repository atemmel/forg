package main

import (
	"embed"
	"forg/pkg/compile"
	"forg/pkg/log"
	"forg/pkg/util"
	"forg/pkg/watch"
	"os"
	"os/exec"
	"path/filepath"

	"github.com/urfave/cli/v2" // imports as package "cli"
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
	log.LogVerbose = ctx.Bool("verbose")
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
	log.LogVerbose = ctx.Bool("verbose")
	workingDirectory = util.Either(ctx.String("path"), workingDirectory)
	o, err := compile.NewOpts(workingDirectory)
	o.Target = ctx.String("target")
	log.AssertErr(err)
	log.AssertErr(compile.Compile(o))
	return nil
}

var runningCmd *exec.Cmd

func runCmd(ctx *cli.Context) error {
	log.LogVerbose = ctx.Bool("verbose")
	workingDirectory = util.Either(ctx.String("path"), workingDirectory)
	run(ctx)
	go func() {
		err := runningCmd.Wait()
		if err == nil {
			os.Exit(0)
		}
	}()
	return watch.Extensions(workingDirectory, []string{".cpp", ".hpp"}, func(s string) error {
		return run(ctx)
	}, 100)
}

func run(ctx *cli.Context) error {
	o, err := compile.NewOpts(workingDirectory)
	if err != nil {
		return err
	}
	o.Target = ctx.String("target")
	log.AssertErr(err)
	log.AssertErr(compile.Compile(o))
	if runningCmd != nil {
		_ = runningCmd.Process.Kill()
	}
	runningCmd = exec.Command(o.OutputPath)
	runningCmd.Stdout = os.Stdout
	runningCmd.Stderr = os.Stderr
	return runningCmd.Start()
}

func main() {
	app := &cli.App{
		Name:  "forg",
		Usage: "Build games",
		Action: func(c *cli.Context) error {
			cli.ShowAppHelp(c)
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
			&cli.StringFlag{
				Name:    "target",
				Aliases: []string{"t"},
				Usage:   "set build target (native|windows)",
			},
			&cli.BoolFlag{
				Name:    "verbose",
				Aliases: []string{"V"},
				Usage:   "enable verbose logging",
			},
		},
	}

	cli.AppHelpTemplate = "forg\n\n" + cli.AppHelpTemplate

	if err := app.Run(os.Args); err != nil {
		log.AssertErr(err)
	}
}
