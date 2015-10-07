/***************************************************************
 * (C) Copyright 1998-2010 Chris Delezenski
 * This software is released under the LGPL v2, see COPYING and LGPL
 ***************************************************************/
#include "internal.h"

// ******* Assignment ****** //
const cStringList & cStringList::operator= ( const cStringList & Rhs ) {
	if( this != &Rhs ) {
		Resize(Rhs.allocated);
		used=Rhs.used;
		for( int i = 0; i < Rhs.used; i++ ) Array[ i ] = Rhs.Array[ i ];
	}
	return *this;
}

cStringList::cStringList( int Size ) {
	skipblanks=true;
	Array=NULL;
	allocated = Size;
	used=0;
	GetArray();
	if(Array!=NULL && allocated==0) { exit(1);}
}

cStringList::cStringList( const cStringList & Rhs ) {
	skipblanks=Rhs.skipblanks;
	if( this != &Rhs ) {
	        Resize(Rhs.allocated);
		used=Rhs.used;
	        for( int i = 0; i < used; i++ ) Array[i]=Rhs.Array[i];
	}
}

// ******* concatenation ****** //
void cStringList::operator-=(const cString &Rhs) {
	cStringList temp;
	temp+=Rhs;
	for(int i=0; i<Length(); i++)
		temp+=(*this)[i];
	(*this)=temp;
}

// ****** Comparision ****** //
const int operator ==( const cStringList &Lhs, const cStringList &Rhs ) {
	if(Lhs.Length()!=Rhs.Length()) return false;
	for(int i=0; i<Lhs.Length(); i++)
		if(Lhs[i]!=Rhs[i]) return false;
		return true;
}

const int operator !=( const cStringList &Lhs, const cStringList &Rhs ) {
	if(Lhs.Length()!=Rhs.Length()) return true;
	for(int i=0; i<Lhs.Length(); i++)
		if(Lhs[i]!=Rhs[i]) return true;
		return false;
}

const int operator < ( const cStringList &Lhs, const cStringList &Rhs ) {
	return Lhs.Length()<Rhs.Length();
}

const int operator > ( const cStringList &Lhs, const cStringList &Rhs ) {
	return Lhs.Length()>Rhs.Length();
}

const int operator <= ( const cStringList &Lhs, const cStringList &Rhs ) {
	return Lhs.Length()<=Rhs.Length();
}

const int operator >= ( const cStringList &Lhs, const cStringList &Rhs ) {
	return Lhs.Length()>=Rhs.Length();
}

// ****** Accessors **** //		
//int Length( ) const { return used; }
//int Allocated() const { return allocated; }
//bool Contains(cString s) { for(int i=0; i<Length(); i++) if(Array[i]==s) return true; return false; }
//bool IsOptimum() { return (used==allocated); }
//cString Efficiency() { return cString(used)+"/"+cString(allocated); }
const cString & cStringList::operator[ ]( int Index ) const {
	if( abs(Index) > Length()) { return NULLSTR; }
	if( Index <0 ) Index+=Length();
	return Array[Index];
}

cString & cStringList::operator[ ]( int Index ) {
	if( abs(Index) > Length()) { return NULLSTR; }
	if( Index <0 ) Index+=Length();
	return Array[Index];
}

int cStringList::operator[ ]( cString I ) const {
	for(int i=0; i<used; i++) if(Array[i]==I) return i;
	return 0;
}

// ********** Modifiers **************** //
//void ClearAll() { Resize(0); }			//effectly destroys the entire array.
void cStringList::Compact() {
	cStringList temp(Length());
	for(int i=0; i<used; i++) {
		if((*this)[i]!="") {
			if(!((*this)[i].Length()==1 && isspace((*this)[i][0])) ) {
				temp+=(*this)[i];
			}
		}
	}
	(*this)=temp;
	used=temp.Length();
	if(temp.Length()!=allocated) {
		Resize(temp.Length());
	}
}

void cStringList::UCompact() {
	cStringList temp(Length());
	int j=0;
	for(int i=0; i<used; i++) {
		if((*this)[i]!="") {
			if(!( ((*this)[i].Length()==1 && isspace((*this)[i][0])) || (i>0 && (*this)[i]==(*this)[i-1]))) {
				temp[j]=(*this)[i];
				j++;
			}
		}
	}
	(*this)=temp;
	used=j;
	if(j!=allocated) Resize(j);
}

