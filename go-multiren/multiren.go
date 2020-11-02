package main

import (
	"flag"
	"fmt"
	"os"
	"path/filepath"
	"runtime"
	"strconv"
	"strings"
	"time"
)

//FileContainsDate : f contains a date
func FileContainsDate(f string) bool {

	if len(f) < 7 {
		return false // shortcut, if base is less than 7, there's no date to look for
	}
	const max = 2021
	const min = 1995
	var datelist [max - min]int

	for i := 0; i < len(datelist); i++ {
		datelist[i] = i + min
	}

	for i := 0; i < len(datelist); i++ {
		for j := 1; j < 13; j++ {
			s := time.Month(j).String()
			lhs := f[len(f)-7 : len(f)]
			rhs := s[0:3] + strconv.Itoa(datelist[i])
			if lhs == rhs {
				//fmt.Println("found match - returning true: ", lhs)
				return true
			}
		}
	}
	return false
}

//NormalizePermissions : filename needs to be chowned and chmod'ed
func NormalizePermissions(filename string, force bool) {
	s := ""
	if force {
		s = "sudo "
	}
	//if on mac and force
	if runtime.GOOS == "darwin" && force {
		fmt.Println("\t" + s + "xattr -c \"" + filename + "\"")
		fmt.Println("\t" + s + "chmod -N \"" + filename + "\"")
	}
	//can happen on either linux or mac

	if force && os.Getenv("USER") != "" {
		g := ""
		if os.Getenv("GROUP") != "" {
			g = ":" + os.Getenv("GROUP")
		}
		fmt.Println("\t" + s + "chown " + os.Getenv("USER") + g + " \"" + filename + "\"")
	}
}

//ShortMonthNametoDigits : change short month to padded number
func ShortMonthNametoDigits(m string) string {
	ret := ""
	switch m {
	case "Jan":
		ret = "01"
	case "Feb":
		ret = "02"
	case "Mar":
		ret = "03"
	case "Apr":
		ret = "04"
	case "May":
		ret = "05"
	case "Jun":
		ret = "06"
	case "Jul":
		ret = "07"
	case "Aug":
		ret = "08"
	case "Sep":
		ret = "09"
	case "Oct":
		ret = "10"
	case "Nov":
		ret = "11"
	case "Dec":
		ret = "12"
	default:
		ret = "invalid"
	}
	return ret
}

//PadDay : add a zero to days 1 through 9
func PadDay(d string) string {
	if len(d) == 1 {
		return "0" + d
	}
	return d
}

//Step 1: take an argument
//step 2: stat a file, pulll date (first birth, then created, then last accessed)
//step 3: get filename, just filename and extension
//step 4: detect if date was already added
//step 5: construct new path, print mv statement to screen
//step 6: move file processor to function
func SingleFileProcessor(filename string) {

}

type fileRenameStruct struct {
	force    bool
	nospaces bool
	ar       map[string]string
}

func parseArgs() *fileRenameStruct {
	frs := new(fileRenameStruct)

	flag.BoolVar(&frs.nospaces, "nospaces", false, "replace spaces with dashes")

	flag.Parse()

	return frs
}

