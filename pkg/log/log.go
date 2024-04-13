package log

import (
	"fmt"
	"os"
)

func Stderr(str string, args ...any) {
	fmt.Fprintf(os.Stderr, str, args...)
}

func Assert(err error) {
	if err != nil {
		panic(err)
	}
}

func Die(code int, str string, args ...any) {
	Stderr(str, args...)
	os.Exit(code)
}
