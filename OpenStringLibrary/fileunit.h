/***************************************************************
 * (C) Copyright 1998-2010 Chris Delezenski
 * This software is released under the LGPL v2, see COPYING and LGPL
 ***************************************************************/
#ifndef __FILE__H___
#define __FILE__H___

enum { CASE_NOCHANGE, CASE_TOLOWER, CASE_TOUPPER, CASE_TOTITLE };
class cDualList;
class cFile {

	
	public:
		bool exec;
		bool rec;
		bool cis; //true for case insensitive


//		cFile(cString filename,bool _exec=true, bool _rec=true, bool _cis=true);
		cFile(cString filename,bool _exec, bool _rec, bool _cis);

		void SetFilename(cString _fname) { fname=_fname; }
		void SetPath(cString _path) { path=_path; }
		void SetEnding(cString _ending) { ending=_ending; }
		void SetRec(bool _rec) { rec=_rec; }
		void SetExec(bool _exec) { exec=_exec; }
		void SetCaseSensitive(bool _cis) { cis=_cis; }	
		
		void ToTitle() {
			ending=ending.TitleCase();
			fname=fname.TitleCase();	
		}
		
		void ToLower() {
			ending=ending.ToLower();
			fname=fname.ToLower();	
			
		}
		
		void ToUpper() {
			ending=ending.ToUpper();
			fname=fname.ToUpper();	
		}	
		
		cString GetFilename() { return fname; }
		cString GetPath() { return path; }
		cString GetEnding() { return ending; }
		
		cString GetFullPath() { 
		
			if(path=="/") path="";
			else if(path=="") path=".";
			if(ending=="") {
				return path+"/"+fname;
			} else {			
			return path+"/"+fname+"."+ending;
			}
		}
		
		
		bool NoSpaces();
		bool Rename_ToFile(cFile &newfilename, int casestat, bool nospaces);
//		bool Rename_ToFile(cFile &newfilename, int casestat=CASE_NOCHANGE, bool nospaces=false);
		bool Rename_ToFile(cString _fname, int casestat, bool nospaces) { cFile temp(_fname,exec,rec,cis); return Rename_ToFile(temp,
		casestat,nospaces); }
//		bool Rename_ToFile(cString _fname, int casestat=CASE_NOCHANGE, bool nospaces=false) { cFile temp(_fname,exec,rec,cis); return Rename_ToFile(temp, casestat,nospaces); }
		
		cFile& operator=(cFile Rhs);
		cFile& operator=(cString Rhs) { cFile temp(Rhs,exec,rec,cis); (*this)=temp; return *this; }
		bool Rename_ToLower(bool nospaces);
		bool Rename_ToUpper(bool nospaces);
		bool Rename_ToTitle(bool nospaces);
		bool Rename_ChangeEnding(cString newending, int casestat=CASE_NOCHANGE, bool nospaces=false);
		bool Rename_Timestamp(int casestat=CASE_NOCHANGE, bool nospaces=false);
		bool RenameFile(cDualList & fileprops);
		cString ShowTime(time_t &t);
		
		bool Exists();

		bool Remove() {
			if(Exists()) {
				if(exec) {
					return unlink(GetFullPath())==0;
				} else { 
					cout<<"unlink(\""<<GetFullPath()<<"\")"<<endl; 
				}
				return true;
			}
			cerr<<"File \""<<GetFullPath()<<"\" Doesn't exist and therefore cannot be deleted"<<endl;
			return false;
		}
		
		bool Command(cString cmd, cString post="");
		bool DateTimeStamp(int mon, int day, int year, int hour=0, int min=0);
		bool DateTimeStamp(cString time);
		bool symlinkto(cFile other) {
			return symlink(other.GetFullPath(),GetFullPath())==0;
		}
		
		bool symlinkfrom(cFile other) {
			return symlink(GetFullPath(),other.GetFullPath())==0;
		}
		

		bool symlinkto(cString other) {
			return symlink(other,GetFullPath())==0;
		}
		
		bool symlinkfrom(cString other) {
			return symlink(GetFullPath(),other)==0;
		}

		bool IsDirectory() {
			struct stat mystat;
			stat(GetFullPath(),&mystat);
			if(mystat.st_mode & S_IFDIR) return true;
			return false;
		}

		void info() {
			cerr<<"path="<<path<<endl;
			cerr<<"fname="<<fname<<endl;
			cerr<<"ending="<<ending<<endl;
			cerr<<"fullpath="<<GetFullPath()<<endl;
			if(rec) cerr<<"rec: true"<<endl;
			else cerr<<"rec: false"<<endl;

			if(cis) cerr<<"cis: true"<<endl;
			else cerr<<"cis: false"<<endl;

			if(exec) cerr<<"exec: true"<<endl;
			else cerr<<"exec: false"<<endl;
		}

	private:
		cString fname;
		cString path;
		cString ending;

};

int String2case(cString s);
#endif



 




