 /*
    destroyall.cpp - Seeks out all files that are identical based on an independent md5 hash.
    Prints to stdout suggested deletions.
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
using namespace std;

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <OpenStringLib.h>

#ifndef __USE__CVS__
#define VERSION "(Part of Dev Tools) Norepeats v0.93 - 12-09-03 - written by Chris Delezenski"
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

bool isone(cString s) {
	cString t=s;
	if(t.Contains('.')) t=t.ChopRt('.');
	if(t.Length()<=18) return false;
	if(!t.Contains("___")) return false;
	
	t=t.ChopLf(t.Length()-18); //remove content
	if(t.ChopRt(15)!="___") return false;
	t=t.ChopLf(3);
	//bad interpretation of number
	if(t.ChopRt(t.Length()-10).AtoI()==0) return false;
	t=t.ChopLf(10);
	if(!isalpha(t[2])) return false;
	t[2]='_';
	if(t!="_____") return false;
	return true;
}

cString fixit(cString s) {
//palindromefinder___1348993417__Z__.txt
	//3 10 5
	
	if(s.Contains('.')) return s.ChopRt('.').ChopRt(18)+"."+s.ChopAllLf('.');
	return s.ChopRt(18);
}

void lookfordups(cString prefix, cString wildcard) {
	cDualList thelist;
	FileandHashList(thelist,"./",wildcard);
	int hits=0;
	cString dir,curfile, newfile;
	//int donothing=3;
	
//	struct stat statbuf;/
//	cTimeAndDate tad;
	
	for(int i=0; i<thelist.Length(); i++) {
		if(thelist[i]!="") {
						
				cout<<endl<<"# want to rename "<<thelist.GetName(i)<<endl;
				
				
				
				dir=thelist.GetName(i).ChopRt('/');
				curfile=thelist.GetName(i).ChopAllLf('/');
				newfile=prefix+"-"+curfile;

//				stat(thelist.GetName(i),&statbuf);	/
//				tad=(&statbuf.st_mtime);	
				
cout<<"mv -vi \'"<<curfile<<"\' \'"<<newfile<<"\'"<<endl;
				cerr<<"\rProgress: "<<(i+1)<<" / "<<thelist.Length()<<" hits="<<hits<<"          " ;	
			//} //end of for j

		}
		cerr<<"\rProgress: "<<(i+1)<<" / "<<thelist.Length()<<" hits="<<hits<<"          " ;	
	} //end of for i
	cout<<"# hits="<<hits<<endl;
}

void testit(cString s) {
	cout<<"------TEST IT------"<<endl;
	struct stat statbuf;
	cTimeAndDate tad;

	stat(s,&statbuf);	
	tad=(&statbuf.st_mtime);	
	struct tm mytm;
	localtime_r(&statbuf.st_mtime,&mytm);
	
	cout<<"mytm.year="<<mytm.tm_year+1900<<endl;			
	cout<<"cTimeAndDate::GetYear()=="<<tad.GetYear()+1900<<endl;


}
int main(int argc, char* argv[]) {

	cerr<<"Sept 29th, 2012"<<endl;
	cArgs arguments(argc,argv,"-");
	
	
	cString wildcard=arguments.GetArg("w",1);
	cString prefix=arguments.GetArg("p",1);
	
	
	cout<<"#!/bin/sh"<<endl;
	cerr<<"process"<<endl;
	
	
	
	if(wildcard=="") wildcard="*";
	cerr<<"wildcard="<<wildcard<<endl;
	lookfordups(prefix,wildcard);
	
	
	
	return 0;
}






