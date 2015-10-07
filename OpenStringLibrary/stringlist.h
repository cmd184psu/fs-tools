/***************************************************************
 * (C) Copyright 1998-2010 Chris Delezenski
 * This software is released under the LGPL v2, see COPYING and LGPL
 ***************************************************************/
#ifndef __STRINGLIST_____H
#define __STRINGLIST_____H

#include <sys/types.h>
#include <dirent.h>


class cString;
class cStringList {
	public:
		// ****** Assignment ***** //
		const cStringList & operator=( const cStringList & Rhs );

		// ****** Constructors ****** //
		cStringList( int Size =0 );
		cStringList( const cStringList & Rhs );
	   	~cStringList( ) { 
			if(allocated!=0) { delete [ ] Array;  }
		}
		
		// ********** Concatenation **************** //
		void operator+=( const cString &Rhs ) {
			if(IsOptimum()) Resize(allocated+500);
			(*this)[used]=Rhs;
			used++;	
		}
		void operator+=( const cStringList &Rhs) { for(int i=0; i<Rhs.Length(); i++)   (*this)+=Rhs[i]; }
		void operator-=( const cString &Rhs); 
		void operator-=( const cStringList &Rhs) { for(int i=-1; i>-Rhs.Length(); i--) (*this)-=Rhs[i]; }

		// ****** Comparision ****** //
		
		friend const int operator == ( const cStringList &Lhs, const cStringList &Rhs );
		friend const int operator != ( const cStringList &Lhs, const cStringList &Rhs );
		friend const int operator <  ( const cStringList &Lhs, const cStringList &Rhs ); 
		friend const int operator >  ( const cStringList &Lhs, const cStringList &Rhs );
		friend const int operator <= ( const cStringList &Lhs, const cStringList &Rhs );
		friend const int operator >= ( const cStringList &Lhs, const cStringList &Rhs );
		friend ostream & operator<< ( ostream & Out, const cStringList &Value );

		
		// ****** Accessors **** //		
	    	int Length( ) const { return used; }
	    	int size( ) const { return used; }
		int Allocated() const { return allocated; }
		bool Contains(cString s) { for(int i=0; i<Length(); i++) if(Array[i].Contains(s)) return true; return false; }
		bool IsOptimum() { return (used==allocated); }
		cString Efficiency() { return cString(used)+"/"+cString(allocated); }
		const cString & operator[] ( int Index ) const;
		cString & operator[] ( int Index );
		int operator[ ]( cString I ) const;
			
		// ********** Modifiers **************** //
		void ClearAll() { Resize(0); }			//effectly destroys the entire array.
		void Compact();					//remove all blank lines 
		void UCompact();				//remove all non-unique elements
		void Insert(int index, cString s="");		//insert s into the array.
		void Reverse();					// reverse the order of the array
		void Swap(int i, int j);			//swap 2 elements in the array
		void Sort();					//alphabetically sort the array, using a linear sort
		void TrimAll() { for(int i=0; i< Length(); i++) Array[i]=Array[i].Trim(); } 
		
		// ********** File IO **************** //
		bool FromFile(const cString &fname);
		bool FromFile(int fd);
		bool FromFile(ifstream & ifs, int n);
		bool FromFile(ifstream & ifs) { return FromFile(ifs, MAX_STRING_LEN); }
		
