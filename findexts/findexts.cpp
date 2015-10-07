 //TBD
 
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <OpenStringLib.h>

void usage() {
	cout<<"USAGE: findexts generates a listing of extensions and how many there are of each, errors to stderr"<<endl;
}
void CreateFileList(cStringList &mylist) {
	cString cmd="find ./ -type f";
	FILE *t=popen(cmd, "r" );
	mylist.FromFile(t);
	pclose(t);
	
	
	for(int i=0; i<mylist.Length(); i++) {
		if(!mylist[i].ChopLf(1).Contains(".")) {
			
			mylist[i]="no ext";
		}
	}
	
	mylist.UCompact();
}

int main(int argc, char* argv[]) {
	cerr<<"December 22nd, 2013"<<endl;
	
	cArgs arguments(argc,argv,"-");
	
	
	bool csv=arguments.IsSet("csv");

	
	int thresh=arguments.GetArg("t",1).AtoI();
	cDualList endings;
	cStringList thelist;
	CreateFileList(thelist);
	int num=0;
	cString temp_ext="";
	cString filename="";
	
	for(int i=0; i<thelist.Length(); i++) {
		
		filename=thelist[i];
		if(filename.Contains('/')) filename=filename.ChopAllLf('/');
		
		if(filename.Contains('.')) temp_ext=filename.ChopAllLf('.');
		else temp_ext="[no ext]";
	
		num=endings[temp_ext].AtoI()+1;
		endings[temp_ext]=num;
		cerr<<"\rProgress: "<<(i+1)<<" / "<<thelist.Length()<<endl;
	}
	endings.Sort();
	endings.Compact();
	cerr<<"show listing of "<<endings.Length()<<" hits"<<endl;
	for(int i=0; i<endings.Length(); i++) {
		if(endings[i].AtoI()>thresh) {
			if(csv) cout<<endings.GetName(i)<<","<<endings[i]<<endl;
			else cout<<endings.GetName(i)<<" occurred "<<endings[i]<<" times "<<endl;
		}
	}
	return 0;
}
