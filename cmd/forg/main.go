package main

import (
	"embed"
	"fmt"
	"forg/pkg/compile"
	"forg/pkg/log"
	"forg/pkg/util"
	"forg/pkg/watch"
	"os"
	"os/exec"
	"path/filepath"

	"github.com/goccy/go-yaml"
	"github.com/urfave/cli/v2" // imports as package "cli"
)

type config struct {
	Always innerConf
	Modes  map[string]innerConf
}

var (
	workingDirectory = "."

	//go:embed template/*
	templateProject embed.FS

	conf config
)

func init() {
	var err error
	workingDirectory, err = os.Getwd()
	log.AssertErr(err)
	bytes, err := os.ReadFile("./forg.yaml")
	if err == nil {
		yaml.Unmarshal(bytes, &conf)
		conf.Always.CompileFlags = util.Either(conf.Always.CompileFlags, "-O0 -g -I./include")
		conf.Always.LinkerFlags = util.Either(conf.Always.LinkerFlags, "-rdynamic -lraylib")
	}
	fmt.Println(conf)
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

func initFromFlag(ctx *cli.Context) {
	log.LogVerbose = ctx.Bool("verbose")
	workingDirectory = util.Either(ctx.String("path"), workingDirectory)
}

func buildCmd(ctx *cli.Context) error {
	initFromFlag(ctx)
	o, err := compile.NewOpts(workingDirectory)
	o.Target = ctx.String("target")
	log.AssertErr(err)
	log.AssertErr(compile.Compile(o))
	return nil
}

var runningCmd *exec.Cmd

func runCmd(ctx *cli.Context) error {
	initFromFlag(ctx)
	run(ctx)
	go func() {
		err := runningCmd.Wait()
		if err == nil {
			os.Exit(0)
		}
	}()
	if ctx.Bool("watch") {
		return watch.Extensions(workingDirectory, []string{".cpp", ".hpp"}, func(s string) error {
			runningCmd.Process.Kill()
			return run(ctx)
		}, 100)
	}
	return nil
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

type innerConf struct {
	CompileFlags string `yaml:"compile-flags"`
	LinkerFlags  string `yaml:"linker-flags"`
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
				Flags: []cli.Flag{
					&cli.BoolFlag{
						Name:    "watch",
						Aliases: []string{"w"},
						Usage:   "watch for continuous rebuilds",
					},
				},
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
