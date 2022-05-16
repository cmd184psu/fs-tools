package fstools

import (
	"bufio"
	"bytes"
	"errors"
	"fmt"
	"io"
	"io/ioutil"
	"log"
	"os"
	"os/exec"
	"path/filepath"
	"strconv"
	"strings"
	"sync"
	"time"

	"golang.org/x/crypto/ssh"
)

const private_ssh_key_default = "%s/.ssh/id_rsa"

var ssh_key = ""

func GetPrivateSSHKey() string {
	if strings.EqualFold(ssh_key, "") {
		ssh_key = fmt.Sprintf(private_ssh_key_default, os.Getenv("HOME"))
	}
	return ssh_key
}

func SetPrivateSSHKey(newkey string) {
	ssh_key = newkey
}

func Touch(fileName string) error {
	_, err := os.Stat(fileName)
	if os.IsNotExist(err) {
		file, err := os.Create(fileName)
		defer file.Close()
		return err
	} else {
		currentTime := time.Now().Local()
		return os.Chtimes(fileName, currentTime, currentTime)
	}
}

func System3(cmd string) (err error) {
	arglist := strings.Split(cmd, " ")
	var b bytes.Buffer
	err = Popen3(&b,
		exec.Command(arglist[0], arglist[1:]...),
	)
	io.Copy(os.Stdout, &b)
	return
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

func ReadToSlice(filename string, force bool) ([]string, error) {
	if b, err := FileExists(filename); err != nil || !b {
		if err != nil && !force {
			//file existance test fails entirely ( doesn't mean file doesn't exist )
			return make([]string, 0), err

		}
		if !b && !force {
			//file does not exist, but we want to throw an error indicating as such
			return make([]string, 0), errors.New("file not found")
		}
		//ignore the fact that the file doesn't exist and return an empty array, quietly
		return make([]string, 0), nil
	}

	file, err := os.Open(filename)
	if err != nil {
		return nil, err
	}
	defer file.Close()

	var lines []string
	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		lines = append(lines, scanner.Text())
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

	// if err := call(stack, pipe_stack); err != nil {
	// 	log.Fatalln(string(error_buffer.Bytes()), err)
	// }
	// return err
	return call(stack, pipe_stack)
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

func Popen3GrepFast(cmd string, musthave string, mustnothave string) ([]string, error) {
	var b bytes.Buffer
	arglist := strings.Split(cmd, " ")

	//check for len<2 and return null array

	var greplist []string
	if len(musthave) != 0 {
		greplist = strings.Split(musthave, "&")
	}
	var antigreplist []string
	if len(mustnothave) != 0 {
		antigreplist = strings.Split("-v "+mustnothave, "&")
	}
	//app:=arglist[0]
	var err error

	//case 1: popen and grep
	if len(greplist) > 0 && len(antigreplist) == 0 {
		if err = Popen3(&b,
			exec.Command(arglist[0], arglist[1:]...),
			exec.Command("grep", greplist[0:]...),
		); err != nil {
			log.Fatalln(err)
		}
	} else if len(greplist) == 0 && len(antigreplist) > 0 {

		//case 2: popen and antigrep
		if err = Popen3(&b,
			exec.Command(arglist[0], arglist[1:]...),
			exec.Command("grep", antigreplist[0:]...),
		); err != nil {
			log.Fatalln(err)
		}
	} else if len(greplist) > 0 && len(antigreplist) > 0 {
		//case 3: popen, grep and antigrep

		if err = Popen3(&b,
			exec.Command(arglist[0], arglist[1:]...),
			exec.Command("grep", greplist[0:]...),
			exec.Command("grep", antigreplist[0:]...),
		); err != nil {
			log.Fatalln(err)
		}
	}
	var bstring = b.String()
	var slice []string
	if len(strings.TrimSpace(bstring)) != 0 {
		slice = strings.Split(b.String(), "\n")
	}
	if len(slice) < 2 {
		return slice, nil
	}
	return slice[:len(slice)-1], nil
}

func Popen3Grep(cmd string, musthave string, mustnothave string) ([]string, error) {
	var b bytes.Buffer
	arglist := strings.Split(cmd, " ")

	//check for len<2 and return null array
	var slice []string
	var greplist []string
	if len(musthave) != 0 {
		greplist = strings.Split(musthave, "&")
	}
	var antigreplist []string
	if len(mustnothave) != 0 {
		antigreplist = strings.Split(mustnothave, "&")
	}
	//app:=arglist[0]
	var err error

	if err = Popen3(&b, exec.Command(arglist[0], arglist[1:]...)); err != nil {
		return slice, err
	}

	var bstring = b.String()

	if len(strings.TrimSpace(bstring)) != 0 {
		slice = strings.Split(b.String(), "\n")
	}

	newslice := make([]string, 0)
	for i := 0; i < len(slice); i++ {
		//case 1: there are neither musthaves nor mustnothaves

		if len(strings.TrimSpace(slice[i])) != 0 {

			if len(greplist) == 0 && len(antigreplist) == 0 {
				//not really grep at all.. just expensive popen
				newslice = append(newslice, slice[i])
			} else if len(greplist) > 0 && len(antigreplist) == 0 {
				// only look at musthaves
				if SliceContains(greplist, slice[i]) {
					newslice = append(newslice, slice[i])
				}
			} else if len(musthave) == 0 && len(antigreplist) > 0 {
				if !SliceContains(antigreplist, slice[i]) {
					newslice = append(newslice, slice[i])
				}
			} else {
				if SliceContains(greplist, slice[i]) && !SliceContains(antigreplist, slice[i]) {
					newslice = append(newslice, slice[i])
				}
			}
		}
	}

	return newslice, nil
}

// func Popen3DoubleGrep(cmd string, musthave string) ([]string, error) {
// 	var b bytes.Buffer
// 	arglist := strings.Split(cmd, " ")

// 	//check for len<2 and return null array

// 	var greplist []string
// 	if len(musthave) != 0 {
// 		greplist = strings.Split(musthave, "&")
// 	}

// 	//app:=arglist[0]
// 	var err error

// 	//case 1: popen and grep
// 	if len(greplist) == 2 {
// 		if err = Popen3(&b,
// 			exec.Command(arglist[0], arglist[1:]...),
// 			exec.Command("grep", greplist[0]),
// 			exec.Command("grep", greplist[1]),
// 		); err != nil {
// 			log.Fatalln(err)
// 		}
// 	} else {
// 		return make([]string, 0), &os.SyscallError{}
// 	}
// 	slice := strings.Split(b.String(), "\n")
// 	return slice[:len(slice)-1], nil
// }

func SSHPopenToString(hostname string, command string) (string, error) {
	client, session, err := getsshclient(hostname)
	if err != nil {
		//panic(err)
		return "", err
	}
	out, err := session.CombinedOutput(command)
	if err != nil {
		//panic(err)
		return "", err
	}
	//fmt.Println(string(out))
	client.Close()
	return strings.TrimSpace(string(out)), nil
}

func getsshclient(host string) (*ssh.Client, *ssh.Session, error) {

	key, err := ioutil.ReadFile(GetPrivateSSHKey())
	if err != nil {
		log.Fatalf("unable to read private key: %v", err)
	}

	// Create the Signer for this private key.
	signer, err := ssh.ParsePrivateKey(key)
	if err != nil {
		log.Fatalf("unable to parse private key: %v", err)
	}
	sshConfig := &ssh.ClientConfig{
		User: os.Getenv("USER"),
		Auth: []ssh.AuthMethod{
			ssh.PublicKeys(signer),
		},
	}
	sshConfig.SetDefaults()
	sshConfig.HostKeyCallback = ssh.InsecureIgnoreHostKey()

	client, err := ssh.Dial("tcp", host+":22", sshConfig)
	if err != nil {
		return nil, nil, err
	}

	session, err := client.NewSession()
	if err != nil {
		client.Close()
		return nil, nil, err
	}

	return client, session, nil
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
	var err error
	var outfile *os.File
	cmd := exec.Command(arglist[0], arglist[1:]...)

	// open the out file for writing
	if !strings.EqualFold(ofile, "") {
		outfile, err = os.Create(ofile)
		if err != nil {
			panic(err)
		}
		defer outfile.Close()
		cmd.Stdout = outfile
	}
	err = cmd.Start()
	cmd.Wait()
	return err
}

func Spinny(sigChan chan bool) {
	quit := false
	for !quit {

		s := "|/-\\"
		for i := 0; i < len(s); i++ {
			fmt.Printf("%c", s[i])
			time.Sleep(100 * time.Millisecond)
			fmt.Printf("\b")
		}

		select {
		// case msg1 := <-messages:
		// 	//fmt.Println("received", msg1)
		// 	quit = true
		case sig := <-sigChan:
			quit = sig //fmt.Println("received signal", sig)
		default:
			//fmt.Println("not yet")
		}

	}
	fmt.Printf(" \b")
}

func System3AndSpin(cmd string, redirect string) (err error) {
	//var errorRec error
	var wg sync.WaitGroup
	wg.Add(1)
	//messages := make(chan string)
	sigChan := make(chan bool)
	errorChan := make(chan error)
	go func() {
		defer wg.Done()
		defer close(errorChan)
		defer close(sigChan)
		e := ExecToFile(cmd, redirect)
		sigChan <- true
		errorChan <- e
	}()
	go Spinny(sigChan)
	//errorRec = <-errorChan
	err = <-errorChan
	wg.Wait()
	return err
}

func Error2ExitCode(err error) int {
	//fmt.Printf("err was %q\n", err.Error())
	if strings.HasPrefix(err.Error(), "exit status ") {
		r, _ := strconv.ParseInt(err.Error()[12:], 10, 32)
		return int(r)
	}
	return 255
}

func Hostname() (string, error) {
	var hostname string
	var err error
	if hostname, err = PopentoString("hostname -s"); err != nil {
		return "localhost", err
	}

	return strings.TrimSpace(hostname), nil
}
