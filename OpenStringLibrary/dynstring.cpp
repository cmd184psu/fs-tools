/***************************************************************
 * (C) Copyright 1998-2010 Chris Delezenski
 * This software is released under the LGPL v2, see COPYING and LGPL
 ***************************************************************/

#include "internal.h"

cString::cString( const cString & Value ) {
	GetBuffer( strlen( Value.Buffer ) );
	strcpy( Buffer, Value.Buffer );
}

cString::cString( const char *Value ) {
	if( Value == NULL ) { 
		GetBuffer( 0 );
		Buffer[ 0 ] = '\0';
	} else {
		GetBuffer( strlen( Value ) );
		strcpy( Buffer, Value );
	}
}

cString::cString(const double Value) {
	char temp[NUMERIC_STRING]="";
	sprintf(temp,"%f",(float)Value);
	GetBuffer(strlen(temp));
	strcpy(Buffer,temp);
}

const cString cString::Dec2Hex() const {
	cString value=(*this);
	long int internal_value=strtol(value,NULL,10);
	char *value2=new char [20];
	sprintf(value2,"%#lx",internal_value);
	value=value2;
	delete value2;
	return value;
}

const cString cString::Hex2Dec() const {
	cString value=*this;
	unsigned long int internal_value=strtol(value,NULL,16);
	return cString(internal_value);
}

cString::cString(const char Value) {
	GetBuffer(1);
	Buffer[0]=Value;
	Buffer[1]='\0';
}

cString::cString(const int Value) {
	char temp[NUMERIC_STRING]="";
	sprintf(temp,"%i",Value);
	GetBuffer(strlen(temp));
	strcpy(Buffer,temp);
}
#ifndef WIN32
cString::cString(const uid_t Value) {
	char temp[NUMERIC_STRING]="";
	sprintf(temp,"%i",int(Value));
	GetBuffer(strlen(temp));
	strcpy(Buffer,temp);
}
#endif
cString::cString(const unsigned long int Value) {
	char temp[NUMERIC_STRING]="";
	sprintf(temp,"%li",Value);
	GetBuffer(strlen(temp));
	strcpy(Buffer,temp);
}

cString::cString(const off_t Value) {
	char temp[NUMERIC_STRING]="";
	sprintf(temp,"%ld",(long int)Value);
	GetBuffer(strlen(temp));
	strcpy(Buffer,temp);
}


cString::cString(const bool Value) {
	GetBuffer(6);
	
	if(Value) strcpy(Buffer,"true");
	else strcpy(Buffer,"false");
}


const cString cString::PadandLimit(int maxlen, cString ending) const {
	if(maxlen<Length()) return Limit(maxlen-ending.Length(),ending);
	cString temp, temp2;
	int l=maxlen-Length();
	for(int i=0; i<l; i++) temp+=' ';
	temp2=(*this)+temp;
	return (*this)+temp;
}

const cString cString::Limit(int maxlen, cString ending) const {
	if(maxlen>=Length()) return *this;
	return this->ChopRt(Length()-maxlen)+ending;
}



cString & cString::operator=( const char * Rhs ) {
	if( Rhs == NULL ) Rhs="";
	const int Len = strlen( Rhs );
	if( Len >= BufferLen ) {
		if( BufferLen != -1 ) delete [ ] Buffer;
	        GetBuffer( Len );
	}
	strcpy( Buffer, Rhs );
	return *this;
}


cString & cString::operator=( const cString & Rhs ) {
	int Len=0;
	if(strcmp(Rhs.Buffer,"")!=0) Len=strlen( Rhs.Buffer );
	if( this == &Rhs ) return *this;
		
        if( Len >= BufferLen ) {
		if( BufferLen != -1 ) delete [ ] Buffer;
		GetBuffer( Len );
        }
        strcpy( Buffer, Rhs.Buffer );
	return *this;
}


cString & cString::operator=( char Rhs ) {
	GetBuffer(1);
	Buffer[0]=Rhs;
	Buffer[1]='\0';
	return *this;
}

cString & cString::operator=( const double Rhs ) {
	char temp[NUMERIC_STRING]="";
	sprintf(temp,"%f",(float)Rhs);
	GetBuffer(strlen(temp));
	strcpy(Buffer,temp);
	return *this;
}

cString & cString::operator=( const int Rhs ) {
	char temp[NUMERIC_STRING]="";
	sprintf(temp,"%i",Rhs);
	GetBuffer(strlen(temp));
	strcpy(Buffer,temp);
	return *this;
}

