/***************************************************************
 * (C) Copyright 1998-2011 Chris Delezenski
 * This software is released under the LGPL v3, see COPYING and LGPL
 ***************************************************************/
#ifndef __OPEN_STRING_LIB_H__
#define __OPEN_STRING_LIB_H__
using namespace std;
#include <fstream>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef WIN32
#include <termios.h>
#include <signal.h>
#endif
//forward declarations
class cString;
class cStringList;
class cVectorList;
class cDualList;

#ifndef MIN
#define MIN(x,y)     (((x) < (y)) ? (x) : (y))
#define MAX(x,y)     (((x) > (y)) ? (x) : (y))
#define MID(x,y,z)   MAX((x), MIN((y), (z)))
#endif
/***************************************************************
 * (C) Copyright 1998-2010 Chris Delezenski
 * This software is released under the LGPL v2, see COPYING and LGPL
 ***************************************************************/
#ifndef ___DYNSTRING___H
#define ___DYNSTRING___H
//using namespace std;

#define MAX_STRING_LEN 1024
#define NUMERIC_STRING 33

// cString class interface: support operations for cStrings
//
// CONSTRUCTION: with (a) no initializer or (b) a
//     const char * (or char *), or (c) another cString
//
// ******************PUBLIC OPERATIONS*********************
// =                      --> Usual assignment
// [ ]                    --> Indexing with bounds check
// ==, !=, <, <=, >, >=   --> Usual relational and equality
// << and >>              --> Input and output
// unsigned int Length( ) --> Return strlen equivalent


//this define was made to ease transition from another string class
#define GetInt() AtoI()
#ifndef SA
#define	SA	struct sockaddr
#endif






class cStringList;
class cString {
	public:
		cString( ) : BufferLen( -1 ) { Buffer=new char[1]; Buffer[0]='\0'; }
		cString( const cString & Value );
		cString( const string Value    );
		cString( const char * Value    );
		cString( const char Value      );
		cString( const double Value    );
		cString( const int Value       );
		cString( const unsigned long int Value);
		cString( const bool Value);
		cString( const off_t Rhs );
		#ifndef WIN32
		cString( const uid_t Rhs );
		#endif

		// ********** Assignment ***************** // 
		cString & operator=(const char * Rhs);
		cString & operator=(const std::string Rhs);
		cString & operator=( const cString & Rhs );
		cString & operator=( const double Rhs );
		cString & operator=( char Rhs );
		cString & operator=( int Rhs );
		cString & operator=( const off_t Rhs );
		cString & operator=( unsigned long int Rhs );
		cString & operator=( bool Rhs );

		const char* c_str() const { return Buffer; }
		operator const char * ( ) const { return Buffer; }
		operator char * ( ) { return Buffer; }
		const int AtoI() const { return atoi(Buffer);}
		const cString AtoSI() const { return cString(atoi(Buffer));}
		const long int AtoL() const {return atol(Buffer);}
		const double AtoF() const {return atof(Buffer);}
		const cString AtoBS() const { cString temp; temp=(*this); if(temp.ToUpper()=="TRUE") temp="TRUE"; else temp="FALSE"; return temp; }
		const cString Limit(int maxlen, cString ending) const;			
		const cString PadandLimit(int maxlen, cString ending) const;			
		const string StoS() const { static string temp; temp=Buffer; return temp; }

		// Destructor
		~cString( ) { if( BufferLen != -1 ) delete [ ] Buffer; }
		
		// *********** Concatenation ***************//
		const cString & operator+=( char Rhs ) { return operator+=(cString(Rhs)); }
		const cString & operator+=( const cString & Rhs );
	
		//Adds the right hand side to a copy of the current string and returns the copy
		cString operator+( const cString & Rhs) const;
		cString operator+( const char & Rhs) const;
		cString operator+( const char * Rhs) const;
		cString operator+( const int Rhs) const { return operator+(cString(Rhs)); }
	
		//adds the Rhs to the front of the string
		void operator-=( const cString & Rhs) { cString temp=Rhs+(*this); (*this)=temp; }
	
		//helps the compiler make the desicion for ambiguous statements
		friend const cString operator +(const char*,const cString);
	

		// *********** Direct Access ***********//
		char operator[ ]( int Index ) const;
		char & operator[ ]( int Index );
		int Length( ) const { return int(strlen( Buffer )); }
		int length( ) const { return int(strlen( Buffer )); }
		bool Contains(const cString &search) const { return strstr(Buffer,search.Buffer)!=NULL; }
		bool Contains(char s);
		bool ContainsAnyOf(const cString &pool) {
			for(int i=0; i<pool.Length(); i++) if(Contains(pool[i])) return true;
			return false;
		}

		bool PartialMatch(const cString &pool, const cString &sep) const;



		bool StartsWith(cString s) const { 
			if(s.Length()<Length()) return s==Part(0,s.Length()-1);
			if(s.Length()>Length()) return s.Part(0,Length()-1)==(*this);
			return s==(*this);
		}	

		bool EndsWith(cString s) const { 
			if(s.Length()<Length()) return s==Part(Length()-s.Length(),Length()-1);
			if(s.Length()>Length()) return s.Part(0,Length()-1)==(*this);
			return s==(*this);
		}	


		
		bool IsTrue() {
			return cString::IsTrue(*this);
		}
		static bool IsTrue(cString s) {
			return s.ToUpper()=="TRUE";
		}
		bool IsFalse() {
			return cString::IsFalse(*this);
		}
		static bool IsFalse(cString s) {
			return !IsTrue(s);
		}
		
		int ContainsXtimes(char s);
		bool ContainsCase( cString search) { return ToUpper().Contains(search.ToUpper()); }
		bool ContainsDigit();
		bool IsNumber();
		bool IsBool() { return (ToUpper()=="TRUE" || ToUpper()=="FALSE"); }
		bool IsString() { return (!IsNumber() && !IsBool()); }
		
		void SetTrue() { (*this)="TRUE"; }
		void SetFalse() { (*this)="FALSE"; }
		
		// *********** comparisions ********** //
		//cString vs cString

		friend bool operator==( const cString & Lhs, const cString & Rhs );
		friend bool operator!=( const cString & Lhs, const cString & Rhs );
		friend bool operator< ( const cString & Lhs, const cString & Rhs );
		friend bool operator> ( const cString & Lhs, const cString & Rhs );
		friend bool operator<=( const cString & Lhs, const cString & Rhs );
		friend bool operator>=( const cString & Lhs, const cString & Rhs );

		//cString vs char*
		friend bool operator==( const cString & Lhs, const char* Rhs );
		friend bool operator!=( const cString & Lhs, const char* Rhs );
		friend bool operator< ( const cString & Lhs, const char* Rhs );
		friend bool operator> ( const cString & Lhs, const char* Rhs );
		friend bool operator<=( const cString & Lhs, const char* Rhs );
		friend bool operator>=( const cString & Lhs, const char* Rhs );
	
