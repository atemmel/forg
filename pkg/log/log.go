package log

import (
	"fmt"
	"os"
)

func Error(str string, args ...any) {
	fmt.Fprintf(os.Stderr, str, args...)
}
