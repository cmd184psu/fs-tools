/***************************************************************
 * (C) Copyright 1998-2011 Chris Delezenski
 * This software is released under the LGPL v2, see COPYING and LGPL
 ***************************************************************/
#ifndef ____FILE_LIST____H
#define ____FILE_LIST____H
class cFile;
class cDualList;
class cFileList: public cStringList {
	public:
		bool exec,rec,cis;
		cFile GetFile(int i);
		bool Rename_ToLower(bool nospaces=false);
		bool Rename_ToUpper(bool nospaces=false);
		bool Rename_ToTitle(bool nospaces=false);
		bool Rename_ToFile(cString &_fname,int casestat=CASE_NOCHANGE, bool nospaces=false) {
			cFile myfile(_fname,exec,rec,cis);
			return Rename_ToFile(myfile,casestat, nospaces);
			
		}
		bool Rename_ToFile(cFile & _fname, int casestat=CASE_NOCHANGE, bool nospaces=false);
		bool Rename_ToFile_applyChanges(int casestat=CASE_NOCHANGE, bool nospaces=false);
		bool RenameFile(cDualList & fileprops);
		bool Rename_ChangeEnding(cString newending, int casestat=CASE_NOCHANGE, bool nospaces=false);
		bool Rename_Timestamp(int casestat=CASE_NOCHANGE, bool nospaces=false);
		bool Command(cString cmdline,int casestat=CASE_NOCHANGE);
		bool DateTimeStamp(cString timestamp);
		
		bool Remove();
		bool symlinkto(cStringList other);
		bool symlinkfrom(cStringList other);
		bool GetList(cString starting_path, cString pattern="*");	
		cFileList(bool _exec, bool _rec, bool _cis);
		cFileList() { exec=false; rec=false; cis=false; }
		void info();
};
#endif




