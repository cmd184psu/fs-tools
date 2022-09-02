package main

import (
	"fmt"
	"io/ioutil"
	"net/http"
	"os"
	"strings"
)

func uploadFile(w http.ResponseWriter, r *http.Request) {
	var s string
	s = os.Getenv("HOME") + "/"
	fmt.Println("want to write to directory:" + s)
	fmt.Println("File Upload Endpoint Hit")

	// Parse our multipart form, 10 << 20 specifies a maximum
	// upload of 10 MB files.
	r.ParseMultipartForm(1000 << 20)
	// FormFile returns the first file for the given key `myFile`
	// it also returns the FileHeader so we can get the Filename,
	// the Header and the size of the file
	file, handler, err := r.FormFile("fileX")
	if err != nil {
		fmt.Println("Error Retrieving the File")
		fmt.Println(err)
		return
	}
	defer file.Close()
	fmt.Printf("Uploaded File: %+v\n", handler.Filename)
	fmt.Printf("File Size: %+v\n", handler.Size)
	fmt.Printf("MIME Header: %+v\n", handler.Header)

	// Create a temporary file within our temp-images directory that follows
	// a particular naming pattern
	f, err := os.Create(s + handler.Filename)
	if err != nil {
		fmt.Println(err)
	}
	defer f.Close()

	// read all of the contents of our uploaded file into a
	// byte array
	fileBytes, err := ioutil.ReadAll(file)
	if err != nil {
		fmt.Println(err)
	}
	// write this byte array to our temporary file
	f.Write(fileBytes)
	// return that we have successfully uploaded our file!
	fmt.Fprintf(w, "Successfully Uploaded File\n")
}

func installRPM(w http.ResponseWriter, r *http.Request) {
	var s string
	s = os.Getenv("HOME") + "/"
	fmt.Println("want to write to directory:" + s)
	fmt.Println("File Upload Endpoint Hit")

	fmt.Printf("RequestURI: %+v\n", r.RequestURI)

	rpmtoinstall := strings.Split(strings.Split(r.RequestURI, "?")[1], "=")[1]

	fmt.Printf("Method: %+v\n", r.Method)
	exe := "yum install " + s + rpmtoinstall + " -y"
	// return that we have successfully uploaded our file!
	fmt.Fprintf(w, "Successfully Install RPM "+exe+"\n")
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

func Hostname() (string, error) {
        var hostname string
        var err error
        if hostname, err = PopentoString("hostname -s"); err != nil {
                return "localhost", err
        }

        return strings.TrimSpace(hostname), nil
}

func ackBack(w http.ResponseWriter, r *http.Request) {
	var s string
	s = os.Getenv("HOME") + "/"
	fmt.Println("want to write to directory:" + s)
	fmt.Println("File Upload Endpoint Hit")
	fmt.Printf("RequestURI: %+v\n", r.RequestURI)
	fmt.Printf("Method: %+v\n", r.Method)
        var hostname,_:=Hostname()

	fmt.Fprintf(w, "ack back from "+hostname+"\n")
}



func setupRoutes() {
	http.HandleFunc("/upload", uploadFile)
	http.HandleFunc("/installrpm", installRPM)
        http.HandleFunc("/ack", ackBack)
	http.ListenAndServe(":8080", nil)
}

func main() {
	fmt.Println("Hello World")
	setupRoutes()
}