		//char* vs cString
		friend bool operator==( const char* Lhs, const cString & Rhs );
		friend bool operator!=( const char* Lhs, const cString & Rhs );
		friend bool operator< ( const char* Lhs, const cString & Rhs );
		friend bool operator> ( const char* Lhs, const cString & Rhs );
		friend bool operator<=( const char* Lhs, const cString & Rhs );
		friend bool operator>=( const char* Lhs, const cString & Rhs );


		//cString vs string
		friend bool operator==( const cString & Lhs, const string Rhs );
		friend bool operator!=( const cString & Lhs, const string Rhs );
		friend bool operator< ( const cString & Lhs, const string Rhs );
		friend bool operator> ( const cString & Lhs, const string Rhs );
		friend bool operator<=( const cString & Lhs, const string Rhs );
		friend bool operator>=( const cString & Lhs, const string Rhs );
	
		//string vs cString
		friend bool operator==( const string Lhs, const cString & Rhs );
		friend bool operator!=( const string Lhs, const cString & Rhs );
		friend bool operator< ( const string Lhs, const cString & Rhs );
		friend bool operator> ( const string Lhs, const cString & Rhs );
		friend bool operator<=( const string Lhs, const cString & Rhs );
		friend bool operator>=( const string Lhs, const cString & Rhs );


		//cString vs double
		friend bool operator==( const cString & Lhs, const double Rhs );
		friend bool operator!=( const cString & Lhs, const double Rhs );
		friend bool operator< ( const cString & Lhs, const double Rhs );
		friend bool operator> ( const cString & Lhs, const double Rhs );
		friend bool operator<=( const cString & Lhs, const double Rhs );
		friend bool operator>=( const cString & Lhs, const double Rhs );
		
		//double vs cString
		friend bool operator==( const double Lhs, const cString & Rhs );
		friend bool operator!=( const double Lhs, const cString & Rhs );
		friend bool operator< ( const double Lhs, const cString & Rhs );
		friend bool operator> ( const double Lhs, const cString & Rhs );
		friend bool operator<=( const double Lhs, const cString & Rhs );
		friend bool operator>=( const double Lhs, const cString & Rhs );
	
		//cString vs int
		friend bool operator==( const cString & Lhs, const int Rhs );
		friend bool operator!=( const cString & Lhs, const int Rhs );
		friend bool operator< ( const cString & Lhs, const int Rhs );
		friend bool operator> ( const cString & Lhs, const int Rhs );
		friend bool operator<=( const cString & Lhs, const int Rhs );
		friend bool operator>=( const cString & Lhs, const int Rhs );
		
		//int vs cString
		friend bool operator==( const int Lhs, const cString & Rhs );
		friend bool operator!=( const int Lhs, const cString & Rhs );
		friend bool operator< ( const int Lhs, const cString & Rhs );
		friend bool operator> ( const int Lhs, const cString & Rhs );
		friend bool operator<=( const int Lhs, const cString & Rhs );
		friend bool operator>=( const int Lhs, const cString & Rhs );
			
		// ************ file i/o and std i/o ********************** //
		
		friend istream & operator>> ( istream & In, cString & Value );
		friend ostream & operator<< ( ostream & Out, const cString &Value );
//		friend ostream & operator<< ( ostream & Out, const cString &Value );
	
		bool FromFile(ifstream &inf,int n);
		bool FromFile(ifstream &inf) { return FromFile(inf,MAX_STRING_LEN); }
		
		bool FromFile(cString thefile);
		bool ToFile(cString thefile);

		bool FromFile(FILE* inf);
		bool FromFile(int inf);
//		bool FromMem(memfile& inblock);
		void FromPopen(cString cmdline);
		void FromPopen(cString cmdline, cString grep);
		bool GetLine(istream &input);
		bool GetLine(int inf);
		int ToFile(ofstream &inf,cString ending="\n");
		int ToFile(FILE* inf, cString ending="\n");
		int ToFile(int fd, cString ending="\n");
//		bool ToMem(memfile& outblock);	

		// ************ Formatters ************* //
		//Replaces every occurence of a with b
		const cString Replace(char a,char b) const;
		const cString Replace(const cString &a, const cString & b) const;
	
		const cString Part(int st, int len) const;
		const cString operator()(int Sindex, int Eindex) const { return Part(Sindex,Eindex); }
		const cString ChopRt(int c) const     { return Part(0,-1-c); }
		const cString ChopLf(int c) const     { if(c>Length()-1) return ""; else return Part(c,-1);   }
		const cString ChopLf(char c) const    { 
			if(!Contains(c)) return *this;
			 return ChopLf(SeekLf(c)+1);
		}
		
		const cString ChopRt(char c) const    { 
			if(!Contains(c)) return *this;
			return ChopRt(-SeekRt(c));
		}


		const cString ChopRt(char c, int times) const {
			if(times==1) return this->ChopRt(c);
			return this->ChopRt(c).ChopRt(c,times-1);
		}
		const cString ChopLf(char c, int times) const {
			if(times==1) return this->ChopLf(c);
			return this->ChopLf(c).ChopLf(c,times-1);
		}

		const cString ChopAllLf(char c) const { 
			if(!Contains(c)) return *this;
			return ChopLf(SeekRt(c)+Length()+1); 
		}
		const cString ChopAllRt(char c) const {
			if(!Contains(c)) return *this;
			return ChopRt(Length()-SeekLf(c));
		}
		const cString Reverse() const;   
		const cString Trim() const { return TrimLf().Reverse().TrimLf().Reverse(); }
		const cString TrimLf() const;
		const cString TrimRt() const { return Reverse().TrimLf().Reverse(); } 
		const cString RemoveChar(char c) const;
		int instances(char c) const;
		const cString NoSpaces() const { return RemoveChar(' '); }
		const cString WebFriendly() const ;
		const cString Escape() const;
		const cString ToUpper() const;
		const cString ToLower() const;
		const cString ToTitle() const;
		const cString TitleCase() const { cerr<<"titlecase is deprecated, use ToTitle instead!"<<endl; return ToTitle(); }
		const cString Money() const;
		const cString Hex2Dec() const;
		const cString Dec2Hex() const;
		const bool ToBool() const {
			return ToUpper()=="TRUE";
		}
	
		static const cString BoolToString(bool b) {
			if(b) return "True";
			return "False";
		}
		static const cString BoolToYesNo(bool b) {
			if(b) return "Yes";
			return "No";
		}
		static const cString BoolToOnOff(bool b) {
			if(b) return "On";
			return "Off";
		}
	
