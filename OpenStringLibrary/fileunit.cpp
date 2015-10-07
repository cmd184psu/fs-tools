/***************************************************************
 * (C) Copyright 1998-2010 Chris Delezenski
 * This software is released under the LGPL v2, see COPYING and LGPL
 ***************************************************************/
#include "internal.h"
#ifndef WIN32
cFile& cFile::operator=(cFile Rhs) {

	fname=Rhs.fname;
	path=Rhs.path;
	ending=Rhs.ending;
	return *this;
}

cFile::cFile(cString filename, bool _exec, bool _rec, bool _cis) {
	exec=_exec;
	rec=_rec;
	cis=_cis;
	if(filename.Contains('/')) path=filename.ChopRt('/');
	else path="";
	fname=filename;
	while(fname.Contains('/')) fname=fname.ChopLf('/');
	ending=fname;
	while(fname.Contains('.')) fname=fname.ChopRt('.');
	if(ending.Contains('.')) ending=ending.ChopLf('.');
	else ending="";
}

bool cFile::NoSpaces() {
	fname.Replace(' ', '_');	
	return true;
}

bool cFile::Rename_ToLower(bool nospaces) {
	cFile newfilename=*this;
	newfilename.ending=newfilename.ending.ToLower();
	newfilename.fname=newfilename.fname.ToLower();	
	return Rename_ToFile(newfilename, CASE_TOLOWER,nospaces);
}

bool cFile::Rename_ToUpper(bool nospaces) {
	cFile newfilename=*this;
	newfilename.ending=newfilename.ending.ToUpper();
	newfilename.fname=newfilename.fname.ToUpper();	
	return Rename_ToFile(newfilename, CASE_TOUPPER,nospaces);
}

bool cFile::Rename_ToTitle(bool nospaces) {
	cFile newfilename=*this;
	newfilename.ending=newfilename.ending.TitleCase();
	newfilename.fname=newfilename.fname.TitleCase();	
	return Rename_ToFile(newfilename, CASE_TOTITLE,nospaces);
}

bool cFile::Rename_ChangeEnding(cString newending, int casestat,bool nospaces) {
	cFile newfilename=*this;
	newfilename.ending=newending;	
	return Rename_ToFile(newfilename, casestat,nospaces);
}

bool cFile::Rename_Timestamp(int casestat,bool nospaces) {
	struct stat buf;
	stat(GetFullPath(),&buf);	
	cFile newfilename=*this;
	cString s=ShowTime(buf.st_mtime);
	if(newfilename.GetFullPath().Contains(s)) return false;
	
	newfilename.fname=newfilename.fname+"-"+s;
	return Rename_ToFile(newfilename,casestat,nospaces);
}

bool cFile::Command(cString cmd, cString post) {
	cString cmdline=cmd+" \""+GetFullPath()+"\"";
	if(!Exists()) return false;
	if(post!="") cmdline+=" "+post;
	if(exec) return (system(cmdline)==0);
	else cout<<cmdline<<endl;
	return true;
}

