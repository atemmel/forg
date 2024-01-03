package watch

import (
	"os"
	"time"
)

type Callback func(string) error

type state struct {
	files map[string]time.Time
	fn    Callback
}

func Files(files []string, fn Callback, interval time.Duration) error {

	state := &state{
		files: map[string]time.Time{},
	}

	for _, file := range files {
		info, err := os.Stat(file)
		if err != nil {
			return err
		}
		state.files[file] = info.ModTime()
	}

	for {

		err := poll(state)
		if err != nil {
			return err
		}

		time.Sleep(interval * time.Millisecond)
	}
}

func poll(state *state) error {
	for file, timestamp := range state.files {
		info, err := os.Stat(file)
		if err != nil {
			if os.IsNotExist(err) {
				delete(state.files, file)
				continue
			}
			return err
		}

		latest := info.ModTime()

		if latest.After(timestamp) {
			state.files[file] = latest

			//TODO: consider if error should be handled or not
			err := state.fn(file)
			_ = err
		}
	}
	return nil
}