		cString String2IntList();
		cString IntList2String();
		cString CeasarEnCrypt(int slide);
		cString CeasarDeCrypt(int slide);
		cString EnCrypt(int slide);
		cString DeCrypt(int slide);

		bool AppendToFile(cString fname) {
			ofstream ofile;
			ofile.open(fname,ios::app);
			if(!ofile) return false;
			ofile<<fname<<endl;
			ofile.close();
			return true;
		}
		
		static bool AppendTextToFile(cString text, cString fname) {
			return text.AppendToFile(fname);
		}

	
	 protected:
		const int SeekRt(char c) const;
		const int SeekLf(char c) const;
		char *Buffer;             // Stores the chars
		int BufferLen;            // Max strlen for Buffer
		void GetBuffer( unsigned int MaxStrLen );
		char magic;
};

inline const cString operator +(const char* a, const cString b) {
	return cString(a)+b;
}

inline void cString::GetBuffer( unsigned int MaxStrLen ) {
	BufferLen = MaxStrLen;
	Buffer = new char[ BufferLen + 1 ];
	//    len=strlen(Buffer);
}

//cString vs cString
inline bool operator==( const cString & Lhs, const cString & Rhs ) {
	return strcmp( Lhs.Buffer, Rhs.Buffer ) == 0;
}

inline bool operator!=( const cString & Lhs, const cString & Rhs ) {
	return strcmp( Lhs.Buffer, Rhs.Buffer ) != 0;
}

inline bool operator<( const cString & Lhs, const cString & Rhs ) {
	return strcmp( Lhs.Buffer, Rhs.Buffer ) < 0;
}

inline bool operator>( const cString & Lhs, const cString & Rhs ) {
	return strcmp( Lhs.Buffer, Rhs.Buffer ) > 0;
}

inline bool operator<=( const cString & Lhs, const cString & Rhs ) {
	return strcmp( Lhs.Buffer, Rhs.Buffer ) <= 0;
}

inline bool operator>=( const cString & Lhs, const cString & Rhs ) {
	return strcmp( Lhs.Buffer, Rhs.Buffer ) >= 0;
}

//cString vs char*
inline bool operator==( const cString & Lhs, const char* Rhs ) {
	return strcmp( Lhs.Buffer, Rhs ) == 0;
}

inline bool operator!=( const cString & Lhs, const char* Rhs ) {
	return strcmp( Lhs.Buffer, Rhs ) != 0;
}

inline bool operator<( const cString & Lhs, const char *Rhs ) {
	return strcmp( Lhs.Buffer, Rhs ) < 0;
}

inline bool operator>( const cString & Lhs, const char* Rhs ) {
	return strcmp( Lhs.Buffer, Rhs ) > 0;
}

inline bool operator<=( const cString & Lhs, const char* Rhs ) {
	return strcmp( Lhs.Buffer, Rhs ) <= 0;
}

inline bool operator>=( const cString & Lhs, const char* Rhs ) {
	return strcmp( Lhs.Buffer, Rhs ) >= 0;
}

//char* vs cString
inline bool operator==( const char *Lhs, const cString & Rhs ) {
	return strcmp( Lhs, Rhs.Buffer ) == 0;
}

inline bool operator!=( const char *Lhs, const cString & Rhs ) {
	return strcmp( Lhs, Rhs.Buffer ) != 0;
}

inline bool operator<( const char* Lhs, const cString & Rhs) {
	return strcmp( Lhs, Rhs.Buffer ) < 0;
}

inline bool operator>( const char* Lhs, const cString & Rhs ) {
	return strcmp( Lhs, Rhs.Buffer ) > 0;
}

inline bool operator<=( const char* Lhs, const cString & Rhs ) {
	return strcmp( Lhs, Rhs.Buffer ) <= 0;
}

inline bool operator>=( const char* Lhs, const cString & Rhs ) {
	return strcmp( Lhs, Rhs.Buffer ) >= 0;
}

//cString vs string
inline bool operator==( const cString & Lhs, const string Rhs ) {
	return strcmp( Lhs.Buffer, Rhs.c_str() ) == 0;
}

inline bool operator!=( const cString & Lhs, const string Rhs ) {
	return strcmp( Lhs.Buffer, Rhs.c_str() ) != 0;
}

inline bool operator<( const cString & Lhs, const string Rhs ) {
	return strcmp( Lhs.Buffer, Rhs.c_str() ) < 0;
}

inline bool operator>( const cString & Lhs, const string Rhs ) {
	return strcmp( Lhs.Buffer, Rhs.c_str() ) > 0;
}

inline bool operator<=( const cString & Lhs, const string Rhs ) {
	return strcmp( Lhs.Buffer, Rhs.c_str() ) <= 0;
}

inline bool operator>=( const cString & Lhs, const string Rhs ) {
	return strcmp( Lhs.Buffer, Rhs.c_str() ) >= 0;
}

//string vs cString
inline bool operator==( const string Lhs, const cString & Rhs ) {
	return strcmp( Lhs.c_str(), Rhs.Buffer ) == 0;
}

inline bool operator!=( const string Lhs, const cString & Rhs ) {
	return strcmp( Lhs.c_str(), Rhs.Buffer ) != 0;
}

inline bool operator<( const string Lhs, const cString & Rhs) {
	return strcmp( Lhs.c_str(), Rhs.Buffer ) < 0;
}

inline bool operator>( const string Lhs, const cString & Rhs ) {
	return strcmp( Lhs.c_str(), Rhs.Buffer ) > 0;
}

inline bool operator<=( const string Lhs, const cString & Rhs ) {
	return strcmp( Lhs.c_str(), Rhs.Buffer ) <= 0;
}

inline bool operator>=( const string Lhs, const cString & Rhs ) {
	return strcmp( Lhs.c_str(), Rhs.Buffer ) >= 0;
}


//cString vs double
inline bool operator==( const cString & Lhs, const double Rhs ) {
	return Lhs.AtoF() == Rhs;
}

inline bool operator!=( const cString & Lhs, const double Rhs ) {
	return Lhs.AtoF() != Rhs;
}

inline bool operator<( const cString & Lhs, const double Rhs ) {
	return Lhs.AtoF() < Rhs;
}

inline bool operator>( const cString & Lhs, const double Rhs ) {
	return Lhs.AtoF() > Rhs;
}

inline bool operator<=( const cString & Lhs, const double Rhs ) {
	return Lhs.AtoF() <= Rhs;
}

inline bool operator>=( const cString & Lhs, const double Rhs ) {
	return Lhs.AtoF() >= Rhs;
}

//double vs cString
inline bool operator==( const double Lhs, const cString & Rhs ) {
	return Lhs == Rhs.AtoF();
}

inline bool operator!=( const double Lhs, const cString & Rhs ) {
	return Lhs != Rhs.AtoF();
}

