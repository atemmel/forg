package watch

import (
	"fmt"
	"io/fs"
	"os"
	"strings"
	"time"
)

type Callback func(string) error

type state struct {
	dir        fs.FS
	extensions []string
	files      map[string]time.Time
}

func Extensions(directory string, extensions []string, fn Callback, interval time.Duration) error {
	state := &state{
		dir:        os.DirFS(directory),
		extensions: extensions,
		files:      map[string]time.Time{},
	}

	_ = poll(state, func(path string, d fs.DirEntry) error {
		i, err := d.Info()
		if err != nil {
			return nil
		}
		state.files[path] = i.ModTime()
		return nil
	})

	for {
		_ = poll(state, func(p string, d fs.DirEntry) error {
			i, err := d.Info()
			if err != nil {
				return nil
			}
			now := i.ModTime()
			before := state.files[p]

			if before.After(now) {
				fmt.Println(p, "was changed")
				state.files[p] = now
				//TODO: consider if error should be handled or not
				_ = fn(p)
			}
			return nil
		})

		time.Sleep(interval * time.Millisecond)
	}
}

func poll(state *state, callback func(string, fs.DirEntry) error) error {
	return fs.WalkDir(state.dir, ".", func(path string, d fs.DirEntry, err error) error {
		for _, ext := range state.extensions {
			if strings.HasSuffix(d.Name(), ext) {
				goto OK
			}
		}
		return nil
	OK:
		return callback(path, d)
	})
}
