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

void FileList_R(cStringList &mylist, cString fullpath, cString ending,bool usedirs) {
	if(fullpath[0]=='/') chdir("/");
	
	
	cString cmd;
	
	if(!usedirs) cmd="find \""+fullpath+"\" -type f -iname \""+ending+"\"";
	else cmd="find \""+fullpath+"\" -type d -iname \""+ending+"\"";
	FILE *t=popen(cmd, "r" );
	mylist.FromFile(t);
	pclose(t);
	mylist.UCompact();
}

void FileandHashList(cDualList &mylist, cString fullpath, cString ending, bool usedirs) {
	cStringList temp;
	cString temphash;
	FileList_R(temp,fullpath,ending,usedirs);
	for(int i=0; i<temp.Length(); i++) {
		cerr<<"\rprocessing "<<i<<" of "<<temp.Length();
		//temphash=gethash(temp[i]);
		mylist[temp[i]]="hash";
	}
}

void FileList_1(cStringList &mylist, cString fullpath, cString ending, bool usedirs) {
	if(fullpath[0]=='/') chdir("/");
	cString cmd;
	
	if(usedirs) cmd="find \""+fullpath+"\" -follow -type d -maxdepth 1 -iname \""+ending+"\"";
	else cmd="find \""+fullpath+"\" -follow -type f -maxdepth 1 -iname \""+ending+"\"";
	pclose(mylist.FromFile(popen(cmd, "r" )));
	mylist.UCompact();
}

void FileList_1(cStringList &mylist, cString fullpath, bool usedirs) {
	cStringList split;
	if(fullpath.Contains('*')) 
		split.FromString(fullpath,"*.");
	else
		split.FromString(fullpath,".");
		
	FileList_1(mylist,fullpath.ChopRt('/'),fullpath.ChopAllLf('/'),usedirs);
}

void FileList_R(cStringList &mylist, cString fullpath, bool usedirs) {
	cStringList split;
//	cerr<<"fullpath="<<fullpath<<endl;
	if(fullpath.Contains('*')) 
		split.FromString(fullpath,"*.");
	else
		split.FromString(fullpath,".");
	FileList_R(mylist,fullpath.ChopRt('/'),fullpath.ChopAllLf('/'),usedirs);
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
//	cout<<"tail -f "<<progressFile<<" | /usr/bin/zenity --progress --auto-close &"<<endl;
}

void progressDuring(cString progressFile, int cur, int n) {
	cout<<"# "<<cur<<" of "<<n<<endl;
	cout<<"echo \""<<int(float(cur)/float(n)*100.0)<<"\" >> "<<progressFile<<endl;
}

void progressEnding(cString progressFile) {
	cout<<"echo 100 >>"<<progressFile<<endl;
	cout<<"rm -rvf "<<progressFile<<endl;
}



void lookfordups(cString wildcard, cString newending) {

	cerr<<"wildcard="<<wildcard<<endl;
	cerr<<"newending="<<newending<<endl;
	cDualList thelist;
	FileandHashList(thelist,"./",wildcard,false);
	int hits=0;
	cString progressFile="da-progress.txt";
	progressBeginning(progressFile);
	//int donothing=3;
	for(int i=0; i<thelist.Length(); i++) {
		if(thelist[i]!="") {
				cout<<endl<<"# "<<thelist.GetName(i)<<" matches criteria for deletion"<<endl;
				cout<<"mv -iv \'"<<thelist.GetName(i)<<"\' \'"<<thelist.GetName(i).ChopRt('.')+"."+newending.ChopLf('.')<<"\'"<<endl;
				cerr<<"\rProgress: "<<(i+1)<<" / "<<thelist.Length()<<" hits="<<hits<<"          " ;	
				progressDuring(progressFile,i+1,thelist.Length());
			//} //end of for j

		}
		cerr<<"\rProgress: "<<(i+1)<<" / "<<thelist.Length()<<" hits="<<hits<<"          " ;	
	} //end of for i
	cout<<"# hits="<<hits<<endl;
	progressEnding(progressFile);

}


int main(int argc, char* argv[]) {

	cerr<<"January 5th, 2013"<<endl;
	cArgs arguments(argc,argv,"-");
	
	
	cString wildcard=arguments.GetArg("w",1);
	cString newending=arguments.GetArg("n",1);
	cStringList wildcardlist;
	cout<<"#!/bin/sh"<<endl;
	cerr<<"process"<<endl;
	


	if(wildcard=="") wildcard="*";
	
	
	if(!wildcard.Contains(';')) {
		 lookfordups(wildcard,newending);
	} else {
		wildcardlist.FromString(wildcard,';');
		for(int i=0; i<wildcardlist.Length(); i++) lookfordups(wildcardlist[i],newending);
	}
	
	
	
	return 0;
}