inline bool operator< ( const double Lhs, const cString & Rhs ) {
	return Lhs < Rhs.AtoF();
}

inline bool operator> ( const double Lhs, const cString & Rhs ) {
	return Lhs > Rhs.AtoF();
}

inline bool operator<=( const double Lhs, const cString & Rhs ) {
	return Lhs <= Rhs.AtoF();
}

inline bool operator>=( const double Lhs, const cString & Rhs ) {
	return Lhs >= Rhs.AtoF();
}

//cString vs int
inline bool operator==( const cString & Lhs, const int Rhs ) {
	return Lhs.AtoI() == Rhs;
}

inline bool operator!=( const cString & Lhs, const int Rhs ) {
	return Lhs.AtoI() != Rhs;
}

inline bool operator<( const cString & Lhs, const int Rhs ) {
	return Lhs.AtoI() < Rhs;
}

inline bool operator>( const cString & Lhs, const int Rhs ) {
	return Lhs.AtoI() > Rhs;
}

inline bool operator<=( const cString & Lhs, const int Rhs ) {
	return Lhs.AtoI() <= Rhs;
}

inline bool operator>=( const cString & Lhs, const int Rhs ) {
	return Lhs.AtoI() >= Rhs;
}

//int vs cString
inline bool operator==( const int Lhs, const cString & Rhs ) {
	return Lhs == Rhs.AtoI();
}

inline bool operator!=( const int Lhs, const cString & Rhs ) {
	return Lhs != Rhs.AtoI();
}

inline bool operator< ( const int Lhs, const cString & Rhs ) {
	return Lhs < Rhs.AtoI();
}

inline bool operator> ( const int Lhs, const cString & Rhs ) {
	return Lhs > Rhs.AtoI();
}

inline bool operator<=( const int Lhs, const cString & Rhs ) {
	return Lhs <= Rhs.AtoI();
}

inline bool operator>=( const int Lhs, const cString & Rhs ) {
	return Lhs >= Rhs.AtoI();
}


//console io

inline istream & operator >> ( istream & In, cString & Value ) {
    static char Str[ MAX_STRING_LEN ];
    In >> Str;
    Value = Str;
    return In;
}

inline ostream & operator << ( ostream & Out, const cString &Value ) {
    return Out << Value.Buffer;
}

// Attribute codes: 
// 00=none 01=bold 04=underscore 05=blink 07=reverse 08=concealed
// Text color codes:
// 30=black 31=red 32=green 33=yellow 34=blue 35=magenta 36=cyan 37=white
// Background color codes:
// 40=black 41=red 42=green 43=yellow 44=blue 45=magenta 46=cyan 47=white

#define TEXT_FGCOL_BLACK 30
#define TEXT_FGCOL_RED 31
#define TEXT_FGCOL_GREEN 32
#define TEXT_FGCOL_YELLOW 33
#define TEXT_FGCOL_BLUE 34
#define TEXT_FGCOL_MAGENTA 35
#define TEXT_FGCOL_CYAN 36
#define TEXT_FGCOL_WHITE 37
#define TEXT_FGCOL_NORMAL 39
class cColor {
	public:
		static cString Black() { return cString("\033[1;"+cString(TEXT_FGCOL_BLACK)+"m"); }
		static cString Red() { return cString("\033[1;"+cString(TEXT_FGCOL_RED)+"m"); }
		static cString Green() { return cString("\033[1;"+cString(TEXT_FGCOL_GREEN)+"m"); }
		static cString Yellow() { return cString("\033[1;"+cString(TEXT_FGCOL_YELLOW)+"m"); }
		static cString Blue() { return cString("\033[1;"+cString(TEXT_FGCOL_BLUE)+"m"); }
		static cString Magenta() { return cString("\033[1;"+cString(TEXT_FGCOL_MAGENTA)+"m"); }
		static cString Cyan() { return cString("\033[1;"+cString(TEXT_FGCOL_CYAN)+"m"); }
		static cString White() { return cString("\033[1;"+cString(TEXT_FGCOL_WHITE)+"m"); }
		static cString Normal() { return cString("\033[0;"+cString(TEXT_FGCOL_NORMAL)+"m"); }
/*
TODO:
#define TEXT_BKCOL_BLK 40
#define TEXT_BKCOL_RED 41
#define TEXT_BKCOL_GREEN 42
#define TEXT_BKCOL_YELLOW 43
#define TEXT_BKCOL_BLUE 44
#define TEXT_BKCOL_MAGENTA 45
#define TEXT_BKCOL_CYAN 46
#define TEXT_BKCOL_WHITE 47
*/

};


#endif







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






/***************************************************************
 * (C) Copyright 1998-2010 Chris Delezenski
 * This software is released under the LGPL v2, see COPYING and LGPL
 ***************************************************************/
#ifndef ___VECTORLIST_______H
#define ___VECTORLIST_______H

class cVectorList {
	public:

		// ********** Constructors ************ //
		cVectorList( int Size =0 );
		~cVectorList( )       { delete [ ] Array; }
		void init() {
			allocated=0;
			used=0;
			Array=NULL;
			
		}

		const cVectorList & operator=( const cVectorList & Rhs );
		const cVectorList & operator=( const cStringList & Rhs ) {
			Resize(1);
			(*this)[0]=Rhs;
			return *this;
		}
		
		// ********* Concatenation ************ //
		void operator+=( const cStringList &Rhs ) {
			if(IsOptimum()) Resize(allocated+500);
			(*this)[used]=Rhs;
			used++;	
		}
		void operator+=( const cVectorList &Rhs) { for(int i=0; i<Rhs.Length(); i++)   (*this)+=Rhs[i]; }
		void operator-=( const cStringList &Rhs); 
		void operator-=( const cVectorList &Rhs) { for(int i=-1; i>-Rhs.Length(); i--) (*this)-=Rhs[i]; }


		// ******** Accessors ******** //
		const cStringList & operator[] ( int Index ) const;
		cStringList & operator[] ( int Index );
		int operator[ ]( cStringList & I ) const;
		cString operator[]( cString I ) const { return (Array[defaultindex])[I]; } 
		cString Efficiency() { return cString(used)+"/"+cString(allocated); }
		const int Length() const { return used; }
		bool IsOptimum() { return used==allocated; }

		// ********** Modifiers **************** //
		void ClearAll() { 	for(int i=0; i<used; i++) Array[i].ClearAll(); }
		void Compact();
		void Sort(int row); //by row
		void Insert(int index, cStringList s);
		void Reverse();
		
		void Swap(int i,int j) {
			cStringList temp;
			temp=Array[i];
			Array[i]=Array[j];
			Array[j]=temp;
		}

		void RemoveKth(int k) { Array[k].ClearAll(); Compact(); }

