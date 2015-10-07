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







