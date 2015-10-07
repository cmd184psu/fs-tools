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

void FileList_R(cStringList &mylist, cString fullpath, cString ending, bool dironly) {
	if(fullpath[0]=='/') chdir("/");
	
	cString modifier="f";
	if(dironly) modifier="d";
	
	
	cString cmd="find \""+fullpath+"\" -type "+modifier+" -iname \""+ending+"\"";
	FILE *t=popen(cmd, "r" );
	mylist.FromFile(t);
	pclose(t);
	mylist.UCompact();
}

void FileandHashList(cDualList &mylist, cString fullpath, cString ending, bool dironly) {
	cStringList temp;
	cString temphash;
	FileList_R(temp,fullpath,ending, dironly);
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

void FileList_R(cStringList &mylist, cString fullpath, bool dironly) {
	cStringList split;
//	cerr<<"fullpath="<<fullpath<<endl;
	if(fullpath.Contains('*')) 
		split.FromString(fullpath,"*.");
	else
		split.FromString(fullpath,".");
	FileList_R(mylist,fullpath.ChopRt('/'),fullpath.ChopAllLf('/'),dironly);
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

void lookfordups(cString wildcard, bool dironly, bool norename) {
	
	
	cString chosendir="";
	
	if(dironly) chosendir=wildcard.ChopRt('*')+"___newdir";
	
	else chosendir=wildcard.ChopLf('.')+"___newdir";
	
	
	cout<<"#"<<wildcard.ChopRt('*')<<"__newdir"<<endl;
	cout<<"#"<<wildcard.ChopLf('.')<<"__newdir"<<endl;
	cout<<"mkdir "<<chosendir<<endl;
	
	cDualList thelist;
	FileandHashList(thelist,"./",wildcard,dironly);
	int hits=0;
	cString dir,curfile, newfile;
	//int donothing=3;
	
	cTimeAndDate today;

	cString target="";

	for(int i=0; i<thelist.Length(); i++) {
		if(thelist[i]!="") {
			cout<<endl<<"# "<<thelist.GetName(i)<<" matches criteria for deletion"<<endl;
			
			today.Refresh();
			
			if(norename) target=chosendir+"/"+thelist.GetName(i).ChopAllLf('/');
			else target=chosendir+"/"+thelist.GetName(i).ChopAllLf('/').ChopRt('.')
			+"___"+cString(today.SecondsSince1970())+"__"+char(i%26+int('A'))+"__"
			+"."+thelist.GetName(i).ChopAllLf('.');
			
			cout<<"mv -i \""<<thelist.GetName(i)<<"\" \""<<target<<"\""<<endl;
			
			
			cerr<<"\rProgress: "<<(i+1)<<" / "<<thelist.Length()<<" hits="<<hits<<"          " ;	
			//} //end of for j

		}
		cerr<<"\rProgress: "<<(i+1)<<" / "<<thelist.Length()<<" hits="<<hits<<"          " ;	
	} //end of for i
	cout<<"# hits="<<hits<<endl;
}


int main(int argc, char* argv[]) {

	cerr<<"January 1st, 2014"<<endl;
	cArgs arguments(argc,argv,"-");
	
	
	cString wildcard=arguments.GetArg("w",1);
	cStringList wildcardlist;

	cout<<"#!/bin/sh"<<endl;
	cerr<<"process"<<endl;
	
	
	
	if(wildcard=="") wildcard="*";
	cerr<<"wildcard="<<wildcard<<endl;
	
	bool dironly=arguments.IsSet("d");
	
	bool norename=arguments.IsSet("nr") || arguments.IsSet("norename");
	if(!wildcard.Contains(';')) {
		 lookfordups(wildcard, dironly,norename);
	} else {
		wildcardlist.FromString(wildcard,';');
		for(int i=0; i<wildcardlist.Length(); i++) lookfordups(wildcardlist[i],dironly,norename);
	}	

	
	
	
	return 0;
}






