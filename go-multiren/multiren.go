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

const debug = true

func debugPrint(s string) {
	if debug {
		fmt.Println("# " + s)
	}
}

//FileBaseContainsDate : f contains a date
func FileBaseContainsDate(f string) bool {

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
				debugPrint("# found match - returning true: " + lhs)
				return true
			}
		}
	}

	fmt.Println("# exhausted all options, returning false")
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

func singleFileProcessor(filename string) {
	debugPrint("now processing filename: " + filename)
}

type fileRenameStruct struct {
	force           bool
	nospaces        bool
	filename        *filenameStruct
	verbose         bool
	ver             bool
	removedate      bool
	newpath         string
	modtimeOverride string
	adddate         bool
	redate          bool
}

type filenameStruct struct {
	fullname string
	base     string
	path     string
	ext      string
	hasDate  bool
	modtime  string
}

func parseFilename(f string) *filenameStruct {
	fns := new(filenameStruct)
	fns.fullname = f

	fns.ext = filepath.Ext(fns.fullname)

	l := len(fns.fullname)

	//tar.gz and tar.bz2 exception
	if l > 7 && fns.ext == ".gz" && fns.fullname[l-7:l] == ".tar.gz" {
		fns.ext = ".tar.gz"
	} else if l > 8 && fns.ext == ".bz2" && fns.fullname[l-8:l] == ".tar.bz2" {
		fns.ext = ".tar.bz2"
	}

	fns.base = filepath.Base(fns.fullname)[0 : len(filepath.Base(fns.fullname))-len(fns.ext)]

	fns.path = filepath.Dir(fns.fullname)

	fns.hasDate = FileBaseContainsDate(fns.base)

	debugPrint("=====>filename " + f + " has date: " + strconv.FormatBool(fns.hasDate))

	return fns
}

func doForce(f bool) string {
	if f {
		return "sudo "
	}
	return ""
}

func doVerboseMove(v bool) string {
	if v {
		return "v"
	}
	return ""
}

func parseArgs() *fileRenameStruct {
	frs := new(fileRenameStruct)

	flag.BoolVar(&frs.nospaces, "nospaces", false, "replace spaces with dashes")
	var filename string
	filename = ""
	flag.StringVar(&filename, "filename", "", "file path to parse")
	flag.BoolVar(&frs.force, "force", false, "use the force! (sudo)")
	flag.BoolVar(&frs.verbose, "verbose", false, "verbose move")
	flag.BoolVar(&frs.ver, "ver", false, "show version")
	flag.BoolVar(&frs.ver, "version", false, "show version")
	flag.BoolVar(&frs.removedate, "removedate", false, "remove date from filename")
	flag.BoolVar(&frs.removedate, "skipdate", false, "do not add date to filename")
	flag.StringVar(&frs.newpath, "newpath", "", "new file path to move into")
	flag.StringVar(&frs.modtimeOverride, "redate", "", "override the last modified date, must be of the form DDMMMYYYY, e.g. 01Jan1980")
	flag.Parse()

	frs.adddate = !frs.removedate

	frs.redate = (frs.modtimeOverride != "")
	frs.filename = parseFilename(filename)
	if frs.redate {
		frs.removedate = true
	}

	return frs
}

func main() {
	const VERSION = "Multi-renamer (c) C Delezenski <cmd184psu@gmail.com> - 2Nov2020"

	frs := parseArgs()
	if frs.ver {
		fmt.Println(VERSION)
		os.Exit(0)
	}

	if frs.filename.fullname == "" {
		fmt.Println("# Err: Missing parameter: filename")
		os.Exit(1)
	}

	fileStat, err := os.Stat(frs.filename.fullname)

	if err != nil {
		//log.Fatal(err)
		fmt.Println("# Err: " + frs.filename.fullname + " not found")
		os.Exit(1)
	}

	if fileStat.IsDir() {
		fmt.Println("# Err: " + frs.filename.fullname + " is a directory")
		os.Exit(1)
	}
	//2020-09-26 23:11:25.28049781 -0400 EDT
	if !frs.redate {
		frs.filename.modtime = fileStat.ModTime().Format("02Jan2006")
		debugPrint("modtime is " + frs.filename.modtime)
	} else {
		frs.filename.modtime = frs.modtimeOverride
	}

	newfilename := frs.filename.fullname
	newfileBase := frs.filename.base
	debugPrint("removedate: " + strconv.FormatBool(frs.removedate) + " filename has date: " + strconv.FormatBool(frs.filename.hasDate))

	if frs.removedate && frs.filename.hasDate {
		debugPrint("modtime: " + frs.filename.modtime)
		newfileBase = frs.filename.base[0 : len(frs.filename.base)-(len(frs.filename.modtime)+1)]
		newfilename = newfileBase + strings.ToLower(frs.filename.ext)
	} else if frs.filename.hasDate {
		newfilename = frs.filename.base + strings.ToLower(frs.filename.ext)
	}

	debugPrint("old base: " + frs.filename.base)
	debugPrint("old filename is " + frs.filename.fullname)
	debugPrint("new base: " + newfileBase)
	debugPrint("new filename is " + newfilename)

	if frs.adddate {
		if newfileBase[len(newfileBase)-1] != '-' {
			newfileBase = newfileBase + "-"
		}
		newfilename = newfileBase + frs.filename.modtime + strings.ToLower(frs.filename.ext)
	}

	//remove spaces in favor of dashes
	if frs.nospaces {
		newfilename = strings.ReplaceAll(newfilename, " ", "-")
	}

	output := "mv -i" + doVerboseMove(frs.verbose) + " \"" + frs.filename.fullname + "\" \"" + frs.filename.path + "/" + newfilename + "\""
	fmt.Println("if [ -e \"" + frs.filename.fullname + "\" ]; then")
	fmt.Println("\t# execute:")

	//012345678
	//01Jan1980
	if frs.redate {
		touchtime, err := time.Parse("02Jan2006", frs.filename.modtime)
		if err != nil {
			panic(err)
		}

		applydate := strconv.Itoa(touchtime.Year()) +
			ShortMonthNametoDigits(touchtime.Month().String()[:3]) +
			PadDay(strconv.Itoa(touchtime.Day())) + "0000"

		fmt.Println("\t" + doForce(frs.force) + "touch -mt " + applydate + " \"" + frs.filename.fullname + "\"")
		fmt.Println("\t" + doForce(frs.force) + "touch -t " + applydate + " \"" + frs.filename.fullname + "\"")
	}

	NormalizePermissions(frs.filename.fullname, frs.force)

	//if source filename and tgt are the same (e.g. a file with a date and no spaces, but we plan on moving it)
	if frs.filename.fullname != frs.filename.path+"/"+newfilename && frs.filename.fullname != newfilename {
		fmt.Println("\t" + doForce(frs.force) + output)
	}
	if frs.newpath != "" {
		if frs.newpath[len(frs.newpath)-1] != '/' {
			frs.newpath = frs.newpath + "/"
		}
		fmt.Println("\t" + doForce(frs.force) + "mkdir -p \"" + frs.newpath + "\"")
		NormalizePermissions(frs.newpath, frs.force)
		fmt.Println("\t" + doForce(frs.force) + "mv -i" + doVerboseMove(frs.verbose) + " \"" + frs.filename.path + "/" + filepath.Base(newfilename) + "\" \"" + frs.newpath + "\"")
	}
	fmt.Println("fi")
}
