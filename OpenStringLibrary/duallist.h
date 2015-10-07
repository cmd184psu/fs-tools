/***************************************************************
 * (C) Copyright 1998-2011 Chris Delezenski
 * This software is released under the LGPL v2, see COPYING and LGPL
 ***************************************************************/
#ifndef __DUALLIST___H
#define __DUALLIST___H

#ifdef OSX
#include <signal.h>
#endif


using namespace std;
enum {name, val};

class cString;
class cVectorList;
class cDualList:public cVectorList {
		protected:
			cString sep;
			cString dummy;
			bool disallowcompact;
			static bool setonlyifnotset;
		public:
			// ******** Constructors ****** //
			cDualList(int size=0) {
				sep=" = ";
				cVectorList::Resize(2);
				cVectorList::used=2;
				cVectorList::allocated=2;
				if(size!=0) {
					this->Array[name].Resize(size);
					this->Array[val].Resize(size);
				}
				setonlyifnotset=false;
			}
			const cDualList & operator=( const cDualList & Rhs );
			
			// ******* Accessors ***** //
			const cString& GetName(const int i) const { return Array[name][i]; } 
			cString GetValFromIndex(int i) { return Array[val][i]; }
			const int GetIndexFromName(const cString& i) const;
 			int GetIndexFromVal(cString i);
			cString operator[ ]( int Index ) const { return Array[val][Index]; }
			cString & operator[ ]( int Index ) { return Array[val][Index]; }
			cString & operator[ ]( cString Index );
			cString both( int Index, cString sep=" = " ) {  return Array[name][Index]+" = "+Array[val][Index]; }
			cString Efficiency() { return Array[name].Efficiency(); }
			int Length() const { 
				if(cVectorList::Array==NULL) exit(1);
				return Array[name].Length();
			}
			
			// ******Modifiers****** //
			void Sort(int i=0);
			void SetSep(cString s) { sep=s; }
			void Compact();
			void TrimAll();
			
			// ******* File I/O ******* //
			void ToList(cStringList &list,cString sep=" = ");
			
			bool getdisallowcompact() { return disallowcompact; }
			void setdisallowcompact() { disallowcompact=true; }
			void unsetdisallowcompact() { disallowcompact=false; }
			void SetOnlyIfNotSet() { setonlyifnotset=true; }
			void AlwaysSet() { setonlyifnotset=false; }
			bool IsSetOnlyifnotSet() { return setonlyifnotset; }
			void dl_store(cString _name, cString _value);

			void ImportFrom(cString fname,cString wildcard, cString addition="");
			void Mergewith(cDualList &mergeme, cString addition);
			void subset(cString search, cDualList &matches);
	
		protected:
			void RemoveUnset();
			void AddCouple(cString n,cString v) {
				Array[name]+=n;
				Array[val]+=v;
			}
			bool NameContains(cString findme) { return Array[name].Contains(findme); }
			bool ValueContains(cString findme) { return Array[val].Contains(findme); }
			
};

#ifndef WIN32
#define PID_ERROR -1
class cPidFile:public cDualList {

	public:
		static bool Exists(cString fname) {
			struct stat buf;
			return (stat(fname,&buf)==0);
		}
		
		static int FromFile(cString fname) {
			if(!Exists(fname)) return PID_ERROR;
			ifstream infile(fname);
			if(!infile) return PID_ERROR;
			cString pid;
			if(!pid.FromFile(infile)) return PID_ERROR;
			infile.close();
			return pid.AtoI();
		}
		
		static bool AliveandWell(int pid, cString program) {
			if(pid<0) return false;
			ifstream infile;
			infile.open("/proc/"+cString(pid)+"/cmdline");
			cString cmdline;
			cmdline.FromFile(infile);
			infile.close();
			return cmdline.Contains(program);	
		}

		static bool SendSignal(cString fname,int sig) {
			pid_t pid=cPidFile::FromFile(fname);
			if(pid<1) return false;
			if(kill(pid,0)!=0) return remove(fname)==0;
			
			int r=kill(pid,sig);
			if(r!=0) return false;
			perror(NULL);
			return true;
		}

		static bool SignalHUP(cString fname) { return cPidFile::SendSignal(fname,SIGHUP); }
		static bool SignalUSR(cString fname) { return cPidFile::SendSignal(fname,SIGUSR1); }
		static bool SignalUSR2(cString fname) { return cPidFile::SendSignal(fname,SIGUSR2); }
		static bool Kill(cString fname, bool removeit=false) {
			bool r=cPidFile::SendSignal(fname,SIGKILL);
			if(removeit) return remove(fname)==0;
			return r;
		}
		
		static void ToFile(cString fname) {
			ofstream fout;
			fout.open(fname);
			fout<<getpid()<<endl;
			fout.close();
		}
};

#endif 
#endif 