void cStringList::Insert(int index,cString s) {
	cStringList temp;
	for(int i=0; i<index; i++) temp+=(*this)[i];
	temp+=s;
	for(int j=index; j<Length(); j++) temp+=(*this)[j];
	(*this)=temp;
}

void cStringList::Reverse() {
	cStringList temp;
	for(int i=0; i<Length(); i++) temp-=Array[i];
	*this=temp;
}

void cStringList::Swap(int i,int j) {
	cString temp;
	temp=Array[i];
	Array[i]=Array[j];
	Array[j]=temp;
}

void cStringList::Sort() {
	cString temp;
	for(int i=0; i<Length(); i++) {
		for(int j=i; j<Length(); j++) {
			if(Array[i]>Array[j] && i<j) Swap(i,j);
		}
	}
}             

//.void TrimAll();//Call "Trim()" for each string in the list

// ********** File IO **************** //
bool cStringList::FromFile(const cString & fname) {
	if(fname.Contains('*')) return false;
	#ifndef WIN32
	if(!FileExists(fname)) return false;
	struct stat buf;
	stat(fname,&buf);
	ifstream infile(fname);
	
	if(fname.Contains("/proc") || buf.st_size==0) {
		if(!FromFile(infile)) { infile.close(); return false; }
	} else {
		if(!FromFile(infile,buf.st_size)) { infile.close(); return false; }
	}
	infile.close();
	
	
	if((*this)[-1]=="" && Length()!=0) used--;
	#else
	FILE* fptr=fopen(fname,"r");
	FromFile(fptr);
	fclose(fptr);
	#endif
	
	return true;
}

bool cStringList::FromFile(int fd) {
	FILE* fptr=fdopen(fd,"r");
	bool r=FromFile(fptr);
	fclose(fptr);
	return r;
}

bool cStringList::FromFile(ifstream & infile,int size) {
	if(!infile) return false;
	cString temp,temp2;
	used=0;
	bool quit=false;
	while(!infile.eof() && !quit) {
		temp.FromFile(infile,size+1);
		if(temp.Length()+1==size) quit=true;
		if(temp!="" || !skipblanks) {
			if(used+1>allocated) {
				Resize(allocated+500);
				if(used+1>allocated) { cerr<<"Double failed!"<<endl; exit(1); }
			}
			(*this)[used]=temp;
			used++;
		}
	}
	if((*this)[-1]=="" && Length()!=0) used--;
	return true;
}

FILE* cStringList::FromFile(FILE* fname) {
	cString temp;
	while(feof(fname)==0) {
		temp.FromFile(fname);
		if(temp!="") (*this)+=temp;
	}
	if((*this)[-1]=="" && Length()!=0) used--;
	return fname;
}

//bool FromStdin() { return FromFile(STDIN_FILENO); }

/*int cStringList::FromMem(struct memfile &f) {
	int len;
	int bytes=0;
	bytes+=memread(f,&len,sizeof(int));
	cString temp="";
	char * tempchar=NULL;
	for(int i=0; i<len; i++) {
		tempchar=(char*)f.start;
		tempchar+=f.offset;
		temp=tempchar;
		
		bytes+=temp.Length()+1;
		f.offset+=temp.Length()+1;
		(*this)+=temp;
		temp="";
	}
	if((*this)[-1]=="" && Length()!=0) used--;
	return bytes;
}
*/
void cStringList::FromString(const cString &strng, cString delim) {
	if(delim.Length()==1) return FromString(strng,delim[0]);
	delete [] Array;
	Array=NULL;
	allocated=0;
	used=0;	
	Resize(500);
		
	cString substrng=delim;
	cString temp;
	used=1;
	for(int i=0; i<strng.Length(); ) FromString_innerloop(strng,delim,substrng, i);
}

void cStringList::FromStringWithSpaces(const cString &_mystring) {
	char delim=' ';
	ClearAll();
	cString mystring=_mystring+delim;
	cString temp;
	bool ignoredelim=false;
	
	for(int i=0; i<mystring.Length(); i++) {
		if(mystring[i]=='\"') ignoredelim=!ignoredelim;
		if(mystring[i]!=delim || ignoredelim) {
			if(mystring[i]!='\"') temp+=mystring[i];
		} else {
			(*this)+=temp;
			temp="";
		}
	}
}

