package compile

import (
	"fmt"
	"math"
	"os"
	"os/exec"
	"path"
)

type Unit struct {
	Path string
}

type Target struct {
	Units []Unit
}

func Compile(t *Target) error {
	nUnits := len(t.Units)
	for i, unit := range t.Units {
		logCompilation(i, nUnits, unit)
		err := compileUnit(unit)
		if err != nil {
			return err
		}
	}
	return nil
}

func logCompilation(current, all int, unit Unit) {
	filename := path.Base(unit.Path)
	progress := int(math.Round(float64(current + 1) / float64(all) * 100))
	fmt.Fprintf(os.Stderr, "[%3d%%] %s\n", progress, filename)
}

func compileUnit(unit Unit) error {
	cmd := exec.Command("g++", "-c", unit.Path)
	cmd.Stdout = os.Stdout
	cmd.Stderr = os.Stderr
	return cmd.Run()
}