		// ********** File IO ************ //
		bool FromList(cStringList &list,cString sep) {
			for(int i=0; i<list.Length(); i++) {
				if(sep!="" && !list[i].Contains(sep)) continue;
				if(!AddRow(list[i],sep)) return false;
			
			}
			return true;
		}
	
		bool FromFile(const cString &fname, cString sep="\t") {
			cStringList thefile;
			if(!thefile.FromFile(fname)) return false;
			return FromList(thefile,sep);
		}
		
		bool FromFile(int fd,cString sep="\t") {
			cStringList thefile;
			if(!thefile.FromFile(fd)) return false;
			return FromList(thefile,sep);
		}
		
		bool FromFile(ifstream & ifstream,cString sep="\t") {
			cStringList thefile;
			if(!thefile.FromFile(ifstream)) return false;
			return FromList(thefile,sep);
		}
		
		bool FromFile(FILE* fileptr,cString sep="\t") {
			cStringList thefile;
			if(!thefile.FromFile(fileptr)) return false;
			return FromList(thefile,sep);
		}

		bool FromPopen(cString cmdline, cString sep="\t" ) {
			cStringList thefile;
			thefile.FromPopen(cmdline);
			return FromList(thefile,sep);
		}			
		bool FromPopenGrep(cString cmdline, cString grep, cString sep="\t") {
			cStringList thefile;
			thefile.FromPopen(cmdline,grep);
			return FromList(thefile,sep);
		}			

/*		int FromMem(memfile &f, cString sep) {
			cStringList temp;
			int bytes=0;
			bytes=temp.FromMem(f);
			if(bytes!=0) FromList(temp,sep);
			return bytes;
		}
	*/	
		bool FromStdin() { return FromFile(STDIN_FILENO); }
		
		void FromArray(int argc,char *argv[],cString sep="\t") {
			cStringList thefile;
			thefile.FromArray(argc,argv);
			FromList(thefile,sep);
		}
	
		void FromArray(char *argv[],cString sep="\t") {
			cStringList thefile;
			thefile.FromArray(argv);
			FromList(thefile,sep);
		}
	
		void ToList(cStringList &list,cString sep="\t",int s=0, int ed=-1);
		void ToFile(const cString &fname,cString sep="\t",int s=0, int ed=-1, cString ending="\n") {
			cStringList temp;
			ToList(temp,sep,s,ed);
			temp.ToFile(fname,s,ed,ending);
		}

/*		int ToMem(memfile &f, cString sep="\t") {
			cStringList temp;
			ToList(temp,sep);
			return temp.ToMem(f);
		}	
*/
		void ToFile(int fd,cString sep="\t",int s=0, int ed=-1) {
			cStringList thefile;
			ToList(thefile,sep,s,ed);
			thefile.ToFile(fd);
		}

		void ToFile(ofstream & outfile,cString sep="\t") {
			cStringList thefile;
			ToList(thefile,sep);
			thefile.ToFile(outfile);
		}

		void ToFile(FILE* fileptr,cString sep="\t") {
			cStringList thefile;
			ToList(thefile,sep);
			thefile.ToFile(fileptr);
		}

		void ToStdout() { return ToFile(STDOUT_FILENO); }
		void ToStderr() { return ToFile(STDERR_FILENO); }
		void ToArray(int argc,char *argv[],cString sep="\t") {
			cStringList thefile;
			ToList(thefile,sep);
			thefile.ToArray(argc, argv);
		}

		void ToArray(char *argv[],cString sep="\t") {
			cStringList thefile;
			ToList(thefile,sep);
			thefile.ToArray(argv);
		}
				

	protected:
		void SetDefaultIndex( int index) { defaultindex=index; }
		bool AddRow(cString &line,cString sep);
		void AddRow(cStringList & line,cString sep);
		void CreateRow(cString &newline,int thisline,cString sep='\t', int s=0, int ed=-1);
		cStringList * Array;
		static int allocated;   //allocated size of the array
		static int used;
		void GetArray( );   // Call new and check for errors
		void Resize( int NewSize );
		int defaultindex;
		bool Checkforblankrow(int y) { for(int i=0; i< used; i++) if(Array[i][y]!="") return false; return true; }
		static cString NULLSTR;
};
#endif








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




/***************************************************************
 * (C) Copyright 1998-2011 Chris Delezenski
 * This software is released under the LGPL v2, see COPYING and LGPL
 ***************************************************************/
#ifndef __TIME_AND_DATE__H
#define __TIME_AND_DATE__H

struct sDay {
	   int oftheweek;   //0-6
	   int ofthemonth;  //0-30
	   int oftheyear; //0-365(6)
};


//translates from a string to the numeric month
class cString;
class cTimeAndDate {     //contains information about a particular day
	protected:
		int thisyear;     //2000 would be 2000 and so on
		int thismonth;  //numerical month between 0-11
		sDay thisday;
		bool verbose, debug;
		int timeofday[24];
		int hour,minute,second;
		bool isUTC;
		cString timezone;

	public:
		long unsigned int SecondsSince1970(); // return # of seconds since January 1, 1970

		int GetYear() const      { return thisyear;  }
		int GetMonthInt() const { return thismonth; }  //returns 1 thru 12
		cString GetMonthString() const;  //returns January thru December
  

		//almost an external function, doesn't change the class at all,
		//simply returns the corresponding int value, also used internally.
		int GetMonth(const cString& s) const;

		//returns the day structure
		const sDay GetDay() const  { return thisday;   }
		cString GetDayOfWeek() const;


		//set any part of the date manually
		void SetYear(const int pYear) { thisyear=pYear; }
  		void SetMonth(const int pMonth) { *this=cTimeAndDate(pMonth, thisday.ofthemonth, thisyear); }
    		void SetMonth(const cString &m) { *this=cTimeAndDate(GetMonth(m),thisday.ofthemonth,thisyear); }
    		void SetDay(const int pDay) { *this=cTimeAndDate(thismonth,pDay,thisyear); }
		void SetFromDate();
		bool SetFromString(cString s);

		cString ToString();
		
		//default constructors
		cTimeAndDate(int m, int d, int y);
                cTimeAndDate(const cTimeAndDate& d);
		cTimeAndDate(time_t *t);
		cTimeAndDate();
		const cTimeAndDate & operator=(const cTimeAndDate &d); // assignment operator
		const cTimeAndDate & operator=(time_t *t); // assignment operator


	      //displays the current date in a compact form
	      //example:   	7/16/2000
		const cString TodaysDateCompact() const { return cString(thismonth)+"/"+cString(thisday.ofthemonth)+"/"+cString(thisyear); }

	      //displays the current date in a nice form
	      //example:  "July 16, 2000"
		const cString TodaysDate() const { return GetMonthString()+" "+cString(thisday.ofthemonth)+", "+cString(thisyear); }