void cStringList::FromString(const cString &mystring, char delim) {
	if(delim==' ') return FromStringWithSpaces(mystring);
	used=0;
	allocated=0;
	delete [] Array;
	Resize(mystring.Length());
	cString strng=mystring;
	strng+=delim;
	cString temp;
	for(int i=0; i<strng.Length(); i++) {
		if(strng[i]!=delim) {
			temp+=strng[i];
		} else {
			if(used+1>Length()) Resize(allocated+500);
			(*this)[used]=temp;
			used++;
			temp="";
		}
	}
	
}

void cStringList::FromParagraph(const cString &mystring, int width) {
	
	if(mystring.Length()<width) {
		this->Resize(0);
		(*this)+=mystring;
	
	}
	//step one, create a list of words
	cStringList words;
	words.FromString(mystring," ");
	cStringList newlist;
	//step two, reconstruct a new list of multiple lines with multiple words,
	//conditions: each line may be no more than <width> chars long
	//each line must end in a space.
	cString linebuffer="";
	cString templinebuffer="";
	for(int i=0; i<words.Length(); i++) {
		if(linebuffer=="") {
			linebuffer=words[i];
			templinebuffer=linebuffer;
		} else  {
			templinebuffer=linebuffer+" "+words[i];
		}
		
		if(templinebuffer.Length()>width) {
			//cerr<<"adding "<<linebuffer<<endl;
			newlist+=linebuffer;
			linebuffer=words[i];
		}
		 else {
			//if(linebuffer=="") linebuffer=words[i];
			//else
			 linebuffer=templinebuffer;
		}
	}
	newlist+=linebuffer;
	(*this)=newlist;
}


void cStringList::FromMultipleParagraphs(int width) {

	cStringList newmasterlist;
	cStringList *templist;
	cString tempstring;
	for(int i=0; i<Length(); i++) {
		templist=new cStringList;
		tempstring=(*this)[i];
		templist->FromParagraph(tempstring,width);
		tempstring="";
		newmasterlist+=(*templist);
		delete templist;
		templist=NULL;
	}
	(*this)=newmasterlist;
}

bool cStringList::FromArray(int argc,char *argv[]) {
	for(int i=0; i<argc; i++) (*this)+=cString(argv[i]);
	return true;
}

bool cStringList::FromArray(char *argv[]) {
	for(int i=0; argv[i]!=NULL; i++) (*this)+=cString(argv[i]);
	return true;
}

int cStringList::ToFile(const cString &fname,int s, int ed, cString ending) {
	if(fname.Contains('*')) return 0;
	if(ed==-1) ed=Length();
	ofstream ofile(fname);
	int r=ToFile(ofile,s,ed,ending);
	ofile.close();
	return r;
}

int ToFile(int fd,int s, int ed, cString ending) {
	cerr<<"cStringList::ToFile: fixme: NOT IMPLEMENTED!"<<endl;
	return 0;
}

int cStringList::ToFile(ofstream &ofile,int s, int ed, cString ending) {
	if(!ofile) return 0;
	if(ed==-1) ed=Length();
	int j=0;
	for(j=s; j<ed; j++) {
		if(ending=="\n") ofile<<Array[j]<<endl;
		else ofile<<Array[j]<<ending;
	}
	return j-s;
}

int cStringList::ToFile(FILE* fp, int s, int ed, cString ending) {
	if(fp==NULL) return 0;
	if(ed==-1) ed=Length();
	cString temp="";
	for(int j=s; j<ed; j++) {
		temp=Array[j]+ending;
		temp.ToFile(fp);
	}
	return ed-s;
}

//int ToStdout(int s=0, int ed=-1, cString ending="\n") { return ToFile(STDOUT_FILENO,s,ed,ending); }
//int ToStderr(int s=0, int ed=-1, cString ending="\n") { return ToFile(STDERR_FILENO,s,ed,ending); }
	
/*int cStringList::ToMem(struct memfile &f) {
	int len=Length();
	int bytes=0;
	bytes+=memwrite(f,&len,sizeof(int));
	for(int i=0; i<len; i++) bytes+=memwrite(f,(*this)[i],(*this)[i].Length()+1);

	return bytes;
}
*/
cString cStringList::ToString(cString &ret, cString s) {
	ret="";
	if(Length()==0) return ret;

	ret=Array[0];
	for(int i=1; i<Length(); i++)
		ret+=s+Array[i];
	return ret;
}