		FILE* FromFile(FILE* fileptr);
		bool FromStdin() { return FromFile(STDIN_FILENO); }
//		int FromMem(memfile &f);
		void FromString(const cString &line,cString delim); 
		void FromString(const cString &line,char delim);
		void FromStringWithSpaces(const cString &line);
		void FromParagraph(const cString &mystring, int width=80);
		void FromMultipleParagraphs(int width=80);
		
		
		bool FromArray(int argc,char *argv[]);
		bool FromArray(char *argv[]);			//NULL terminated array
		void FromPopen(cString cmdline);
		void FromPopen(cString cmdline, cString search); //popen grep
 		int ToFile(const cString &fname,int s=0, int ed=-1, cString ending="\n");
		int ToFile(int fd,int s=0, int ed=-1, cString ending="\n");
		int ToFile(ofstream & ofstream,int s=0, int ed=-1, cString ending="\n");
		int ToFile(FILE* fileptr,int s=0, int ed=-1, cString ending="\n");
		int ToStdout(int s=0, int ed=-1, cString ending="\n") { return ToFile(STDOUT_FILENO,s,ed,ending); }
		int ToStderr(int s=0, int ed=-1, cString ending="\n") { return ToFile(STDERR_FILENO,s,ed,ending); }
//		int ToMem(memfile &f);
		cString ToString(cString &line,cString delim); //serialize down to a string
		cString ToString(cString &line,char delim) { return ToString(line,cString(delim)); }
		cString ToString(cString delim) { cString temp; return ToString(temp,delim); }
		cString ToString(char delim) { cString temp; return ToString(temp,delim); }
		bool ToArray(int &argc,char *argv[]);
		bool ToArray(char *argv[]); //NULL terminated array
		
		char **ToCharList();
		
 		// ********* Friends ************* //
		friend class cDualList;
		friend class cVectorList;
		bool skipblanks; //when set to true, FromFile will ignore blanks in the file.
		
		//-1, means lhs is > rhs
		//0, means lhs == rhs
		//1, means lhs < rhs
		const int IndexCompare(cString lhs, cString rhs) const { 
			if(lhs==rhs) return 0;
			if((*this)[lhs]>(*this)[rhs]) return -1;
			if((*this)[lhs]<(*this)[rhs]) return 1;
			return 0;
		}				

		bool IndexCompareLeft(cString lhs, cString rhs) const { return IndexCompare(lhs,rhs)==-1; }
		
		bool IndexCompareRight(cString lhs, cString rhs) const { return IndexCompare(lhs,rhs)==1; }

		bool IndexCompareEquals(cString lhs, cString rhs) const { return IndexCompare(lhs,rhs)==0; }
		bool DirList(cString path) {
			ClearAll();
			DIR *directory;
			struct dirent * direntry;
			if(path[-1]!='/') path+="/";
			directory=opendir(path);
			if (!directory) {
				cerr<<"couldn't open "<<path<<endl;
				return false;
			}
			while ((direntry=readdir(directory))) {
				if (direntry->d_name[0]!='.') {
					(*this)+=direntry->d_name;
				} //end if
			} //end while
			closedir(directory);
			return Length()!=0;
		}
		
		
		int Grep(cString needle) {
			if(Length()==0) return -1;
			for(int i=0; i<Length(); i++) if((*this)[i].Contains(needle)) return i;
			return -1;
		}
		bool GetFiles(cString path, cString pattern, bool norec=false);
		
		static bool FileExists(const cString & fname);
	private:
		void FromString_innerloop(const cString & strng, cString delim, cString & sub, int & i);

	protected:
     		cString * Array;
		int allocated;   //allocated size of the array
		int used;
		void GetArray();   // Call new and check for errors
		
		void Resize( int NewSize );
		static cString NULLSTR;
	
};

class cArgs:public cStringList {
	private:
		int argc;
		char **argv;
		cString lead;
	
	public:
		bool HasExact(cString needle) {
			for(int i=0; i<Length(); i++) if((*this)[i]==needle) return true;
			return false;
		}
	
	
		bool IsSet(cString _options) { return HasExact(lead+_options); }
		cString GetArg(cString _options, int optarg) {
			int theindex=(*this)[lead+_options];
			if(theindex==-1) return "";
			
			if(theindex+optarg>Length()) return "";
			return (*this)[theindex+optarg];
		}
		cArgs(int _argc, char **_argv, cString _lead) {
			argv=_argv;
			argc=_argc;
			lead=_lead;
			FromArray(_argc,_argv);
		}
		int GetArgc() { return argc; }
		char **GetArgv() { return argv; }
		cString GetLead() { return lead; }
};
inline ostream & operator << ( ostream & Out, const cStringList &Value ) {
	for(int i=0; i<Value.Length(); i++) Out<<Value[i]<<endl;
	return Out;
}



#endif