cString & cString::operator=( const unsigned long int Rhs ) {
	char temp[NUMERIC_STRING]="";
	sprintf(temp,"%li",Rhs);
	GetBuffer(strlen(temp));
	strcpy(Buffer,temp);
	return *this;
}

cString & cString::operator=( const off_t Rhs ) {
	char temp[NUMERIC_STRING]="";
	sprintf(temp,"%ld",(long int)Rhs);
	GetBuffer(strlen(temp));
	strcpy(Buffer,temp);
	return *this;
}

cString & cString::operator=( const bool Rhs ) {
	if(Rhs) *this="true";
	else *this="false";
	return *this;
}


// *********** Concatenation ***************//
const cString & cString::operator+=( const cString & Rhs ) {
	char *temp;
	temp=new char[Length()+Rhs.Length()+5];
	strcpy(temp,Buffer);
	strcat(temp,Rhs.Buffer);
	*this=temp;
	delete temp;
	return *this;
}

cString cString::operator+( const cString & Rhs) const {
	cString temp=*this;
	temp+=Rhs;
	return temp;
}

cString cString::operator+( const char & Rhs) const {
	cString temp=*this;
	temp+=Rhs;
	return temp;
}

cString cString::operator+( const char * Rhs) const {
	cString temp=*this;
	temp+=Rhs;
	return temp;
}

// *********** Direct Access ***********//
char & cString::operator[ ]( int Index ) {
	if(Length()==0) { magic='\0'; return magic; } //{ cerr<<"nothing to return"<<endl; exit(1); }
	if(Index<0) Index+=Length();
	if(Index<0) { cerr<<"Requested Negative Index is beyond the length of this string, check code "<<Index<<"/"<<Length()<<endl; exit(1); }
	if(Index>Length()) { cerr<<"Requested Index is beyond the length of this string, check code. "<<Index<<"/"<<Length()<<endl; exit(1); }
	return Buffer[ Index ];
}

char cString::operator[ ]( int Index ) const {
	if(Length()==0) { return '\0'; } //{ cerr<<"nothing to return"<<endl; exit(1); }
	if(Index<0) Index+=Length();
	if(Index<0) { cerr<<"Requested Negative Index is beyond the length of this string, check code "<<Index<<"/"<<Length()<<endl; exit(1); }
	if(Index>Length()) { cerr<<"Requested Index is beyond the length of this string, check code. "<<Index<<"/"<<Length()<<endl; exit(1); }
	return Buffer[ Index ];
}

bool cString::Contains(char c) {
	for(int i=0; i<Length(); i++) if(Buffer[i]==c) return true;
	return false;
}

bool cString::PartialMatch(const cString& parseme, const cString& sep) const {
	cStringList filter;
	filter.FromString(parseme,"*");
	filter.Compact();

	//filter list is empty due to a bad parseme, ignore and reject
	if(filter.Length()==0) return false;

	//all conditions in the filter must be true to pass
	for(int i=0; i<filter.Length(); i++) {
		if(!this->ToLower().Contains(filter[i].ToLower())) return false;
	}
	return true;
}



// ************ file i/o and std i/o ********************** //

bool cString::FromFile(ifstream &inf, int n) {
	char *line=new char[n+1]; 
	if(inf) inf.getline(line,n,'\n');
	else return false;
	(*this)=line;
	return true;
}

bool cString::FromFile(FILE* inf) {
	char line[MAX_STRING_LEN]="";
	if (fgets(line,MAX_STRING_LEN,inf)!=NULL) { 
 	 	if(line[strlen(line)-1]=='\n') {
		  	(*this)=line;
			(*this)=(*this).ChopRt('\n');
	  	} else {
		 	(*this)=line;
		}
	} else {
		return false;
	}
	return true;
}

/*bool cString::FromMem(memfile& inblock) {
	char tempbuf[MAX_STRING_LEN];
	cString tempstring(MAX_STRING_LEN);
	memread(inblock,(void*)tempbuf, MAX_STRING_LEN*sizeof(char));
	(*this)=(char*)tempbuf;
	return true;
}
*/

bool cString::FromFile(cString thefile) {
	cStringList temp;
	if(!temp.FromFile(thefile)) return false;
	cString temp2;
	if(temp.Length()>1) temp.ToString(temp2,"\n");
	else if(temp.Length()==1) temp2=temp[0];
	else return false;
	*this=temp2;
	return true;
}

bool cString::ToFile(cString thefile) {
	ofstream fout(thefile);
	if(!fout) return false;

	fout<<(*this)<<endl;
	fout.close();
	return true;
}




