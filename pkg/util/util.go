package util

import (
	"os"
	"os/exec"
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
