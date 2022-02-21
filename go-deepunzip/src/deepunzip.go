package main

import (
	"flag"
	"fmt"

	//"log"
	"math/rand"
	"os"

	//"path/filepath"
	"runtime"
	"strconv"
	"strings"

	//"sync"
	"time"

	"path"
	"path/filepath"

	"archive/zip"

	"github.com/cmd184psu/fs-tools/fstools-gomod"
)

const debug = true

//var wg sync.WaitGroup

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
	walk            bool
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
	} else if l > 8 && fns.ext == ".enc" && fns.fullname[l-8:l] == ".tgz.enc" {
		fns.ext = ".tgz.enc"
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
	flag.BoolVar(&frs.walk, "walk", false, "walk the directory tree (EXPERIMENTAL)")
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

func singleFileProcessor(f string, frs *fileRenameStruct) error {
	debugPrint("now processing filename: " + f)

	fns := parseFilename(f)
	debugPrint("now processing filename: " + fns.fullname)
	//2020-09-26 23:11:25.28049781 -0400 EDT

	fileStat, err := os.Stat(fns.fullname)
	if err != nil {
		//wg.Done()

		return err
	}
	if !frs.redate {
		fns.modtime = fileStat.ModTime().Format("02Jan2006")
		debugPrint("modtime is " + fns.modtime)
	} else {
		fns.modtime = frs.modtimeOverride
	}
	newfilename := fns.fullname
	newfileBase := fns.base
	debugPrint("removedate: " + strconv.FormatBool(frs.removedate) + " filename has date: " + strconv.FormatBool(fns.hasDate))

	if frs.removedate && fns.hasDate {
		debugPrint("modtime: " + fns.modtime)
		newfileBase = fns.base[0 : len(fns.base)-(len(fns.modtime)+1)]
		newfilename = newfileBase + strings.ToLower(fns.ext)
	} else if fns.hasDate {
		newfilename = fns.base + strings.ToLower(fns.ext)
	}

	debugPrint("old base: " + fns.base)
	debugPrint("old filename is " + fns.fullname)
	debugPrint("new base: " + newfileBase)
	debugPrint("new filename is " + newfilename)

	if frs.adddate {
		if newfileBase[len(newfileBase)-1] != '-' {
			newfileBase = newfileBase + "-"
		}
		newfilename = newfileBase + fns.modtime + strings.ToLower(fns.ext)
	}

	//remove spaces in favor of dashes
	if frs.nospaces {
		newfilename = strings.ReplaceAll(newfilename, " ", "-")
	}

	output := "mv -i" + doVerboseMove(frs.verbose) + " \"" + fns.fullname + "\" \"" + fns.path + "/" + newfilename + "\""
	fmt.Println("if [ -e \"" + fns.fullname + "\" ]; then")
	fmt.Println("\t# execute:")
	//012345678
	//01Jan1980
	if frs.redate {
		touchtime, err := time.Parse("02Jan2006", fns.modtime)
		if err != nil {
			panic(err)
		}
		applydate := strconv.Itoa(touchtime.Year()) +
			ShortMonthNametoDigits(touchtime.Month().String()[:3]) +
			PadDay(strconv.Itoa(touchtime.Day())) + "0000"
		fmt.Println("\t" + doForce(frs.force) + "touch -mt " + applydate + " \"" + fns.fullname + "\"")
		fmt.Println("\t" + doForce(frs.force) + "touch -t " + applydate + " \"" + fns.fullname + "\"")
	}

	NormalizePermissions(fns.fullname, frs.force)

	//if source filename and tgt are the same (e.g. a file with a date and no spaces, but we plan on moving it)
	if fns.fullname != fns.path+"/"+newfilename && fns.fullname != newfilename {
		fmt.Println("\t" + doForce(frs.force) + output)
	}
	if frs.newpath != "" {
		if frs.newpath[len(frs.newpath)-1] != '/' {
			frs.newpath = frs.newpath + "/"
		}
		fmt.Println("\t" + doForce(frs.force) + "mkdir -p \"" + frs.newpath + "\"")
		NormalizePermissions(frs.newpath, frs.force)
		fmt.Println("\t" + doForce(frs.force) + "mv -i" + doVerboseMove(frs.verbose) + " \"" + fns.path + "/" + filepath.Base(newfilename) + "\" \"" + frs.newpath + "\"")
	}
	fmt.Println("fi")

	fmt.Printf("#Current Unix Time: %v\n", time.Now().Unix())
	rand.Seed(time.Now().UnixNano())
	n := rand.Intn(2) + 2 // n will be between 2 and 4

	debugPrint("sleeping for a number of seconds...")

	time.Sleep(time.Duration(n) * time.Second)

	fmt.Printf("# --> Current Unix Time: %v\n", time.Now().Unix())
	debugPrint("Work completed for file: " + f + "\n")
	//wg.Done()
	return nil
}

func getArchiveList(rootpath string) []*filenameStruct {

	list := make([]*filenameStruct, 0, 10)

	err := filepath.Walk(rootpath, func(path string, info os.FileInfo, err error) error {
		if info.IsDir() {
			return nil
		}
		debugPrint("found: " + path)

		fns := parseFilename(path)

		debugPrint("\tfile extension: " + fns.ext)

		//if filepath.Ext(path) == ".gz" || filepath.Ext(path) == ".tar.gz" || filepath.Ext(path) == ".tgz" {
		if fns.ext == ".tgz" || fns.ext == ".tar.gz" || fns.ext == ".gz" || fns.ext == ".zip" {
			debugPrint("\t appending---->" + fns.fullname)

			list = append(list, fns)
		}

		return nil
	})

	if err != nil {
		fmt.Printf("walk error [%v]\n", err)
	}
	return list
}

//func ExtractTarGz(gzipStream io.Reader) {
//    uncompressedStream, err := gzip.NewReader(gzipStream)
//    if err != nil {
//        log.Fatal("ExtractTarGz: NewReader failed")
//    }
//
//    tarReader := tar.NewReader(uncompressedStream)
//
//    for true {
//        header, err := tarReader.Next()
//
//
//      fmt.Println("\tworking on ===> ",header.Name)
////        if(header.Name=="./") {
////          continue
////        }
////
//        if err == io.EOF {
//            break
//        }
//
//        if err != nil {
//            log.Fatalf("ExtractTarGz: Next() failed: %s", err.Error())
//        }
//
//        switch header.Typeflag {
//        case tar.TypeDir:
//
//          //fmt.Println("directory, do nothing")
//          if header.Name!="./" {
//
//            if err := os.Mkdir(header.Name, 0755); err != nil {
//                log.Fatalf("ExtractTarGz: Mkdir() failed: %s", err.Error())
//            }
//          }
//        case tar.TypeReg:
//            outFile, err := os.Create(header.Name)
//            if err != nil {
//                log.Fatalf("ExtractTarGz: Create() failed: %s", err.Error())
//            }
//            if _, err := io.Copy(outFile, tarReader); err != nil {
//                log.Fatalf("ExtractTarGz: Copy() failed: %s", err.Error())
//            }
//            outFile.Close()
//
//        default:
//            log.Fatalf(
//                "ExtractTarGz: uknown type: %s in %s",
//                header.Typeflag,
//                header.Name)
//        }
//
//    }
//}

func untarInnerLoop(p *filenameStruct) {
	f, err := os.Open(p.fullname)
	if err != nil {
		fmt.Println(err)
		os.Exit(1)
	}
	defer f.Close()
	fstools.Untar(fstools.TrimSuffix(p.fullname, p.ext), f)
}

func gunzipInnerLoop(p *filenameStruct) {
	f, err := os.Open(p.fullname)
	if err != nil {
		fmt.Println(err)
		os.Exit(1)
	}
	defer f.Close()
	fstools.Gunzip(fstools.TrimSuffix(p.fullname, p.ext), f)
}

func unzipInnerLoop(p *filenameStruct) {

	f, err := zip.OpenReader(p.fullname)

	//f, err := os.Open(p.fullname)
	if err != nil {
		fmt.Println(err)
		os.Exit(1)
	}
	defer f.Close()
	fstools.Unzip(fstools.TrimSuffix(p.fullname, p.ext), f)
}

func main() {
	excess := 20
	l := 1

	for l > 0 {
		excess--
		list := getArchiveList(".")
		l = len(list)

		for i, p := range list {
			fmt.Println("#[%d:%s,%s===%s]\n", i, p.fullname, path.Dir(p.fullname), path.Base(p.fullname))
			fmt.Println("mkdir -p " + p.base)

			if p.ext == ".tgz" || p.ext == ".tar.gz" {
				fmt.Println("tar -xvpf " + p.fullname + " -C " + p.base)
				untarInnerLoop(p)

				fmt.Println("rm -rf " + p.fullname)

				err := os.Remove(p.fullname)
				if err != nil {
					fmt.Println(err)
					return
				}
				fmt.Println("File " + p.fullname + " successfully deleted")
			} else if p.ext == ".gz" {

				//fmt.Println("gunzip "+p.fullname)
				gunzipInnerLoop(p)
				err := os.Remove(p.fullname)
				if err != nil {
					fmt.Println(err)
					return
				}
			} else if p.ext == ".zip" {

				fmt.Println("****unzip " + p.fullname)
				unzipInnerLoop(p)
				err := os.Remove(p.fullname)
				if err != nil {
					fmt.Println(err)
					return
				}
			}

		}

		if excess < 0 {
			fmt.Println("bail early!!!!")
			os.Exit(1)
		}

	}
}

//func main() {
//	const VERSION = "Deep-unzip (c) C Delezenski <cmd184psu@gmail.com> - 1June2021"
//	frs := parseArgs()
//	if frs.ver {
//		fmt.Println(VERSION)
//		os.Exit(0)
//	}
//
//	//if frs.walk {
//    log.Println("begin walk...")
//	//walks the whole tree
//
//
//     err := filepath.Walk(".",
//			func(path string, info os.FileInfo, err error) error {
//				if err != nil {
//					return err
//				}
//
//				//process or do not process (if file, if certain criteria are met)
//
//				fileStat, err := os.Stat(path)
//				if err != nil {
//					return err
//				}
//
//				if fileStat.IsDir() {
//					fmt.Println("# Entering Directory: " + path)
//				} else {
//					fmt.Println("# is a file: " + path)
//					//wg.Add(1)
//					go singleFileProcessor(path, frs)
//				}
//				return nil
//			})
//		if err != nil {
//			log.Println(err)
//		}
//
//	} else {
//		//log.Println("begin non-walk...")
//
//		if frs.filename.fullname == "" {
//			fmt.Println("# Err: Missing parameter: filename")
//			os.Exit(1)
//		}
//		fileStat, err := os.Stat(frs.filename.fullname)
//
//		if err != nil {
//			//log.Fatal(err)
//			fmt.Println("# Err: " + frs.filename.fullname + " not found")
//			os.Exit(1)
//		} else if fileStat.IsDir() {
//			fmt.Println("# Err: " + frs.filename.fullname + " is a directory")
//			os.Exit(1)
//		} else {
//			err = singleFileProcessor(frs.filename.fullname, frs)
//		}
//	}
//	//wg.Wait()
//	//debugPrint("=== end of walk===")
//	//log.Println("The end")
//
//}
