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
#include <iostream>
#include <string>

#define VERSION "(Part of Dev Tools) stringsfltr v0.01 - 5-21-20 - written by Chris Delezenski"


int main() {
	bool good_to_go=true;

	std::string avoid="`!@#$%^&*(),./<>?'[]{}-=_+:;'\"\\|";
	cerr<<"avoiding these characters: "<<avoid<<endl;
	for (std::string line; std::getline(std::cin, line);) {
		good_to_go=true;
		
		if(line.length()<8) good_to_go=false;
    
		if(line.find("rim") != std::string::npos) std::cout <<"VIRUS?!?:"<< line << std::endl;


		for(int j=0; good_to_go && j<line.length(); j++ ) {
	 		if(avoid.find_first_of(line[j])!=-1) {
				//cerr<<"SKIP LINE!!! b/c of char: "<<line[j]<<endl;	    		
				good_to_go=false; 
	    			break;
	    		}
	    	}
	    
    		if(good_to_go) std::cout << line << std::endl;
    	}
    	return 0;
}


