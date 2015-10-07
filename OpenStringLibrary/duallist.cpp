/***************************************************************
 * (C) Copyright 1998-2011 Chris Delezenski
 * This software is released under the LGPL v2, see COPYING and LGPL
 ***************************************************************/

#include "internal.h"
bool cDualList::setonlyifnotset=false;
// Low level store
void cDualList::dl_store(cString _name, cString _value) { 
	int pos=GetIndexFromName(_name);
	if(pos==-1) {
		AddCouple(_name,_value);
		return;
	}

	cString temp=(*this)[pos];
	if((setonlyifnotset && (*this)[_name]=="") || !setonlyifnotset) {
		(*this)[pos]=_value;
	}
}	



// ******* Accessors ***** //
//const cString& GetName(const int i) const { return Array[name][i]; } 
//cString GetValFromIndex(int i) { return Array[val][i]; }


// Search and acquire the index in the Array based on the name "i" provided
const int cDualList::GetIndexFromName(const cString& i) const  {
		for (int j=0; j<Length(); j++)
			if (Array[name][j]==i) return j;
		return -1;
}

int cDualList::GetIndexFromVal(cString i)  {
	for (int j=0; j<Length(); j++) 	if (Array[val][j]==i) return j;
	return -1;
}

//cString operator[ ]( int Index ) const { return Array[val][Index]; }
//cString & operator[ ]( int Index ) { return Array[val][Index]; }


// Just like an associative array - myarray["sometextualindex"] will return a string in the next column over
// if the element doesn't exist, the NULLSTR is returned.
// Important to note, NULLSTR is a solid cString() object and not actually a NULL pointer, so operations on NULLSTR such as Length() or concatentation
// 	are guarenteed to work without issue.
cString & cDualList::operator[ ]( cString n ) {
	int i=0;
	i=GetIndexFromName(n);
	if(i==-1) { AddCouple(n,""); return Array[val][-1]; } 
	 for(int i=0; i<Array[name].Length(); i++) {
		if(Array[name][i]==n) return Array[val][i];
		
	}
	return cVectorList::NULLSTR;
}

//cString both( int Index, cString sep=" = " ) {  return Array[name][Index]+" = "+Array[val][Index]; }
//cString Efficiency() { return Array[name].Efficiency(); }

// ******Modifiers****** //
void cDualList::Sort(int i) {
	 cVectorList::Sort(i);
}

//void SetSep(cString s) { sep=s; }


// Usually called right before writing to a file or 
//	perhaps after several deletes, this function will eliminate all the
//	names with blank values
void cDualList::Compact() {

//	if(disallowcompact) {
//		RemoveUnset();
//		return;
//	}
	for(int i=0; i<Length(); i++)
		if (Array[val][i]=="" || Array[name][i]=="") {
			Array[val][i]=""; Array[name][i]="";
		}
	Array[name].Compact();
	Array[val].Compact();
}

// ******* File I/O ******* //
// Inspired by Java, cDualList is capable of reducing down to a cStringList (1 dimension array) 


void cDualList::ToList(cStringList &list,cString sep) {
	cString newline;
	int l=Array[name].Length();
	list.ClearAll();
	if(l==0) {
//		cerr<<"ERROR: list contains no entries"<<endl;
		list.ClearAll();
		return;
	}
	for(int j=0; j<l; j++){
		newline="";
		CreateRow(newline,j,sep,0,2);
		if(newline=="") {
			list+="---new line---";
			list[-1]="";
		} else {
			list+=newline;
		}
	}
}

void cDualList::RemoveUnset() {
	for(int i=0; i<Length(); i++) {
		if(Array[val][i]=="<unset>") {
			Array[val][i]="";
			Array[name][i]="";
		}
	}
}

int cVectorList::allocated=2;
int cVectorList::used=2;

//Replace *this with the RightHandSide of the ='s operator
//	in practice, this looks like myarray=yourarray;
const cDualList & cDualList::operator=( const cDualList & Rhs ) {
	ClearAll();
	for(int i = 0; i < Rhs.Length(); i++) 
		AddCouple(Rhs.GetName(i),Rhs[i]);
		
	Compact();
	//dbfilename=Rhs.dbfilename;
	return *this;
}


// Do a quick and dirty search in the list and return a shorter list of matches
void cDualList::subset(cString search, cDualList &matches) {
	//chop that star off
	if(search[-1]=='*') search=search.ChopRt(1);
	for(int i=0; i<Length(); i++) {
		if((search.Length()==0 || GetName(i).Contains(search) )&& (*this)[i]!="<unset>") matches[GetName(i)]=(*this)[i];
		
	}
}

//Corner stone of a particular product, import from a file all the variables that match a particular wildcard
// optional: use "addition" to prepend text to the newly imported data
// example 1:
// original:
//	item1 = value1
//	item2 = value2
// imported file:
// 	item2 = pizza
//	item3 = value3
// would produce:
//	item1 = value1
//	item2 = pizza
//	item3 = value 3
//
// same example using "prepend" as the addition:
// would produce:
//	item1 = value1
//	item2 = value2
//	prepend.item2 = pizza
//	prepend.item3 = value 3

void cDualList::ImportFrom(cString fname,cString wildcard, cString addition) {
	cStringList import_stringlist;
	import_stringlist.FromFile(fname);
	cDualList importdb; 
	cDualList importss; //subset
	importdb.FromList(import_stringlist, " = ");
	importdb.Compact();
	importdb.subset(wildcard, importss);
	Mergewith(importss,addition);
}

//traverse mergeme and add to this all the elements
// ImportFrom() has almost identical functionality and makes use of this function for the "meat"
// leaving the file i/o out, as we may want to handle things without files, programmatically
void cDualList::Mergewith(cDualList &mergeme, cString addition) {
	if(addition!="" && addition[-1]!='.') addition+=".";
	for(int i=0; i<mergeme.Length(); i++) (*this)[addition+mergeme.GetName(i)]=mergeme[i];
}
	
