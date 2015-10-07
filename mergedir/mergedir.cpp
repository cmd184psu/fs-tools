 /*
    norepeats.cpp - Seeks out all files that are identical based on an independent md5 hash.
    Prints to stdout suggested deletions.
    Copyright (C) 2003  Chris Delezenski

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
	cString cmd="find "+fullpath+" -type f -iname \""+ending+"\"";
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
		temphash=gethash(temp[i]);
		mylist[temp[i]]=temphash;
	}
}

void FileList_1(cStringList &mylist, cString fullpath, cString ending) {
	if(fullpath[0]=='/') chdir("/");
	cString cmd="find "+fullpath+" -follow -type f -maxdepth 1 -iname \""+ending+"\"";
	pclose(mylist.FromFile(popen(cmd, "r" )));
	mylist.UCompact();
}
void FileAndDirList_1(cStringList &mylist, cString fullpath) {
	if(fullpath[0]=='/') chdir("/");
	cString cmd="find \""+fullpath+"\" -follow -maxdepth 1 -iname \"*\"";
	pclose(mylist.FromFile(popen(cmd, "r" )));
	mylist.UCompact();
	for(int i=0; i<mylist.Length(); i++) {
	
		if(!mylist[i].Contains('/')) {
			cerr<<"removing "<<mylist[i]<<endl;
			 mylist[i]="";
		}
	}
	mylist.Compact();
	cerr<<"===> length="<<mylist.Length()<<endl;
	
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

void lookfordups(cString wildcard) {
	cDualList thelist;
	FileandHashList(thelist,"./",wildcard);
	int hits=0;
	int donothing=3;
	for(int i=0; i<thelist.Length(); i++) {
		if(thelist[i]!="") {
			for(int j=i+1; j<thelist.Length(); j++) {
				if(thelist[j]==""){ 
					donothing=3;
				}
				else if(j==i) {
					donothing=3;
				} else {
					if(thelist.GetValFromIndex(i)==thelist.GetValFromIndex(j) && i!=j) {
						cout<<endl<<"# "<<thelist.GetName(i)<<" and "<<thelist.GetName(j)<<" contain the same data"<<endl;
						
						if(thelist.GetName(i).Length()>thelist.GetName(j)) {
							cout<<"rm -rvf \""<<thelist.GetName(i)<<"\""<<endl; 		
							thelist[i]="";
						} else {
							cout<<"rm -rvf \""<<thelist.GetName(j)<<"\""<<endl;
							thelist[j]="";
						}
						hits++;
					}
	
				} //end if	
				cerr<<"\rProgress: "<<(i+1)<<" / "<<thelist.Length()<<" hits="<<hits<<"          " ;	
			} //end of for j

		}
		cerr<<"\rProgress: "<<(i+1)<<" / "<<thelist.Length()<<" hits="<<hits<<"          " ;	
	} //end of for i
	cout<<"# hits="<<hits<<endl;
}

void safe_merge(cString source, cString source2, cString target) {
	cStringList dirlist_src, dirlist_tgt;
	cString dirlist_src_changeto;
	
	FileAndDirList_1(dirlist_src, source2);
	FileAndDirList_1(dirlist_tgt, source);
	
	//deep copy
	for(int i=0; i<dirlist_src.Length(); i++) {
		dirlist_src_changeto=dirlist_src[i];
		if(dirlist_tgt.Contains(dirlist_src[i].ChopAllLf('/').ChopAllRt('.'))) {
			cerr<<"dirlist_tgt contains: "<<dirlist_src[i].ChopAllLf('/').ChopAllRt('.')<<endl;
			
			if(dirlist_src[i].Contains('.')) dirlist_src_changeto=target+"/"+dirlist_src[i].ChopAllLf('/').ChopAllRt('.')+"-MERGED."+dirlist_src[i].ChopAllLf('/').ChopLf('.');
			else  dirlist_src_changeto=target+"/"+dirlist_src[i].ChopAllLf('/')+"-MERGED";
			
			
			
		} else {
			dirlist_src_changeto=target;
		}
		cout<<"mv -vf \""+dirlist_src[i]+"\" \""+dirlist_src_changeto<<"\""<<endl;	
	}
}

int main(int argc, char* argv[]) {

	cerr<<"December 7th, 2011"<<endl;
	cArgs arguments(argc,argv,"-");
	
	
	cString dir1=arguments.GetArg("dir1",1);
	cString dir2=arguments.GetArg("dir2",1);
	
	cout<<"#!/bin/sh"<<endl;
	cerr<<"process"<<endl;
	
	if(dir1=="" || dir2=="") {
		cerr<<"failure, dir1 or dir2 is blank!"<<endl;
		return 1;
	}
	
	cString dir3=dir1.Replace(' ','_')+"_and_"+dir2.Replace(' ','_')+"_merged";
	
	cout<<"mv -vf \""<<dir1<<"\" \""<<dir3<<"\""<<endl;
	
	safe_merge(dir1, dir2, dir3);
	
	cout<<"rmdir -p \""<<dir2<<"\""<<endl;
	cout<<"echo \"Merge Complete\""<<endl;
	
	return 0;
}






