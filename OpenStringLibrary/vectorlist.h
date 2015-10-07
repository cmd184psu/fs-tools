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








