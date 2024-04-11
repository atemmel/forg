package main

import (
	"flag"
	"fmt"
	"os"
)

var (
	path = "."
)

func die(args ...any) {
	fmt.Fprintln(os.Stderr, args...)
	os.Exit(1)
}

func init() {
	flag.StringVar(&path, "-p", ".", "Set path to project to operate on")
	flag.Parse()
}

func main() {
	op := flag.Arg(0)
	if op == "" {
		die("No operation specified...")
	}

	switch op {
	case "init":
		fmt.Println("init")
	}
}