func main() {
	const VERSION = "Multi-renamer (c) C Delezenski <cmd184psu@gmail.com> - 2Nov2020"

	var filename string
	flag.StringVar(&filename, "filename", "", "file path to parse")

	var force bool
	flag.BoolVar(&force, "force", false, "use the force! (sudo)")

	var verbose bool
	flag.BoolVar(&verbose, "verbose", false, "verbose move")

	var ver bool
	flag.BoolVar(&ver, "ver", false, "show version")
	flag.BoolVar(&ver, "version", false, "show version")

	var removedate bool
	flag.BoolVar(&removedate, "removedate", false, "remove date from filename")
	flag.BoolVar(&removedate, "skipdate", false, "do not add date to filename")

	var newpath string
	flag.StringVar(&newpath, "newpath", "", "new file path to move into")

	var modtime string
	modtime = ""
	flag.StringVar(&modtime, "redate", "", "override the last modified date, must be of the form DDMMMYYYY, e.g. 01Jan1980")
	frs := parseArgs()
	if ver {
		fmt.Println(VERSION)
		os.Exit(0)
	}

	var adddate bool
	adddate = !removedate

	var redate bool
	redate = (modtime != "")
	if redate {
		removedate = true
	}

	if filename == "" {
		fmt.Println("Missing parameter: filename")
		os.Exit(1)
	}

	fileStat, err := os.Stat(filename)

	if err != nil {
		//log.Fatal(err)
		fmt.Println("# Err: " + filename + " not found")
		os.Exit(1)
	}

	if fileStat.IsDir() {
		fmt.Println("# Err: " + filename + " is a directory")
		os.Exit(1)
	}
	// fmt.Println("base: ", filepath.Base(filename))

	ext := filepath.Ext(filename)

	//tar.gz exception
	if len(filename) > 7 && ext == ".gz" && filename[len(filename)-7:len(filename)] == ".tar.gz" {
		ext = ".tar.gz"
	}
	if len(filename) > 8 && ext == ".bz2" && filename[len(filename)-8:len(filename)] == ".tar.bz2" {
		ext = ".tar.bz2"
	}

	basefilename := filepath.Base(filename)[0 : len(filepath.Base(filename))-len(ext)]

	path := filepath.Dir(filename)

	//fmt.Println("path=" + path)
	// fmt.Println("partial filename: ", basefilename)
	//2020-09-26 23:11:25.28049781 -0400 EDT
	if !redate {
		modtime = fileStat.ModTime().Format("02Jan2006")
	}

	newfilename := filename

	containsdate := FileContainsDate(basefilename)

	if removedate && containsdate {
		basefilename = basefilename[0 : len(basefilename)-(len(modtime)+1)]

		newfilename = basefilename + strings.ToLower(ext)
	} else if containsdate {
		newfilename = basefilename + strings.ToLower(ext)
	}

	if adddate {
		if basefilename[len(basefilename)-1] != '-' {
			basefilename = basefilename + "-"
		}
		newfilename = basefilename + modtime + strings.ToLower(ext)
	}

	//remove spaces in favor of dashes
	if frs.nospaces {
		newfilename = strings.ReplaceAll(newfilename, " ", "-")
	}
	//fmt.Println("# OS detected: " + runtime.GOOS)
	v := ""
	if verbose {
		v = "v"
	}
	output := "mv -i" + v + " \"" + filename + "\" \"" + path + "/" + newfilename + "\""

	fmt.Println("if [ -e \"" + filename + "\" ]; then")

	fmt.Println("\t# execute:")
	s := ""
	//if -force, then add sudo to everything to ensure it all works
	if force {
		s = "sudo "
	}
	//012345678
	//01Jan1980
	if redate {
		touchtime, err := time.Parse("02Jan2006", modtime)
		if err != nil {
			panic(err)
		}

		applydate := strconv.Itoa(touchtime.Year()) +
			ShortMonthNametoDigits(touchtime.Month().String()[:3]) +
			PadDay(strconv.Itoa(touchtime.Day())) + "0000"

		fmt.Println("\t" + s + "touch -mt " + applydate + " \"" + filename + "\"")
		fmt.Println("\t" + s + "touch -t " + applydate + " \"" + filename + "\"")
	}

	//if on mac and force
	// if runtime.GOOS == "darwin" && force {
	// 	fmt.Println("\t" + s + "xattr -c \"" + filename + "\"")
	// 	fmt.Println("\t" + s + "chmod -N \"" + filename + "\"")
	// }
	// //can happen on either linux or mac

	// if force && os.Getenv("USER") != "" {
	// 	g := ""
	// 	if os.Getenv("GROUP") != "" {
	// 		g = ":" + os.Getenv("GROUP")
	// 	}
	// 	fmt.Println("\t" + s + "chown " + os.Getenv("USER") + g + " \"" + filename + "\"")
	// }
	NormalizePermissions(filename, force)

	//if source filename and tgt are the same (e.g. a file with a date and no spaces, but we plan on moving it)
	if filename != path+"/"+newfilename && filename != newfilename {
		fmt.Println("\t" + s + output)
	}
	if newpath != "" {

		if newpath[len(newpath)-1] != '/' {
			newpath = newpath + "/"
		}

		fmt.Println("\t" + s + "mkdir -p \"" + newpath + "\"")

		// if force && os.Getenv("USER") != "" {
		// 	g := ""
		// 	if os.Getenv("GROUP") != "" {
		// 		g = ":" + os.Getenv("GROUP")
		// 	}
		// 	fmt.Println("\t" + s + "chown " + os.Getenv("USER") + g + " \"" + newpath + "\"")
		// }
		NormalizePermissions(newpath, force)
		fmt.Println("\t" + s + "mv -i" + v + " \"" + path + "/" + filepath.Base(newfilename) + "\" \"" + newpath + "\"")
	}

	fmt.Println("fi")
}