void cString::FromPopen(cString cmdline) {
	cStringList capture;
	capture.FromPopen(cmdline);
	capture.Compact();
	if(capture.Length()!=0) 
		(*this)=capture[0];
	else
		(*this)="";
}

void cString::FromPopen(cString cmdline, cString grepme) {
	cStringList thelist;
	pclose(thelist.FromFile(popen(cmdline, "r" )));
	thelist.Compact();
	for(int i=0; i<thelist.Length(); i++) {
		if(thelist[i].Contains(grepme)) {
			(*this)=thelist[i];
			return;
		}
	}
	(*this)="";
}

//designed to read in lines that are smaller than MAX_STRING_LEN; otherwise, bail out
bool cString::GetLine(int fd) {
	char *line=NULL;
	line=new char[MAX_STRING_LEN];
	if(line==NULL) return false;
	if(fd<0) return false;
	
	//reads in line up to MAX_STRING_LEN-1 or end of line
	int bytes=read(fd,line,MAX_STRING_LEN-2);
	if(bytes==0) return false; //nothing to read
	if(bytes==MAX_STRING_LEN-2) return false; //line too long to be safe 
	line[bytes]='\0';
	if(line[bytes-1]=='\n') line[bytes-1]='\0';
	(*this)=cString(line);
	return true;
}

bool cString::GetLine(istream &input) {
	char line[MAX_STRING_LEN]="";
	if(input) input.getline(line,MAX_STRING_LEN,'\n');
	else return false;
	(*this)=line;
	return true;
}

int cString::ToFile(ofstream &inf,cString ending) {
	if(!inf) return 0;
	if(ending=="\n") {
		inf<<(*this)<<endl;
		return Length()+1;
	} else {
		inf<<Buffer<<ending;
		return Length()+ending.Length();
	}
}

int cString::ToFile(FILE* inf, cString ending) {
	if(inf==NULL) return 0;
	if(ending=="\n") {
		fprintf(inf,"%s\n",Buffer);
		return Length()+1;
	} else {
		fprintf(inf,"%s%s",Buffer,ending.Buffer);
		return Length()+ending.Length();
	}		
}

int cString::ToFile(int fd, cString ending) {
	if(fd<0) return 0;
	cString temp=(*this)+ending;
	return write(fd,temp.Buffer,temp.Length());
}

/*bool cString::ToMem(memfile& outblock) {
	ssize_t size=memwrite(outblock,(void*)Buffer,Length()*sizeof(char)+1);
	return (size>0);
}
*/
// ************ Formatters ************* //

const cString cString::Replace(char a, char b) const {
	cString temp=*this;
	for(int i=0; i<Length(); i++) if(temp[i]==a) temp[i]=b;
	return temp;
}

const cString cString::Replace(const cString &a, const cString &b) const {
	cString temp="";
	for(int i=0; i<Length(); ) {
		if(Part(i,i+a.Length()-1)==a) {
			i+=a.Length();
			temp+=b; 
		} else  {
			temp+=(*this)[i];
			i++;
		}
	}
	return temp;
}

const cString cString::Part(int st, int len) const {
	if(st<0) st=0;
	if(st>Length()) { cerr<<"cString::Part("<<st<<","<<len<<") starting point is greater than length of string, check code"<<endl; exit(1); }
	if(len<0) { len+=Length(); }
	if(len>Length()) len=Length()-1;
	cString temp="";
	for(int i=st; i<len+1; i++) temp+=Buffer[i];
	//if(temp=="") { cerr<<"cString::Part("<<st<<","<<len<<") returns nothing, check code!"<<endl; exit(1); }
	return temp;
}

const cString cString::Reverse() const {
	cString temp;
	for(int i=0; i<Length(); i++) temp-=Buffer[i];
	return temp;
}

const cString cString::TrimLf() const {
	cString temp="";
	bool firstnonspace=false;
	for(int i=0; i<Length(); i++) {
		if(!(isspace(Buffer[i]) || Buffer[i]==char(13)) || firstnonspace) {
			firstnonspace=true;
			temp+=Buffer[i];
		}
        }
	return temp;
}

const cString cString::RemoveChar(char a) const {
	cString temp="";
	for (int i=0; i<Length(); i++) if((*this)[i]!=a) temp+=(*this)[i];
	return temp;
}

int cString::instances(char c) const {
	int count=0;
	for (int i=0; i< Length(); i++) if(Buffer[i]==c) count++;
	return count;
}

