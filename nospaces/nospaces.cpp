 /*
    nospaces.cpp - Seeks out all files or directories with spaces and replaces spaces with dashes.
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

#define VERSION "(Part of FS Tools) Nospaces v0.01 - 9-4-20 - written by Chris Delezenski"

void usage() {
	cout<<"USAGE: nospaces [-d] -w [*.ext] generates a batch file to stdout, errors to stderr"<<endl;
}

cString BashFriendly(cString input) {
	cString output=input;
	cString charlist="\\ '\"[]()!#$@&";
	for(int i=0; i<charlist.Length(); i++) {
		output=output.Replace(charlist[i],"\\"+cString(charlist[i]));
	}
	return output;
}

cString CliFriendly(cString input) {
	cString output=input;
	cString charlist="\\ '\"[]()!#$@&";
	for(int i=0; i<charlist.Length(); i++) {
		output=output.Replace(charlist[i],"-");
	}
	return output;
}

void generateProcessor(bool dir, bool rec, bool force) {
	cStringList thelist;
	cString type="";
	cString depth="";
	if(dir) type="d"; else type="f";
	if(!rec) depth="-maxdepth 1";
	cString cmd="find . -type "+type+" "+depth+" -iname \"* *\"";

	FILE *t=popen(cmd, "r" );
	thelist.FromFile(t);
	pclose(t);
	thelist.UCompact();

	for(int i=0; i<thelist.Length(); i++) {
		if(thelist[i]=="") continue;
		cout<<"# "<<thelist[i]<<endl;
		cout<<"sudo xattr -c "<<BashFriendly(thelist[i])<<endl;
		cout<<"sudo chmod -N "<<BashFriendly(thelist[i])<<endl;
		if(force) cout<<"sudo ";
		cout<<"mv -iv "<<BashFriendly(thelist[i])<<" "<<CliFriendly(thelist[i])<<endl; 		
		cerr<<"\rProgress: "<<(i+1)<<" / "<<thelist.Length()<<endl;	
	} //end of for i
	cout<<"# hits="<<thelist.Length()<<endl;
}

int main(int argc, char* argv[]) {
	cerr<<"Sept 5th, 2020"<<endl;
	cArgs arguments(argc,argv,"-");
	if(arguments.IsSet("v")) return 0;
	cout<<"#!/bin/sh"<<endl;
	cerr<<"process"<<endl;
	if(arguments.IsSet("d")) cerr<<"directories: yes"<<endl;
	else cerr<<"directories: no"<<endl;
	if(!arguments.IsSet("nr")) cerr<<"recursion: yes"<<endl;
	else cerr<<"recursion: no"<<endl;

	if(arguments.IsSet("u") generateProcessorUNDO();
	else generateProcessor(arguments.IsSet("d"),!arguments.IsSet("nr"),arguments.IsSet("f"));
	return 0;
}
