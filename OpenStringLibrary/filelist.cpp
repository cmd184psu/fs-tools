/***************************************************************
 * (C) Copyright 1998-2010 Chris Delezenski
 * This software is released under the LGPL v2, see COPYING and LGPL
 ***************************************************************/
#include "internal.h"
#ifndef WIN32

cFile cFileList::GetFile(int i) {
	cFile temp(Array[i],exec,rec,cis);
	return temp;
}		


bool cFileList::Rename_ToLower(bool nospaces) {
	bool a=true;
	cFile temp("/path/noname.ext",exec,rec,cis);
	for(int i=0; i<Length(); i++) {
		temp=Array[i];
		temp.exec=exec;
		temp.rec=rec;
		temp.cis=cis;
		a=temp.Rename_ToLower(nospaces);
		if(!a) return false;
	}
	return a;
}

bool cFileList::Rename_ToUpper(bool nospaces) {
	bool a=true;
	cFile temp("/path/noname.ext",exec,rec,cis);
	for(int i=0; i<Length(); i++) {
		temp=Array[i];
		temp.exec=exec;
		temp.rec=rec;
		temp.cis=cis;
		a=temp.Rename_ToUpper(nospaces);
		if(!a) return false;
	}
	return a;
}

bool cFileList::Rename_ToFile(cFile & myfile, int casestat, bool nospaces) {
	cerr<<"cFileList::Rename_ToFile()"<<endl;
	bool a=true;
	cFile temp("/path/noname.ext",exec,rec,cis);
	
	//cerr<<"About to loop for "<<Length()<<" times"<<endl;
	for(int i=0; i<Length(); i++) {
		cerr<<i<<" of "<<Length()<<endl;
		temp=Array[i];
		temp.exec=exec;
		temp.rec=rec;
		temp.cis=cis;
		
		//cerr<<"About to exec Rename_ToFile at the file level"<<endl;
		myfile.SetEnding(temp.GetEnding());
		myfile.SetPath(temp.GetPath());
		a=temp.Rename_ToFile(myfile,casestat,nospaces);
		
		
		//if(!a) return false;
	}
	return a;
}

bool cFileList::RenameFile(cDualList & fileprop) {
	bool a=true;
	cFile temp("/path/noname.ext",exec,rec,cis);

	for(int i=0; i<Length(); i++) {
		temp=Array[i];
		temp.exec=exec;
		temp.rec=rec;
		temp.cis=cis;
		
		a=a && temp.RenameFile(fileprop);
	}
	return a;
}

bool cFileList::Rename_ToFile_applyChanges(int casestat, bool nospaces) {
	bool a=true;
	cFile *temp, *temp2;
	for(int i=0; i<Length(); i++) {
		temp=new cFile(Array[i],exec,rec,cis);
		temp2=new cFile(Array[i],exec,rec,cis);
		a=temp->Rename_ToFile(*temp2,casestat,nospaces);
		delete temp;
		delete temp2;
		//if(!a) return false;
	}
	return a;
}

bool cFileList::Rename_ToTitle(bool nospaces) {
	bool a=true;
	cFile temp("/path/noname.ext",exec,rec,cis);
	for(int i=0; i<Length(); i++) {
		temp=Array[i];
		temp.exec=exec;
		temp.rec=rec;
		temp.cis=cis;
		a=temp.Rename_ToTitle(nospaces);
		//if(!a) return false;
	}
	return a;
}

bool cFileList::Rename_ChangeEnding(cString newending,int casestat, bool nospaces) {
	bool a=true;
	cFile temp("/path/noname.ext",exec,rec,cis);
	for(int i=0; i<Length(); i++) {
		temp=Array[i];
		temp.exec=exec;
		temp.rec=rec;
		temp.cis=cis;
		a=temp.Rename_ChangeEnding(newending,casestat,nospaces);
		//if(!a) return false;
	}
	return a;
}

bool cFileList::Rename_Timestamp(int casestat, bool nospaces) {
	bool a=true;
	cFile temp("/path/noname.ext",exec,rec,cis);
	for(int i=0; i<Length(); i++) {
		temp=Array[i];
		temp.exec=exec;
		temp.rec=rec;
		temp.cis=cis;
		a=temp.Rename_Timestamp(casestat,nospaces);
		//if(!a) return false;
	}
	return a;
}

bool cFileList::Command(cString cmdline, int casestat) {
	bool a=true;
	cFile temp("/path/noname.ext",exec,rec,cis);
	for(int i=0; i<Length(); i++) {
		temp=Array[i];
		temp.exec=exec;
		temp.rec=rec;
		temp.cis=cis;
		a=temp.Command(cmdline,casestat);
		//if(!a) return false;
	}
	return a;
}

