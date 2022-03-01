package fstools

import (
	"bufio"
	"bytes"
	"errors"
	"fmt"
	"io"
	"log"
	"os"
	"os/exec"
	"path/filepath"
	"strings"
	"time"
)

func Touch(fileName string) {
	_, err := os.Stat(fileName)
	if os.IsNotExist(err) {
		file, err := os.Create(fileName)
		if err != nil {
			log.Fatal(err)
		}
		defer file.Close()
	} else {
		currentTime := time.Now().Local()
		err = os.Chtimes(fileName, currentTime, currentTime)
		if err != nil {
			fmt.Println(err)
		}
	}
}

func System3(cmd string) error {
	arglist := strings.Split(cmd, " ")
	//app:=arglist[0]

	var b bytes.Buffer
	if err := Popen3(&b,
		exec.Command(arglist[0], arglist[1:]...),
	); err != nil {
		log.Fatalln(err)
		return err
	}
	io.Copy(os.Stdout, &b)
	return nil
}

//was "readlines"
func Grep(path string, musthave string, mustnothave string) ([]string, error) {
	file, err := os.Open(path)
	if err != nil {
		return nil, err
	}
	defer file.Close()

	var lines []string
	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		if (musthave == "" || strings.Contains(scanner.Text(), musthave)) &&
			(mustnothave == "" || !strings.Contains(scanner.Text(), mustnothave)) {
			lines = append(lines, scanner.Text())
		}
	}
	return lines, scanner.Err()
}

//formerly Execute
func Popen3(output_buffer *bytes.Buffer, stack ...*exec.Cmd) (err error) {
	var error_buffer bytes.Buffer
	pipe_stack := make([]*io.PipeWriter, len(stack)-1)
	i := 0
	for ; i < len(stack)-1; i++ {
		stdin_pipe, stdout_pipe := io.Pipe()
		stack[i].Stdout = stdout_pipe
		stack[i].Stderr = &error_buffer
		stack[i+1].Stdin = stdin_pipe
		pipe_stack[i] = stdout_pipe
	}
	stack[i].Stdout = output_buffer
	stack[i].Stderr = &error_buffer

	if err := call(stack, pipe_stack); err != nil {
		log.Fatalln(string(error_buffer.Bytes()), err)
	}
	return err
}

func call(stack []*exec.Cmd, pipes []*io.PipeWriter) (err error) {
	if stack[0].Process == nil {
		if err = stack[0].Start(); err != nil {
			return err
		}
	}
	if len(stack) > 1 {
		if err = stack[1].Start(); err != nil {
			return err
		}
		defer func() {
			if err == nil {
				pipes[0].Close()
				err = call(stack[1:], pipes[1:])
			}
		}()
	}
	return stack[0].Wait()
}

func FileExists(filename string) (bool, error) {
	_, err := os.Stat(filename)
	if err == nil {
		return true, nil
	}
	if errors.Is(err, os.ErrNotExist) {
		return false, nil
	}
	return false, err
}

func PopentoString(cmd string) (string, error) {
	arglist := strings.Split(cmd, " ")
	//app:=arglist[0]

	var b bytes.Buffer
	if err := Popen3(&b,
		exec.Command(arglist[0], arglist[1:]...),
		exec.Command("head", "-1"),
	); err != nil {
		log.Fatalln(err)
	}
	return b.String(), nil
}

func PopentoStringAwk(cmd string, awk int) (string, error) {
	arglist := strings.Split(cmd, " ")
	//app:=arglist[0]

	awkstatement := fmt.Sprintf("{print $%d}'", awk)
	var b bytes.Buffer
	if err := Popen3(&b,
		exec.Command(arglist[0], arglist[1:]...),
		exec.Command("head", "-1"),
		exec.Command("awk", awkstatement),
	); err != nil {
		log.Fatalln(err)
	}
	return b.String(), nil
}

func DmidecodeProduct() (string, error) {
	var b bytes.Buffer
	if err := Popen3(&b,
		exec.Command("dmidecode"),
		exec.Command("grep", "-i", "product"),
		exec.Command("head", "-1"),
		exec.Command("awk", "{print $3}"),
	); err != nil {
		log.Fatalln(err)
	}
	return b.String(), nil
}

func Copy(src, dst string) (int64, error) {
	sourceFileStat, err := os.Stat(src)
	if err != nil {
		return 0, err
	}

	if !sourceFileStat.Mode().IsRegular() {
		return 0, fmt.Errorf("%s is not a regular file", src)
	}

	source, err := os.Open(src)
	if err != nil {
		return 0, err
	}
	defer source.Close()

	destination, err := os.Create(dst)
	if err != nil {
		return 0, err
	}
	defer destination.Close()
	nBytes, err := io.Copy(destination, source)
	return nBytes, err
}

func Move(src, dst string) error {
	return os.Rename(src, dst)
}

func GetFirstFile(rootpath string, hint string) string {

	var result string

	err := filepath.Walk(rootpath, func(path string, info os.FileInfo, err error) error {
		if info.IsDir() {
			return nil
		}

		if strings.HasSuffix(path, hint) {
			//VerbosePrintln("found: " + path)
			result = path
		}

		return nil
	})

	if err != nil {
		fmt.Printf("walk error [%v]\n", err)
	}
	return result
}

func ExecToFile(cli string, ofile string) error {
	arglist := strings.Split(cli, " ")
	cmd := exec.Command(arglist[0], arglist[1:]...)

	// open the out file for writing
	outfile, err := os.Create(ofile)
	if err != nil {
		panic(err)
	}
	defer outfile.Close()
	cmd.Stdout = outfile

	err = cmd.Start()
	if err != nil {
		return err
	}
	cmd.Wait()
	return nil
}
