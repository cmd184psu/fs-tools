/***************************************************************
 * (C) Copyright 1998-2010 Chris Delezenski
 * This software is released under the LGPL v2, see COPYING and LGPL
 ***************************************************************/
#include "internal.h"
#include <stdlib.h>
cString cPHP::file_get_contents(cString fname) {
	cStringList list;
	list.FromFile(fname);
	return list.ToString("\n");
}

bool cPHP::System(cString fname,int &retval) {
	#ifdef WIN32
		retval=system((char*)fname);
	#else
	pid_t pid=fork();
	cExec myexec;
	myexec.FromCommandline(fname);
	bool background=false;
	
	
	myexec.SetVerbose(false);
	
	
	if(fname[-1]=='&'){
		background=true;
		fname=fname.ChopRt(1);
	}
	
	switch(pid) {
		case -1:
			if(background) signal(SIGCHLD,SIG_IGN);
		
			retval=-1;
			return false;
		break;
		case 0:
			if(background) signal(SIGCHLD,SIG_IGN);
			
			
	
			setenv("PATH","/bin:/usr/bin",1);
			if(!myexec.Exec()) {
				cerr<<"exec failed?"<<endl;
			}
			cerr<<"child exits 1"<<endl;
			exit(1);
		break;
	}
	if(background) {
		signal(SIGCHLD,SIG_IGN);
		retval=0;
	} else {
		waitpid(pid,&retval,0);
		
	}
	#endif
	return retval==0;
}

bool cPHP::file_put_contents(cString fname, cString contents) {
	cStringList thefile;
	thefile.FromString(contents,"\n");
	return thefile.ToFile(fname)>0;
}