bool cFileList::DateTimeStamp(cString timestamp) {
	bool a=true;
	cFile temp("/path/noname.ext",exec,rec,cis);
	for(int i=0; i<Length(); i++) {
		temp=Array[i];
		temp.exec=exec;
		temp.rec=rec;
		temp.cis=cis;
		a=a && temp.DateTimeStamp(timestamp);
		if(!a) cout<<"failed to set timestamp for file: "<<Array[i]<<endl;
	}
	return a;
}

bool cFileList::Remove() {
	bool a=true;
	cFile temp("/path/noname.ext",exec,rec,cis);
	for(int i=0; i<Length(); i++) {
		temp=Array[i];
		temp.exec=exec;
		temp.rec=rec;
		temp.cis=cis;
		a=temp.Remove();
		//if(!a) return false;
	}
	return a;
}

bool cFileList::symlinkto(cStringList other) {
	bool a=true;
	cFile temp("/path/noname.ext",exec,rec,cis);
	for(int i=0; i<Length(); i++) {
		temp=Array[i];
		temp.exec=exec;
		temp.rec=rec;
		temp.cis=cis;
		a=temp.symlinkto(other[i]);
		//if(!a) return false;
	}
	return a;
}
		
bool cFileList::symlinkfrom(cStringList other) {
	bool a=true;
	cFile temp("/path/noname.ext",exec,rec,cis);
	for(int i=0; i<Length(); i++) {
		temp=Array[i];
		temp.exec=exec;
		temp.rec=rec;
		temp.cis=cis;
		a=temp.symlinkfrom(other[i]);
		//if(!a) return false;
	}
	return a;
}

/*
void FileList(cStringList &mylist, cString fullpath, cString ending, int depth=-1) {
//	cerr<<"filelist"<<endl;
	if(fullpath[0]=='/') chdir("/");
	cString cmd="find "+fullpath+" -iname \""+ending+"\"";
	if(depth!=-1) {
		cmd=cmd+" -maxdepth "+cString(depth);
	}
//	cout<<"about to execute: "<<cmd<<endl;
	pclose(mylist.FromFile(popen(cmd, "r" )));
	mylist.UCompact();
	if(fullpath[0]!='/')
		for(int i=0; i<mylist.Length(); i++) {
			mylist[i]=mylist[i].ChopLf('/');
		}
}
*/

bool cFileList::GetList(cString starting_path, cString pattern) {
	cString cmdline;
	if(cis) cmdline="find "+starting_path+" -iname \""+pattern+"\"";
	else cmdline="find "+starting_path+" -name \""+pattern+"\"";
	
	if(!rec) cmdline+=" -maxdepth 1";
//	cout<<"about to execute:"<<cmdline<<endl;
//	pclose(FromFile(popen(cmdline+" -type f","r")));
	pclose(FromFile(popen(cmdline,"r")));
	cout<<endl;
	
//	cerr<<"found "<<Length()<<" entries"<<endl;
	
	if(Length()==0) return false;
	
//	for(int j=0; j<Length(); j++) cerr<<j+1<<"/"<<Length()<<":"<<Array[j]<<endl;
//	cout<<endl;
	
	Array[Length()-1]="";
	//UCompact();
	cFile *temp;
	if(starting_path[0]!='/') {
		for(int i=0; i<Length(); i++) {
			Array[i]=Array[i].ChopLf('/');
			if(Array[i]!="") {
				temp=new cFile(Array[i],false,false,false);
				if(temp->IsDirectory()) Array[i]="";
				delete temp;
			}
		}
	}
//	cout<<endl;
//	for(int j=0; j<Length(); j++) cerr<<j+1<<"/"<<Length()<<":"<<Array[j]<<endl;
//	cout<<endl;
	Compact();
//	cout<<endl;
//	for(int j=0; j<Length(); j++) cerr<<j+1<<"/"<<Length()<<":"<<Array[j]<<endl;
//	cout<<endl;
	return true;
}

void cFileList::info() {
	cFile temp("/path/noname.ext",exec,rec,cis);
	for(int i=0; i<Length(); i++) {
		temp=Array[i];
		temp.exec=exec;
		temp.rec=rec;
		temp.cis=cis;
		temp.info();
	}
}

cFileList::cFileList(bool _exec, bool _rec, bool _cis) { exec=_exec; rec=_rec, cis=_cis;}
#endif
