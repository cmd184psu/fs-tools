/***************************************************************
 * (C) Copyright 1998-2011 Chris Delezenski
 * This software is released under the LGPL v2, see COPYING and LGPL
 ***************************************************************/
#include "internal.h"

// ********** Constructors ************ //
cVectorList::cVectorList( int Size ) {
	init();
	defaultindex=0;
	allocated = Size;
	used=0;
	Array=NULL;
	GetArray( );
	for(int i=0; i<allocated; i++) {
	    	Array[i].allocated=0;
		Array[i].used=0;
		Array[i].Array=NULL;
	}
}

const cVectorList & cVectorList::operator=( const cVectorList & Rhs ) {
	if( this != &Rhs ) {
		if(Rhs.used>allocated) Resize(Rhs.used);
		used=0;
		for( int i = 0; i < Rhs.used; i++ ) {
			used++;
			Array[used]=Rhs.Array[i];
		}
	}
	return *this;
}

// ****** Concatenation ***** //
void cVectorList::operator-=(const cStringList &Rhs) {
	cVectorList temp;
	temp+=Rhs;
	for(int i=0; i<Length(); i++)
		temp+=(*this)[i];
	(*this)=temp;
}

// ******** Accessors ******** //
const cStringList & cVectorList::operator[ ]( int Index ) const {
	if( abs(Index) > allocated) { cerr<<"index out of range"<<endl; exit(1); }
	if( Index <0 ) Index+=allocated;
	return Array[Index];
}

cStringList & cVectorList::operator[ ]( int Index ) {
	if( abs(Index) > Length()) { Index=0; }
	if( Index <0 ) Index+=used;
	return Array[Index];
}

int cVectorList::operator[ ]( cStringList & I ) const {
	for(int i=0; i<used; i++)
		if(cStringList(Array[i])==I) return i;
		return 0;
}

//cString operator[]( cString I ) const { return (Array[defaultindex])[I]; } 
//cString Efficiency() { return cString(used)+"/"+cString(allocated); }
//const int Length() const { return used; }

// ********** Modifiers **************** //
void cVectorList::Compact() {
	cVectorList temp(Length());
	int j=0;
	for(int i=0; i<used; i++) {
		if(!Checkforblankrow(i)) {
				temp[j]=(*this)[i];
				j++;
		}
	}
	(*this)=temp;
	used=j;
	if(j!=allocated) Resize(j);
}

void cVectorList::Sort(int n) {
	for (int i=0; i< Array[n].Length(); i++) {
		for(int j=0; j< Array[n].Length(); j++) {	
			if(Array[n][i]<Array[n][j] && i>j) {
				for(int k=0; k<used; k++) Array[k].Swap(i,j);
			}
		}
	}	
}

void cVectorList::Insert(int index,cStringList s) {
	cVectorList temp;
	for(int i=0; i<index; i++) temp+=(*this)[i];
	temp+=s;
	for(int j=index; j<Length(); j++) temp+=(*this)[j];
	(*this)=temp;
}

void cVectorList::Reverse() {
	cStringList temp;
	for(int i=0; i<Length(); i++)
		temp-=Array[i];
	*this=temp;
}

// ********** File IO ************ //
//Serialize cVectorlist into a 1 dimensional cStringList class
void cVectorList::ToList(cStringList &list,cString sep,int s, int ed) {
	cString newline;
	if(ed==-1) ed=used;
	int l=Array[0].Length();
	list.ClearAll();
	if(l==0) { return; }
	for(int j=0; j<l; j++){
		newline="";
		CreateRow(newline,j,sep,s,ed);
		//if(newline=="") { cerr<<"ERROR: new line is blank!"<<endl; }
		//else
		list+=newline;
	}
}

bool cVectorList::AddRow(cString &line,cString sep) {
	if(!line.Contains(sep))	return false;
	cStringList newrow;
	newrow.FromString(line,sep);
	if(!newrow.IsOptimum()) newrow.Compact();
	if(newrow.Length()==0) return false;
	AddRow(newrow,sep);
	return true;
}

void cVectorList::AddRow(cStringList & newrow,cString sep) {
	if(allocated<newrow.Length()) Resize(newrow.Length());
	used=newrow.Length();
	for(int i=0; i<newrow.Length(); i++) Array[i]+=(newrow[i].Trim());
}

void cVectorList::CreateRow(cString &newline,int thisline,cString sep, int s, int ed) {
	if(ed==-1) ed=used;
	for(int i=s; i<ed; i++) {
		if(thisline<Array[i].Length()) { 
			if(Array[i][thisline]!="") newline+=Array[i][thisline]; 
		}
		if(i<ed-1) newline+=sep;
	}
}

void cVectorList::GetArray( ) {
	if(allocated==0) {
		used=0;
		if(Array!=NULL) {  delete [] Array; }
		Array=NULL;
		return;
	} 
	Array = new cStringList [ allocated ];
}

void cVectorList::Resize( int NewSize ) {
	int orig_allocated=allocated;
	int orig_used=used;
	if(NewSize==0) { 
		if(Array!=NULL) {
			delete [] Array;
			Array=NULL;
		}
		used=0;
		allocated=0;
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
		cerr<<"vectorlist: WARNING: Array is not null and allocated is 0, this is a bad thing"<<endl;
		cerr<<"coming into the function, allocated = "<<orig_allocated<<" used="<<orig_used<<endl;
		cerr<<"want new size of "<<NewSize<<endl;
		exit(1);
	 }
	cStringList * OldArray;
	OldArray=Array;                    // Save array
	Array=NULL;				// Nullify old array pointer
	//    const int MinOfOldAndNew = MIN( allocated, NewSize );
	allocated=NewSize;
	GetArray();
	for( int i=0; i<used; i++) Array[i]=OldArray[i];
	delete [] OldArray;
	if(Array==NULL) { 
		cerr<<"Array is still NULL, terminate"<<endl;
		exit(1);
	}
}

cString cVectorList::NULLSTR="";