cString cFile::ShowTime(time_t &t) {
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

bool cFile::Rename_ToFile(cFile &newfilename, int casestat,bool nospaces) {
	//cerr<<"Rename_ToFile: from:"<<GetFullPath()<<" to "<<newfilename.GetFullPath()<<endl;
	cString cmdline;
	if(Exists()) {
	
		if(nospaces) { 
			newfilename.NoSpaces();
//			cerr<<"removed spaces?: \""<<newfilename.GetFullPath()<<"\""<<endl;
		}
	
		switch(casestat) {
			case CASE_TOUPPER:
				newfilename.ToUpper();
			break;
			case CASE_TOLOWER:
				newfilename.ToLower();
			
			break;
			case CASE_TOTITLE:
				newfilename.ToTitle();
			break;					
		
			default:
			break;
		
		}
		
		if(GetFullPath()==newfilename.GetFullPath()) {
			cerr<<"#nothing to do, these filenames are the same!"<<endl;
			return false;
		}
		while(newfilename.Exists()) {
			cerr<<"# but "<<newfilename.GetFullPath()<<" already exists, so try adding -b to it"<<endl;
			newfilename.fname+="-b";
		}
		
		
		cmdline="mv \""+GetFullPath()+"\" \""+newfilename.GetFullPath()+"\"";
		
		if(exec) system(cmdline);
		//else
		cout<<"# execute cmdline: "<<cmdline<<endl; 
		cout<<cmdline<<endl;
		return true;
	}
	else
		cerr<<"#file \""<<GetFullPath()<<" doesn't exist - Can't rename to "<<newfilename.GetFullPath()<<endl;
	return false;	
}

bool cFile::RenameFile(cDualList & fileprops) {
	
	//determine that we have a filename to rename to
	if(fileprops["tofile"]=="") {
		fileprops["tofile"]=GetFullPath();
	}
	
	cFile newfilename(fileprops["tofile"],exec,rec,cis);
	
	//add date stemp to the end if we want it
	if(fileprops["appenddate"]=="TRUE") {
		
		struct stat buf;
		stat(GetFullPath(),&buf);	
		//cFile newfilename=*this;
		cString s=ShowTime(buf.st_mtime);
		if(!newfilename.GetFullPath().Contains(s)) newfilename.fname=newfilename.fname+"-"+s;
	
	}
	//set directory structure the same if recursive
	
	if(rec) newfilename.SetPath(GetPath());
	
	
	if(fileprops["toext"]!="") {
	
	
		newfilename.SetEnding(fileprops["toext"]);
	}
	
	cString cmdline;
	if(Exists()) {
	
//		if(nospaces) { 
//			newfilename.NoSpaces();
//			cerr<<"removed spaces?: \""<<newfilename.GetFullPath()<<"\""<<endl;
//		}
		//change case of 4the new filename
		switch(String2case(fileprops["case"])) {
			case CASE_TOUPPER:
				newfilename.ToUpper();
			break;
			case CASE_TOLOWER:
				newfilename.ToLower();
			break;
			case CASE_TOTITLE:
				newfilename.ToTitle();
			break;					
			default:
			break;
		}
		if(GetFullPath()==newfilename.GetFullPath()) {
			cerr<<"#nothing to do, these filenames are the same!"<<endl;
			return false;
		}
		
		//add a "-b" to the end, to avoid overwriting an existing file of the same name.
		while(newfilename.Exists()) {
			cerr<<"# but "<<newfilename.GetFullPath()<<" already exists, so try adding -b to it"<<endl;
			newfilename.fname+="-b";
		}
		
		
		cmdline="mv \""+GetFullPath()+"\" \""+newfilename.GetFullPath()+"\"";
		
		if(exec) system(cmdline);
		//else
		cout<<"# execute cmdline: "<<cmdline<<endl; 
		cout<<cmdline<<endl;
		return true;
	}
	else
		cerr<<"#file \""<<GetFullPath()<<" doesn't exist - Can't rename to "<<newfilename.GetFullPath()<<endl;
	return false;	
}


bool cFile::DateTimeStamp(int mon, int day, int year, int hour, int min) {
	struct utimbuf* buf;
	struct tm *mytime;
	mytime=new struct tm;
	
	mytime->tm_mon=mon-1;
	mytime->tm_mday=day;

	if(year<80) mytime->tm_year=year+100;
	else mytime->tm_year=year;
	mytime->tm_hour=hour;
	mytime->tm_min=min;
	mytime->tm_sec=0;
/*
	cout<<"set tm_mon="<<mytime->tm_mon<<endl;
	cout<<"set tm_mday="<<mytime->tm_mday<<endl;
	cout<<"set tm_year="<<mytime->tm_year<<endl;
	cout<<"set tm_hour="<<mytime->tm_hour<<endl;
	cout<<"set tm_min="<<mytime->tm_min<<endl;
	cout<<"set tm_sec="<<mytime->tm_sec<<endl;
*/	
	time_t ct=0;
	ct=mktime(mytime);

	if(ct>0) cout<<"ct="<<ct<<endl;
	else cout<<"time conversion failed!"<<endl;
	
	buf=new struct utimbuf;
	
	
	
	if(ct>0  && exec){
		buf->actime=ct;
		buf->modtime=ct;
		cerr<<"called utime for "<<GetFullPath()<<endl;
	       utime(GetFullPath(), buf);
	}
//	else {
		cout<<"timestamp "<<GetFullPath()<<" "<<mon<<"/"<<day<<"/"<<year<<" at "<<hour<<":"<<min<<endl;
//	}
	return ct>0;
}


bool cFile::DateTimeStamp(cString time) {
	cStringList listb, filenamebreak, timelist;
	timelist.FromString(time,':');
	if(timelist.Length()<4) { timelist+="0"; timelist+="0"; }
	if(timelist.Length()<5) timelist+="0";

	return DateTimeStamp(timelist[0].AtoI(),timelist[1].AtoI(),timelist[2].AtoI(),timelist[3].AtoI(),timelist[4].AtoI());
}

bool cFile::Exists() {
   ifstream infile(GetFullPath());
	if(infile) { infile.close(); return true; }
   infile.close();
   return false;
}


int String2case(cString s) {
	if(s=="") { return CASE_NOCHANGE; }
	if(s.ToLower()=="lower") { return CASE_TOLOWER; }
	
	if(s.ToLower()=="upper") { return CASE_TOUPPER; }
	
	if(s.ToLower()=="title") { return CASE_TOTITLE; }
	
	cerr<<"Invalid case!"<<endl;
	
	return -1;
}

#endif