const cString cString::WebFriendly() const {
	cString plain=*this, web="";
	for(int i=0; i<plain.Length(); i++) {
		if(plain[i]=='<') {
			web+="&lt;";
		} else if(plain[i]=='>') {
			web+="&gt;";
		} else if(plain[i]=='\"') {
			web+="&quot;";
		} else if(plain[i]=='&') {
			web+="&amp;";
		} else {
			web+=plain[i];
		}
	}
	return web;
}

const cString cString::Escape() const {
	cString plain=*this, web="";
	for(int i=0; i<plain.Length(); i++) {
		if(plain[i]=='\t') {
			web+="\\t";
		} else if(plain[i]=='\"') {
			web+="\\\"";
		} else if(plain[i]=='\\') {
			web+="\\\\";
		} else {
			web+=plain[i];
		}
	}
	return web;
}

const cString cString::ToUpper() const {
	cString temp=*this;
	for(int i=0; i<Length(); i++) temp[i]=toupper(temp[i]);
	return temp;
}

const cString cString::ToLower() const {
	cString temp=*this;
	for(int i=0; i<Length(); i++) temp[i]=tolower(temp[i]);
	return temp;
}

const cString cString::ToTitle() const {
  Buffer[0]=toupper(Buffer[0]);
  for (int i=1; i<Length(); i++)
     if(Buffer[i-1]==' ') Buffer[i]=toupper(Buffer[i]);
     else Buffer[i]=tolower(Buffer[i]);
  return *this;
}

const cString cString::Money() const {
	cString temp=(*this);
     	if(Length()==0) return "$0.00";
   	cString contain="0123456789.";
	for(int i=0; i<temp.Length(); i++)
		if(!contain.Contains(temp[i])) return "$0.00";

	temp=float(int(temp.AtoF()*1000.0))/1000.0;

	while(temp[-4]!='.' && Length()>4) {
		temp=temp.ChopRt(1);
	}

	if(cString("56789").Contains(temp[-1])){
		temp=float(int(temp.AtoF()*100.0)+1)/100.0;
	} else {
		temp=temp.ChopRt(1);
	}

	if(temp.Length()==0 && Length()!=0) {
		 exit(1);
	}

	if(!temp.Contains('.')) temp+=".00";
	if(temp[-1]=='.') temp+="00";
	if(temp[-2]=='.') temp+='0';
	
	while(temp[-3]!='.' && Length()>3) {
		temp=temp.ChopRt(1);
	}
	if(temp=="") return "$0.00";
	return "$"+temp;
}

const int cString::SeekRt(char c) const {
	for (int i=-1; i+Length()!=0; i--) {
		if ((*this)[i]==c) return i;	
	}
	return -1;
}

const int cString::SeekLf(char c) const {
	for (int i=0; i<Length(); i++) {
		if (Buffer[i]==c) return i;	
	}
	return -1;
}

bool cString::ContainsDigit() {
	for(int i=0; i<Length(); i++) {
		if(isdigit(Buffer[i])) return true;
	}
	return false;
}

bool cString::IsNumber() {
	for(int i=0; i<Length(); i++) {
		if(!isdigit(Buffer[i])) return false;
	}
	return true;
}

int cString::ContainsXtimes(char n) {
	if(!Contains(n)) return 0;
	int counter=0;
	for(int i=0; i<Length(); i++) if(Buffer[i]==n) counter++;
	
	return counter;
}

cString cString::String2IntList() {
	cString input=*this;
	cStringList mylist;
	cString temp="";
	for(int i=0; i<input.Length(); i++) {
		mylist+=cString(int(input[i]));
	}
	mylist.ToString(temp," ");
	return temp;
}

cString cString::IntList2String() {
	cString input=*this;
	cStringList mylist;
	mylist.FromString(input," ");
	cString temp="";
	for(int i=0; i<mylist.Length(); i++) {
		temp+=char(mylist[i].AtoI());
	}
	return temp;
}

cString cString::CeasarEnCrypt(int slide) {
	cString input=*this;
	cString temp="";
	for(int i=0; i<input.Length(); i++) {
		temp+=char((input[i]+slide));
	}
	temp=temp.Reverse();
	return temp.String2IntList();
}

cString cString::CeasarDeCrypt(int slide) {
	cString input=*this;
	cString temp="";
	for(int i=0; i<input.Length(); i++) {
		temp+=char((input[i]-slide));
	}
	temp=temp.Reverse();
	return temp.IntList2String();
}

cString cString::EnCrypt(int slide) {
	cString input=*this;
	cString temp=input.String2IntList();
	return temp.CeasarEnCrypt(slide);
}

cString cString::DeCrypt(int slide) {
	cString input=*this;
	cString temp=input.IntList2String();
	return temp.CeasarDeCrypt(slide);
}