		//returns the length of any month, ie limit of July would 31
		int LengthOfMonth(int month) const;
                int LengthOfMonth() const { return LengthOfMonth(thismonth); }

  		int LengthOfYear() const {
    			return int(IsLeapYear())+365; //returns 366 for leapyear, 365 otherwise
                }


		//this function is valid iff IsValiDate returns true
    		bool IsLeapYear() const { return (thisyear%4==0 && thisyear%100!=0) || thisyear%400==0; }
		bool IsValiDate() const;

		//several common operators		
		bool operator< (const cTimeAndDate &d) const;
    		bool operator==(const cTimeAndDate &d) const { return thisyear==d.thisyear && thismonth==d.thismonth && thisday.ofthemonth==d.thisday.ofthemonth; }
  		bool operator<=(const cTimeAndDate &d) const { return *this<d || *this==d; }
  
		bool operator!=(const cTimeAndDate &d) const { return !(*this==d); }
		bool operator> (const cTimeAndDate &d) const { return !(*this<=d); }
        	bool operator>=(const cTimeAndDate &d) const { return !(*this<d); } 
		
		bool Debug() { return debug; }
		void SetDebug(bool _debug) { debug=_debug; }
		bool Verbose() { return verbose; }
		void SetVerbose(bool _verbose) { verbose=_verbose; }
		

		//miltary to real time
		static int Military2RealTime( int h, bool &ispm) {
			if(h==0) {
				ispm=false;
				return 12;
			}
			if(h<=12) {
				ispm=false;
				return h;
			}
			ispm=true;
			return h-12;
		}
		//real time to military time
		static int RealTime2MilitaryTime(int h,bool ispm) {
			if(!ispm && h==12) return 0; //midnight
			if(!ispm || (ispm && h==12)) return h; //am reduces to same time in mil time
			return h+12; //for 1pm on, hours range from 13-23
		}
		//converts a number into a string depicting a day
		static cString Number2Day(int i,int st);
		//same as previous function, but shorter day strings
		static cString Number2ShortDay(int i,int st);
		//Converts a string day into it's number
		static int Day2Number(cString day,int st);
		
		//set the time and date using the external "date" command for now
		static bool SetTimeAndDate(int _m, int _d, int _hour, int _min, int _y);
		static  unsigned long int Years2Days(unsigned long int n) { return n*365; }
		static unsigned long int Days2Hours(unsigned long int n) { return n*24; }
		static unsigned long int Hours2Min(unsigned long int n)     {return n*60; }
		static unsigned long int Min2Sec(unsigned long int n)   { return n*60; }
		static unsigned long int Years2Sec(unsigned long int n) { return Min2Sec(Hours2Min(Days2Hours(Years2Days(n)))); }
		static unsigned long int Days2Years(unsigned long int n)  { return n/365; }
		static  unsigned long int Hours2Days(unsigned long int n)  { return n/24; }
		static unsigned long int Min2Hours(unsigned long int n)   { return n/60; }
		static  unsigned long int Sec2Min(unsigned long int n) { return n/60; }
		static  unsigned long int Sec2Years(unsigned long int n) { return Days2Years(Hours2Days(Min2Hours(Sec2Min(n)))); }
		
		void Refresh();
		cString CurTime();
		int GetHour() { return hour; }
		int GetMinute() { return minute; }
		cString Getampm() { if(hour>11) return "pm"; else return "am"; }
		
		bool IsPm() { return Getampm()=="pm"; }
		bool IsAm() { return !IsPm(); }
		bool IsUTC() { return isUTC; }
		void SetUTC(bool _isUTC) { isUTC=_isUTC; }
		void SetTimezone(cString _tz) { timezone=_tz; }
		bool ApplyTimeZone(std::ostream & fout) {
			fout<<"ZONE=\""<<timezone<<"\""<<endl;
			fout<<"UTC="<<cString::BoolToString(isUTC)<<endl;
			fout<<"ARC=false"<<endl;
			return true;
		}
		static bool SetTimeZone(cString tz, bool isUTC, bool tostdout, cString external_exec="");
		
		
		static cString Ctime() {
			cString temp;
			time_t timer;			//time_t for time functions
			time_t tp;
			timer = time(&tp);
			temp=ctime(&timer);
			return temp;
		}
		
};

#endif

/***************************************************************
 * (C) Copyright 1998-2011 Chris Delezenski
 * This software is released under the LGPL v2, see COPYING and LGPL
 ***************************************************************/
#ifndef __EXECUTIVE__H
#define __EXECUTIVE__H


// Unfortunately, this class is not currently available for Windows
#ifndef WIN32
#include <syslog.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#ifdef OSX
#include <signal.h>
#endif


#include <fcntl.h>
#define TWIDDLE_DEFAULT cString("|/-\\|/-\\")
#define MSG_DEFAULT cString("default:wait")
/**********************************************************************************************************/
// Executive Class
// (c) Chris Delezenski <chris.delezenski@gmail.com>
// 
// This class wraps the execution of programs making use of pipe(2), exec(3), popen(3), and system(3)
// The goal is to provide an api where "should be easy" operations involving external programs 
// can be manipulated and interacted with.
// Secondary goal is to eliminate the need for system(3) and popen(3) as these functions are known for 
// security deficiencies, since they both rely on /bin/sh
/*************************************************************************************************************/
class cStringList;
class cExec {

	protected:
		bool useenvironment;
		cString path;
		bool debug;
		bool usefork;
		bool verbose;
		
		static void child_handler(int signum);
		int successful_forks;
	//	static cDualList pid_status;
		static void Output(cString _msg) {
			#ifndef WIN32
			syslog(LOG_INFO,"%s",(char*)(cString(getpid())+" : "+_msg));
			#endif
			cerr<<cString(getpid())<<" : "<<_msg<<endl;
		}
	public:
		//Program arguments
		cStringList argvalues;
		
		//Environment for the program.  Used for forcing our own environment on a new process.
		// Typically this list contains elements such as HOME=/home/cmd
		cStringList environment;

		
		//execute the list if not debug
		//otherwise just show it
		bool Exec();
		void SetDebug() { debug=true; }
		cExec() { useenvironment=false; debug=false; path=""; usefork=false; verbose=false; }
		void UseEnvironment() { useenvironment=true; }

		bool IsDebug() { return debug; }
		bool IsEnvironment() {return useenvironment; } 
		
		
		void Show(ostream &fout);
		
		
		//The path for the program is the program itself
		//There may be situations where we'd want to execute a particular program but use a different path
		//    Calling the same program with a different 0th argument can change the behavior of the program
		//    A good example of this would be /usr/bin/reboot (symlink to consolehelper)
		void SetPath(const cString &_path) { path=_path; }
		
		
		
