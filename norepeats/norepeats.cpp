 /*
    norepeats.cpp - Seeks out all files that are identical based on an independent md5 hash.
    Prints to stdout suggested deletions.
    Copyright (C) 2020  Chris Delezenski

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <OpenStringLib.h>

#define VERSION "(Part of Dev Tools) Norepeats v1.0.1 - 15Nov20.1 - written by C Delezenski"


void usage() {
	cout<<"USAGE: norepeats -w [*.ext] generates a batch file to stdout, errors to stderr"<<endl;
}

long GetFileSize(cString filename)
{
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

cString ShowTime(time_t &t) {
	struct tm* buf2=NULL;	
	cString month,day,year;
	buf2=localtime(&t);
	if(buf2->tm_mon+1<10) month="0"+cString(buf2->tm_mon+1);
	else month=cString(buf2->tm_mon+1);
	if(buf2->tm_mday+1<10) day="0"+cString(buf2->tm_mday);
	else day=cString(buf2->tm_mday);
	if(buf2->tm_year-100<0) year=cString(buf2->tm_year);
	else if(buf2->tm_year-100<10) year="0"+cString(buf2->tm_year-100);
	else year=cString(buf2->tm_year-100);
	return month+"-"+day+"-"+year;
}

cString GetFileDateStamp(cString filename) {
	struct stat buf;
	stat(filename,&buf);	
	return ShowTime(buf.st_mtime);
}

cString BashFriendly(cString input) {
	cString output=input;
	cString charlist="\\ '\"[]()!#$@&";
	
	for(int i=0; i<charlist.Length(); i++) {
//		cerr<<"want to replace \""<<charlist[i]<<"\""<<endl;//
//		cerr<<"with \""<<"\\"+cString(charlist[i])<<"\""<<endl;
	
		output=output.Replace(charlist[i],"\\"+cString(charlist[i]));
	}
	return output;
}



cString gethash(cString filename) {
//	FILE*t=NULL;
	
	cString newfilename=filename;
	bool catchit=false;
//	if(filename.Contains('\'')) {/
//		catchit=true;/
//		newfilename=filename.Replace("\'","\\\'");
//	}
/*	if(filename.Contains('[')) {
		catchit=true;
		newfilename=filename.Replace("[","\\[");
	}
	if(filename.Contains(']')) {
		catchit=true;
		newfilename=filename.Replace("]","\\]");
	}
	*/
	//cString cmd="md5sum \""+newfilename+"\"";
	cString cmd="md5 \""+newfilename+"\"";
	//char tempcharstar[1024]="";
	if (catchit) {
		cerr<<"cmd="<<cmd<<endl;
	}
	
	//MD5 (whatever.txt) = b3ec0bd9b34f8508ee0376361ce5c281
	
	cStringList popencli;
	cString temp;
	popencli.FromPopen(cmd);

	cerr<<"# "<<popencli.ToString(' ')<<endl;
	temp=popencli.ToString(' ');
	
	popencli.FromString(temp,' ');
	cerr<<"# "<<popencli[-1]<<endl;

	if(catchit) exit(0);
	return popencli[-1];
}

void FileList_R(cStringList &mylist, cString fullpath, cString ending, bool everything) {
	if(fullpath[0]=='/') chdir("/");
	cString cmd="find \""+fullpath+"\" -type f -iname \""+ending+"\"";

	if(!everything) cmd+=" -size -1G";
	FILE *t=popen(cmd, "r" );
	mylist.FromFile(t);
	pclose(t);
	mylist.UCompact();
}

void FileandHashList(cDualList &mylist, cString fullpath, cString ending,bool everything) {
	cStringList temp;
	cString temphash;
	FileList_R(temp,fullpath,ending,everything);
	for(int i=0; i<temp.Length(); i++) {
		cerr<<"\rprocessing "<<i<<" of "<<temp.Length();
		temphash=gethash(temp[i]);
		//cerr<<"hash retrieved: "<<temphash<<endl;
		mylist[temp[i]]=temphash;
	}
}

void FileList_1(cStringList &mylist, cString fullpath, cString ending, bool everything) {
	if(fullpath[0]=='/') chdir("/");
	cString cmd="find \""+fullpath+"\" -follow -type f -maxdepth 1 -iname \""+ending+"\"";

	if(!everything) cmd+=" -size -1G";

	pclose(mylist.FromFile(popen(cmd, "r" )));
	mylist.UCompact();
}

void FileList_1(cStringList &mylist, cString fullpath, bool everything) {
	cStringList split;
	if(fullpath.Contains('*')) 
		split.FromString(fullpath,"*.");
	else
		split.FromString(fullpath,".");
		
	FileList_1(mylist,fullpath.ChopRt('/'),fullpath.ChopAllLf('/'), everything);
}