//cString ToString(cString &line,char delim) { return ToString(line,cString(delim)); }
//cString ToString(cString delim) { cString temp; return ToString(temp,delim); }
//cString ToString(char delim) { cString temp; return ToString(temp,delim); }
	
bool cStringList::ToArray(int &argc,char *argv[]) {
	
	argv=new char* [Length()+1];
	
	
//	argv[Length()]=NULL;
	argc=Length();
	
	for(int i=0; i<Length(); i++) {
		argv[i]=new char[(*this)[i].Length()+1];
		strcpy(argv[i],(*this)[i]);
	}
	
	return true;

}

char ** cStringList::ToCharList() {
	char **argv=new char* [Length()+1];
	
	argv[Length()]=NULL;
	
	for(int i=0; i<Length(); i++) {
		argv[i]=new char[(*this)[i].Length()+1];
		strcpy(argv[i],(*this)[i]);
	}
	return argv;
}


bool cStringList::ToArray(char *argv[]) {
	argv=new char* [Length()+1];
	
	argv[Length()]=NULL;
	
	for(int i=0; i<Length(); i++) {
		argv[i]=new char[(*this)[i].Length()+1];
		strcpy(argv[i],(*this)[i]);
	}
	return true;
}

void cStringList::FromString_innerloop(const cString & strng, cString delim, cString &substrng, int & i) {
	int s=strng.Length();
	for(int j=0; j<delim.Length() && i+j<strng.Length(); j++) substrng[j]=strng[i+j];
	if(substrng!=delim && i<s) {
		if(used+1>allocated) {
			Resize(allocated+500);
		}
		Array[used-1]+=strng[i];
		i++;
	} else {
		used++;
		i+=substrng.Length();
	}
}

void cStringList::GetArray() {
	if(allocated==0) {
		used=0;
		if(Array!=NULL) delete [] Array;
		Array=NULL;
		return;
	} 
	Array = new cString [ allocated ];
}

bool cStringList::FileExists( const cString & fname ) {
	ifstream infile2(fname);
	if (! infile2) return false;
	infile2.close();
	return true;
}

void cStringList::Resize( int NewSize ) {
	if(NewSize==0) { 
		if(Array!=NULL) { delete [] Array; Array=NULL; }
		used=0; allocated=0; 
		return;
	}

	if(NewSize==allocated) return;

	if(Array==NULL) {
		allocated=NewSize;
		used=0;
		GetArray();
		return;
	}
	else if(allocated==0) {
		cerr<<"WARNING: Array is not null and allocated is 0, this is bad"<<endl;
		cerr<<"ERR requested size was "<<NewSize<<endl;
		exit(1);
	}
	
	cString * OldArray;
	OldArray = Array;                    // Save array
	Array=NULL;				// Nullify old array pointer
	allocated = NewSize;
	GetArray( );
	for( int i = 0; i < used; i++ )
	        Array[ i ] = OldArray[ i ];
	delete [ ] OldArray;
	
}

void cStringList::FromPopen(cString cmdline) {
	ClearAll();
	FILE* fileptr=NULL;
	fileptr=popen(cmdline, "r" );
	if(fileptr!=NULL) {
		FromFile(fileptr);
		pclose(fileptr);
	}
	
	Compact();
	if(Length()>1)
		if((*this)[-1]==(*this)[-2]) {
			(*this)[-1]="";
		}
	Compact();
}

void cStringList::FromPopen(cString cmdline, cString grepme) {
	FromPopen(cmdline);
	for(int i=0; i<Length(); i++) 
		if(!(*this)[i].Contains(grepme)) { (*this)[i]=""; }
	Compact();
}


/*
ostream & operator<<( ostream & Out, const cStringList &list ) {
	if(list.Length()==0) return Out;
	cString temp="";
	for(int i=0; i<list.Length(); i++)
		Out<<list[i]<<endl;
	return Out<<temp;
}
*/

cString cStringList::NULLSTR="";

bool cStringList::GetFiles(cString path, cString pattern, bool norec) {
	cString cmdline="find "+path;
	if(norec) cmdline+=" -maxdepth 1";
	cmdline+=" -iname \""+pattern+"\" -type f";
	ClearAll();
	
	FromPopen(cmdline);
	if(Length()==0) {
//		cerr<<"length is 0, returning false"<<endl;
		return false;
	}
	
	//(*this)[-1]="";
	Compact();
	return true;
}