		void Show() { Show(cout); }
		bool FromCommandline(cString cmdline);
		bool Verbose() { return verbose; }
		void SetVerbose(bool _verbose) { verbose=_verbose; }
		
		//void SetCallingProgram(cString _msg) { msg=_msg; }
		
		//Some programs don't behave correctly in a straight fork/exec and it is necessary to force system(3) to be used anyway
		static bool System(cString cmdline, bool forcesystem3=false) {
			if(cmdline.ContainsAnyOf("<>|&*")) forcesystem3=true;
			if(forcesystem3) return system(cmdline)==0;
			cExec myexec;
			return myexec.ForkExecFG(cmdline);
		}
		
		
		bool ForkExecFG(cString cmdline, int &status);
		bool ForkExecFG(cString cmdline) { int status=-42; return ForkExecFG(cmdline,status); }
		bool ForkExecBG(cString cmdline);

		bool ForkExecCaptureFG(cString cmdline, cStringList &output, int &status);
		static bool ForkExecCaptureFGGrep(cString cmdline,cStringList &thelist, cString grepfor) {
			int status=-42;
			bool r=cExec::ForkExecCaptureFGGrep(cmdline,thelist,grepfor,status);
			return r;
		}
		static bool ForkExecCaptureFGGrep(cString cmdline,cStringList &thelist, cString grepfor, int &status);
		
		int NumOfForks() { return successful_forks; }
	
		//int NumberOfBackgroundedProcesses() { return pid_status.Length()-successful_forks; }
		//int GetExitCode(pid_t index_pid) { return pid_status[cString(index_pid)].AtoI(); }
		//int operator[](pid_t index_pid) { return  GetExitCode(index_pid); }
		static void InstallSignal(cExec &_exe) {
			signal((int)SIGCHLD,_exe.child_handler);
		}
		bool WaitForExit(cString _msg);
		static bool WaitForExit(cString _msg, int &status, pid_t& pid);
		
		
		//The following functions are pipe manipulations
		//  More information is provided in the .cpp file
		static int OpenPipeTo(cString cmdline, pid_t &pid);

		static pid_t DualPipeToCommand(const char * cmd, int & pipein, int & pipeout) {
			cExec myexe;
			myexe.FromCommandline(cmd);
			return DualPipeToCommand(myexe.argvalues,pipein,pipeout);
		}
		static pid_t DualPipeToCommand(const cStringList &args, int & pipein, int & pipeout);

		static cString GetFirstLine_PopenAlt(cString cmdline, int &retval) {
			cExec myexe;
			myexe.FromCommandline(cmdline);
			return GetFirstLine_PopenAlt(myexe.argvalues,retval);
		}
		static cString GetFirstLine_PopenAlt(cStringList &args, int &retval);

		static pid_t PopenAlt(cString cmdline, int &pipeout) {
			cExec myexe;
			myexe.FromCommandline(cmdline);
			return PopenAlt(myexe.argvalues,pipeout);
		}
		static pid_t PopenAlt(cStringList &args, int &pipeout);
		
		
		// An example of a very low-level copy
		static bool CopyFile(cString src, cString target) {
			int inF, ouF;
			char line[512];
			int bytes;
			if((inF = open(src, O_RDONLY)) == -1) {
				perror("open");
				return false;
			}
			
			if((ouF = open(target, O_WRONLY | O_CREAT)) == -1) {
				perror("open");
				return false;
			}
			
			while((bytes = read(inF, line, sizeof(line))) > 0)
				write(ouF, line, bytes);
				
			close(inF);
			close(ouF);
			return true;
		}
		static bool ShellOut(cString shell, cString warning="");
		static bool BashOut() {
			return ShellOut("/bin/bash");
		}
		
		static bool SudoBashOut() {
			return ShellOut("/usr/bin/sudo /bin/bash");
		}

		static void WhileWaiting(ostream & wout, cString msg, cString twiddle, pid_t pid) {
			while(true) {
				for(int i=0; i<twiddle.Length(); i++) {
					wout<<msg<<" "<<twiddle[i]<<flush;
					usleep(300);
					wout<<"\r\b"<<flush;
				}
			}
		}
		
		//Exec a program in the background and do something interesting while we wait for that process to complete, such as create a "twiddle"
		static bool ExecWhileWaiting3(cString cmdline, cString msg=MSG_DEFAULT, cString twiddle=TWIDDLE_DEFAULT) {
			pid_t pid=fork();
			switch(pid) {
				case -1:
					cerr<<"fork error"<<endl;
					exit(1);
				break;
				case 0:
					//close(2);
					//close(1);
					//cExec execme;
					//execme.FromCommandline(cmdline);
					//execme.Exec();
					//_exit(1);
					cExec::System(cmdline,true);
					_exit(0);
				break;
			}
			pid_t pid2=fork();
			switch(pid2) {
				case -1:
					cerr<<"fork error"<<endl;
					exit(1);
				break;
				case 0:
					if(msg=="default:wait") msg="waiting for child ("+cString(pid)+")";
					WhileWaiting(cout,msg,twiddle,pid);
					_exit(0);
				break;
			}
			
			int status=-255;
			waitpid(pid,&status,0);
			//	cerr<<"child died, kill my twiddle"<<endl<<"kill("<<pid2<<",9)"<<endl;
			
			kill(pid2,9);
			waitpid(pid2,NULL,0);
			return !(status==128 || status==16 || status==8);
		}

		
		static bool ExecWhileWaiting2(cString cmdline, cString msg=MSG_DEFAULT, cString twiddle=TWIDDLE_DEFAULT) {
			pid_t pid=fork();
			switch(pid) {
				case -1:
					cerr<<"fork error"<<endl;
					exit(1);
				break;
				case 0:
					close(2);
					close(1);
					cExec execme;
					execme.FromCommandline(cmdline);
					execme.Exec();
					_exit(1);
				break;
			}
			pid_t pid2=fork();
			switch(pid2) {
				case -1:
					cerr<<"fork error"<<endl;
					exit(1);
				break;
				case 0:
					if(msg=="default:wait") msg="waiting for child ("+cString(pid)+")";
					WhileWaiting(cout,msg,twiddle,pid);
					_exit(0);
				break;
			}
			
			int status=-255;
			waitpid(pid,&status,0);
			//	cerr<<"child died, kill my twiddle"<<endl<<"kill("<<pid2<<",9)"<<endl;
			
			kill(pid2,9);
			waitpid(pid2,NULL,0);
			return !(status==128 || status==16 || status==8);
		}


