/***************************************************************
 * (C) Copyright 1998-2011 Chris Delezenski
 * This software is released under the LGPL v2, see COPYING and LGPL
 ***************************************************************/
 
#ifndef __PHP_TRANSLATION__H
#define __PHP_TRANSLATION__H
 
class cPHP {
 	public:
 	static cString file_get_contents(cString fname);
	static bool System(cString fname, int &retval);
	static bool file_put_contents(cString fname, cString contents);
	#ifdef LINUX
	static cString getcwd() { 
		return cString(get_current_dir_name());
	}
	#endif
	#ifdef WIN32
	//implement later
	static cString getcwd() { return "."; }
	#endif
	#ifdef OSX
	//implement later
	static cString getcwd() { return "."; }
	#endif
//	static bool copy(cString source, cString dest) { int status=0; return ForkExecFG("cp -avpf "+source+" "+dest,status,true); }
	static bool file_exists(cString fname) { 
		struct stat buf;
		return (stat(fname,&buf)==0);
	}
};

#ifndef WIN32
class cPHP_Custom {
	public:
	
	static void thrower(cString _msg, bool _syslog=false, bool _stdout=false, bool _exception=false) {
		if(_syslog) syslog(LOG_INFO,"%s",(char*)(cString(getpid())+" : "+_msg));
		if(_stdout) cout<<getpid()<<" : "<<_msg<<endl;
		if(_exception) exit(1);
	}
};
#endif
#endif
