 /*
    unzipall.cpp - Seeks out all files ending in an extension, creates a folder and unzips that file in the folder, then removes the
    zip file.
    Prints to stdout suggested file operations.
    Copyright (C) 2011  Chris Delezenski

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
//using namespace std;

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <OpenStringLib.h>

#ifndef __USE__CVS__
#define VERSION "(Part of Dev Tools) Unzipall v0.01 - 29Sept12 - written by Chris Delezenski"
#else
#define VERSION "$id$ - written by Chris Delezenski"
#endif 


void usage() {
	cout<<"USAGE: norepeats -w [*.ext] generates a batch file to stdout, errors to stderr"<<endl;
}

cString gethash(cString filename) {
	FILE*t=NULL;
	cString cmd="md5sum \""+filename+"\"";
	char tempcharstar[1024]="";
	t=popen(cmd,"r");
	fscanf(t,"%s",tempcharstar);
	
	pclose(t);
	return cString(tempcharstar);
}

void FileList_R(cStringList &mylist, cString fullpath, cString ending) {
	if(fullpath[0]=='/') chdir("/");
	cString cmd="find \""+fullpath+"\" -type f -iname \""+ending+"\"";
	FILE *t=popen(cmd, "r" );
	mylist.FromFile(t);
	pclose(t);
	mylist.UCompact();
}

void FileandHashList(cDualList &mylist, cString fullpath, cString ending) {
	cStringList temp;
	cString temphash;
	FileList_R(temp,fullpath,ending);
	for(int i=0; i<temp.Length(); i++) {
		cerr<<"\rprocessing "<<i<<" of "<<temp.Length();
		//temphash=gethash(temp[i]);
		mylist[temp[i]]="hash";
	}
}

void FileList_1(cStringList &mylist, cString fullpath, cString ending) {
	if(fullpath[0]=='/') chdir("/");
	cString cmd="find \""+fullpath+"\" -follow -type f -maxdepth 1 -iname \""+ending+"\"";
	pclose(mylist.FromFile(popen(cmd, "r" )));
	mylist.UCompact();
}

void FileList_1(cStringList &mylist, cString fullpath) {
	cStringList split;
	if(fullpath.Contains('*')) 
		split.FromString(fullpath,"*.");
	else
		split.FromString(fullpath,".");
		
	FileList_1(mylist,fullpath.ChopRt('/'),fullpath.ChopAllLf('/'));
}

void FileList_R(cStringList &mylist, cString fullpath) {
	cStringList split;
//	cerr<<"fullpath="<<fullpath<<endl;
	if(fullpath.Contains('*')) 
		split.FromString(fullpath,"*.");
	else
		split.FromString(fullpath,".");
	FileList_R(mylist,fullpath.ChopRt('/'),fullpath.ChopAllLf('/'));
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

void UnzipIt(cString s, cString wildcard) {
	cout<<endl<<"# "<<s<<" matches criteria for unzipping"<<endl;
	
	cString exe="???",suffix="";
	cTimeAndDate today;
	
	
	
	
	cString newdir=s.ChopRt('.')+"_"+cString(today.SecondsSince1970());
	cout<<endl<<"mkdir \'"<<newdir<<"\'"<<endl;
	
	if(wildcard.ToLower().Contains("zip")) exe="unzip";
	else if(wildcard.ToLower().Contains("tgz") || wildcard.ToLower().Contains("tar.gz")) {
		exe="gunzip < ";
		suffix="| tar -xvpf -";
	}
	else if(wildcard.ToLower().Contains("tbz") || wildcard.ToLower().Contains("tar.bz2")) {
		exe="bunzip2 < ";
		suffix="| tar -xvpf -";
	} else if(wildcard.ToLower().Contains("rpm")) {
		exe="rpm2cpio ";
		suffix="| cpio -i -d ";
	}
	
	cout<<endl;
	if(exe=="???") {
		cout<<"#";
	} 
	cout<<"(cd \'"<<newdir<<"\' && "<<exe<<" \'../"<<s.ChopAllLf('/')<<"\' "<<suffix<<") && rm -rvf \'"<<s<<"\' >script_output.txt 2>&1 "<<endl;  
}

void lookfordups(cString wildcard) {
	cerr<<"wildcard: "<<wildcard<<endl;
	cDualList thelist;
	FileandHashList(thelist,"./",wildcard);
	int hits=0;
	//int donothing=3;
	
	cout<<"echo \'0\' > progress.txt"<<endl;
//	cout<<"tail -f progress.txt | /usr/bin/zenity --progress --auto-close &"<<endl;
//	cout<<"cat progress
	
	
	for(int i=0; i<thelist.Length(); i++) {
		if(thelist[i]!="") {
			UnzipIt(thelist.GetName(i),wildcard);
			cout<<"echo \'"<<int((float(i)/float(thelist.Length()))*100.0)<<"\' >> progress.txt"<<endl;
			cerr<<"\rProgress: "<<(i+1)<<" / "<<thelist.Length()<<" hits="<<hits<<"          " ;	
		}
		cerr<<"\rProgress: "<<(i+1)<<" / "<<thelist.Length()<<" hits="<<hits<<"          " ;	
	} //end of for i
	cout<<"echo 100% >> progress.txt"<<endl;
	cout<<"# hits="<<hits<<endl;
}


int main(int argc, char* argv[]) {

	cerr<<"January 1st, 2014"<<endl;
	cArgs arguments(argc,argv,"-");
	
	
	cString wildcard=arguments.GetArg("w",1);
	cStringList wildcardlist;
	
	cout<<"#!/bin/sh"<<endl;
	cerr<<"process"<<endl;
	
	
	
	if(!wildcard.Contains(';')) {
		 lookfordups(wildcard);
	} else {
		wildcardlist.FromString(wildcard,';');
		for(int i=0; i<wildcardlist.Length(); i++) lookfordups(wildcardlist[i]);
	}	
	
	
	return 0;
}