		static bool ExecWhileWaiting(cString cmdline, cString msg=MSG_DEFAULT, cString twiddle=TWIDDLE_DEFAULT) {
			pid_t pid=fork();
			switch(pid) {
				case -1:
					cerr<<"fork error"<<endl;
					exit(1);
				break;
				case 0:
					close(2);
					close(1);
					if(cmdline.StartsWith("/bin/cp ")) sleep(1);
					cExec execme;
					execme.FromCommandline(cmdline);
					execme.Exec();
					_exit(1);
				break;
			}
			pid_t pid2=fork();
			switch(pid2) {
				case -1:
					cerr<<"fork error"<<endl;
					exit(1);
				break;
				case 0:
					if(msg=="default:wait") msg="waiting for child ("+cString(pid)+")";
		
					WhileWaiting(cout,msg,twiddle,pid);
					_exit(0);
				break;
			}
			int status=-255;
			waitpid(pid,&status,0);
			//	cerr<<"child died, kill my twiddle"<<endl<<"kill("<<pid2<<",9)"<<endl;
			kill(pid2,9);
			waitpid(pid2,NULL,0);
			return (status==0);
		}
		static bool ChrootJail(cString directory, cString cmdline) { return ChrootJail(directory,"",cmdline); }
		static bool ChrootJail(cString directory, cString chtodir, cString cmdline);

};
#else
class cStringList;
class cExec {
	public:
		static bool ForkExecFG(cString cmdline, int &status) {
			status=system(cmdline);
			return status==0;
		}
		static bool ForkExecBG(cString cmdline, int &status) {
			cmdline+="&";
			status=system(cmdline);
			return status==0;
		}
		static bool ForkExecCaptureFG(cString cmdline, cStringList &output, int &status) {
			output.FromPopen(cmdline);
			status=0;
			return true;
		}
		

};
#endif
#endif
/***************************************************************
 * (C) Copyright 1998-2010 Chris Delezenski
 * This software is released under the LGPL v2, see COPYING and LGPL
 ***************************************************************/
#ifndef ____KEYBOARD____H___
#define ____KEYBOARD____H___


enum { KEY_UP, KEY_DOWN, KEY_RIGHT, KEY_LEFT, KEY_UNDEF, KEY_INS, KEY_DEL, KEY_PDN, KEY_PUP, KEY_END, KEY_HOME };
class cString;
class cKeyboard {
	protected:
		cString TextPrompt(cString label, cString defselect, bool password, bool canbreakout);
		int EscapeHandler();
		void DrawGenericPrompt(cString msg, cString currentchoice, int length=0);
		void Backspace(int fd, int num );
		void f5throughf8();
		void Slide_Indicator_Draw(cString label, int percent, char symbol);
		bool escapepressed;
		bool debug;
		bool dontusestty;

	public:
		cKeyboard() {
			dontusestty=false;
			debug=false;
			if(!init()) cerr<<"failed to init keyboard"<<endl;
		}
		cKeyboard(bool _dontusestty, bool _debug) {
			debug=_debug;
			dontusestty=_dontusestty;
			if(!init()) cerr<<"failed to init keyboard"<<endl;
			else if(debug) cerr<<"keyboard initialized"<<endl;
		}
		
		~cKeyboard() {
			if(!release()) cerr<<"failed to release keyboard"<<endl;
			else if(debug) cerr<<"successfully released keyboard"<<endl;
		
		}
	
		bool EscapePressed() {
			bool orig=escapepressed;
			escapepressed=false;
			return orig;
		}
	
		bool init() { 
			escapepressed=false;
			if(dontusestty) return true;
			return system( "stty -echo -icanon min 1 erase ^?" )==0;
		}
	
		bool release() {
			if(dontusestty) return true;
			return system( "stty echo erase ^? icanon" )==0;
		}
	

		void SetDebug() { debug=true; }
		bool GetDebug() { return debug; }
		void ClearDebug() { debug=false; }


		void PressEnter() { char c=' '; do { read(STDIN_FILENO,&c,1); } while(c!='\n'); }
		
		void FlushInput() { tcflush(fileno(stdin), TCIFLUSH); }
	
		void PressEnterFlushed() {
			FlushInput();
			PressEnter();
		}
		cString TextPrompt(cString label, cString defselect, bool canbreakout=false) { return TextPrompt(label,defselect, false, canbreakout); }
		cString ListPrompt(cString label, cString validchoices, int defaultval, bool excessive);
		cString ListPrompt(cString label, cString validchoices, cString defaultval, bool excessive);
		cString ListPrompt(cString label, cStringList & validchoices, int defaultval, bool excessive);
		cString ListPrompt(cString label, cStringList & validchoices, cString defaultval, bool excessive);
		cString PasswordPrompt(cString label, cString defselect, bool canbreakout=false) { return TextPrompt(label,defselect,true,canbreakout); }
		int SlidePrompt(cString usingmsg, cString promptmsg, int currentpct, char symbol);
		cString Test();
};


class cConsolePrompt {
	public:
//		bool IsReleased() { return released; }
		static void PressEnter(bool verbose) {
			if(verbose) cerr<<"Press Enter to Continue"<<endl;
			cKeyboard *kybd;
			kybd=new cKeyboard;
			kybd->PressEnter();
			delete kybd;
		}
		static cString ListPromptwithCustom(cString label, cString list, cString defaultval) {
			cKeyboard *kybd=NULL;
			kybd=new cKeyboard(false,false);
			if(defaultval!="" && !list.Contains(defaultval)) list+=","+defaultval;
			cString temp=kybd->ListPrompt(label,list,defaultval,false);
			delete kybd;
			if(temp=="custom") {
				kybd=new cKeyboard(false,false);
				temp=kybd->TextPrompt(label,defaultval,false);
				delete kybd;
				cerr<<endl;
			}
			return temp;
		}
		static cString ConfigPrompt(cString label, cString list, cString defaultval) {
			if(!list.Contains("custom")) list+=",custom";
			return ListPromptwithCustom(label,list,defaultval);
		}

		static bool BoolPrompt(cString label, bool defaultval) {
			cKeyboard *kybd=NULL;
			kybd=new cKeyboard(false,false);
			int choice=0;
			if(defaultval) choice=1;
			bool ret=(kybd->ListPrompt(label,"no,yes",choice,false)=="yes");
			delete kybd;
			return ret;
		}
		
		static cString ListPrompt(cString label, cString thelist, cString defaultval, bool excess) {		
			cString ret="";
			cKeyboard *kybd=NULL;
			kybd=new cKeyboard(false,false);
			ret=kybd->ListPrompt(label, thelist, defaultval, excess);
			delete kybd;
			return ret;
		}
		static cString TextPrompt(cString label, cString defaultval, bool excess) {
			cString ret="";
			cKeyboard *kybd=NULL;
			kybd=new cKeyboard(false,false);
			ret=kybd->TextPrompt(label, defaultval, excess);
			delete kybd;
			return ret;
		}
};

#endif
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
#endif
