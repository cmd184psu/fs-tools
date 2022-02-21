package fileutils

import (
	"io/fs"
)

func walk(s string, d fs.DirEntry, err error) error {
	if err != nil {
		return err
	}
	if !d.IsDir() {
		println(s)
	}
	return nil
}

// func main() {
//    filepath.WalkDir("..", walk)
// }