void FileList_R(cStringList &mylist, cString fullpath,bool everything) {
	cStringList split;
//	cerr<<"fullpath="<<fullpath<<endl;
	if(fullpath.Contains('*')) 
		split.FromString(fullpath,"*.");
	else
		split.FromString(fullpath,".");
	FileList_R(mylist,fullpath.ChopRt('/'),fullpath.ChopAllLf('/'),everything);
}
/*
void processcmdline(int argc,char *argv[], cProp & switches) {
	cString argument;
	cString total(argc);
	for(int i=1; i<argc; i++) {
		argument=cString(argv[i]).Trim();
		if(argument=="-h" || argument=="-help" || argument=="--help" || argument=="-usage") {
			usage();
			exit(0);
		}
		if(argument=="-v" || argument=="-ver" || argument=="--version") {
			cout<<VERSION<<endl;
			exit(0);
		}
		if(argument=="-man") {
			manpage("norepeats");
			exit(0);
		}
		if(argument=="-w") {
			switches.wildcard=argv[i+1];
			i++;		
		}
		if(argument.Contains('*')) switches.wildcard=argv[i];
	}
}*/
void progressBeginning(cString progressFile) {
	cout<<"echo \'0\' > "<<progressFile<<endl;
	//cout<<"tail -f "<<progressFile<<" | /usr/bin/zenity --progress --auto-close &"<<endl;
}

void progressDuring(cString progressFile, int cur, int n) {
	cout<<"echo \""<<int(float(cur)/float(n)*100.0)<<"\" > "<<progressFile<<endl;
}

void progressEnding(cString progressFile) {
	cout<<"echo 100 >>"<<progressFile<<endl;
	cout<<"rm -rvf "<<progressFile<<endl;
}

bool APreferredKeepOverB(cString a, cString b, cString hint) {
//	cerr<<"enter function"<<endl;

	if(hint!="") {
//		cerr<<"returning because a contains hint "<<hint<<endl;
	return a.Contains(hint);
	}
	if(b.Contains("copy") || b.Contains("-b.") || b.Contains("temp") || b.Contains("junk") || b.Contains("unsorted") || b.Contains("incoming") || b.Contains("newdir") || b.ToLower().Contains("frozen") || !b.Contains("expenses/")) {
///		cerr<<"returning because of something in b: "<<b<<endl;
		 return true;
	}
//	cerr<<" A is less than B "<<cString(a.Length()<b.Length())<<endl;
	return (a.Length()<b.Length());
}

void lookfordups(cString wildcard, cString hint, bool force, bool everything) {
	cDualList thelist;
	
	
	FileandHashList(thelist,"./",wildcard,everything);
		
	cString progressFile="norepeats-progress.txt";
	int hits=0;
	int donothing=3;
	progressBeginning(progressFile);
	for(int i=0; i<thelist.Length(); i++) {
		if(thelist[i]!="") {
				cerr<<endl<<"#\t filename: "<<BashFriendly(thelist.GetName(i))<<endl;
				cerr<<"#\t size: "<<GetFileSize(thelist.GetName(i))<<endl;
				cerr<<"#\t datestamp: "<<GetFileDateStamp(thelist.GetName(i))<<endl;
				

			for(int j=i+1; j<thelist.Length(); j++) {
				if(thelist[j]==""){ 
					donothing=3;
				}
				else if(j==i) {
					donothing=3;
				} else {

					if(i!=j && thelist.GetValFromIndex(i)[0]==thelist.GetValFromIndex(j)[0] && thelist.GetValFromIndex(i)[1]==thelist.GetValFromIndex(j)[1] && //) {

						//if(
							thelist.GetValFromIndex(i)==thelist.GetValFromIndex(j) && i!=j) {
							cout<<endl<<"# "<<thelist.GetName(i)<<" and "<<thelist.GetName(j)<<" contain the same data"<<endl;
							cout<<"# "<<thelist.GetValFromIndex(i)<<" == "<<thelist.GetValFromIndex(j)<<endl;
							if(force) cout<<"sudo ";



							if(APreferredKeepOverB(thelist.GetName(i),thelist.GetName(j),hint)) {
								cout<<"rm -rvf "<<BashFriendly(thelist.GetName(j))<<endl; 		
								thelist[j]="";
							} else {
								cout<<"rm -rvf "<<BashFriendly(thelist.GetName(i))<<endl;
								thelist[i]="";
							}
							progressDuring(progressFile,i+1,thelist.Length());
							hits++;
						} 
					//}
	
				} //end if	
			
				

				//cerr<<"\rProgress: "<<(i+1)<<" / "<<thelist.Length()<<" hits="<<hits<<"          " ;	
			} //end of for j

		}
		cerr<<"\rProgress: "<<(i+1)<<" / "<<thelist.Length()<<" hits="<<hits<<"          " ;	
	} //end of for i
	cout<<"# hits="<<hits<<endl;
	progressEnding(progressFile);
	
}


int main(int argc, char* argv[]) {

	cerr<<VERSION<<endl;
	cArgs arguments(argc,argv,"-");
	
	
	cString wildcard=arguments.GetArg("w",1);
	cString hint="";
	
	if(arguments.IsSet("h"))
		hint=arguments.GetArg("h",1);
	
	cStringList wildcardlist;
	
	cout<<"#!/bin/sh"<<endl;
	cerr<<"process"<<endl;
	
	
	
	if(wildcard=="") wildcard="*";
	
	//new option: -e gives you all files; without it, limit scans to < 1 GB files
	
	if(!wildcard.Contains(';')) {
		 lookfordups(wildcard,hint,arguments.IsSet("f"),arguments.IsSet("e"));
	} else {
		wildcardlist.FromString(wildcard,';');
		for(int i=0; i<wildcardlist.Length(); i++) lookfordups(wildcardlist[i],hint,arguments.IsSet("f"),arguments.IsSet("e"));
	}	
	
	return 0;
}






