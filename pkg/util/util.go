package util

import (
	"os"
	"os/exec"
	"runtime"
	"strings"
)

func Run(args []string) error {
	cmd := exec.Command(args[0], args[1:]...)
	cmd.Stdout = os.Stdout
	cmd.Stderr = os.Stderr
	return cmd.Run()
}

func Either(a, b string) string {
	if a == "" {
		return b
	}
	return a
}

func InPath(cmd string) bool {
	_, err := exec.LookPath(cmd)
	return err == nil
}

func Exists(filepath string) bool {
	if _, err := os.Stat(filepath); err != nil {
		return false
	}
	return true
}

func CleanTarget(target string) string {
	if target == "" {
		target = runtime.GOOS
	} else if strings.Contains(target, "windows") {
		target = "windows"
	} else if strings.Contains(target, "linux") {
		target = "linux"
	}
	return target
}
