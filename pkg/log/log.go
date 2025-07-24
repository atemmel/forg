package log

import (
	"fmt"
	"os"
)

func Stderr(str string, args ...any) {
	fmt.Fprintf(os.Stderr, str, args...)
}

func AssertErr(err error) {
	if err != nil {
		panic(err)
	}
}

func Assert(condition bool) {
	if !condition {
		panic("Assertion failed")
	}
}

func Die(code int, str string, args ...any) {
	Stderr(str, args...)
	os.Exit(code)
}
