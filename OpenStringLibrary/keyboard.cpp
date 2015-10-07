/***************************************************************
 * (C) Copyright 1998-2011 Chris Delezenski
 * This software is released under the LGPL v3, see COPYING and LGPL
 ***************************************************************/
#include "internal.h"

#ifndef WIN32
int cKeyboard::EscapeHandler() {
//	cerr<<"escaped"<<endl;
	char c=' ';
//	read(STDIN_FILENO,&c,1);
	//if( c==27) {
		read(STDIN_FILENO,&c,1);
		switch(c) {
			case 'A':
				return KEY_UP;	
			break;
			case 'B':
				return KEY_DOWN;
			break;
			case 'C':
				return KEY_RIGHT;
			break;
			case 'D':
				return KEY_LEFT;
			break;
			//case '1':
			//	return f5throughf8(debug);
			//break; 
			case '2':
				read(STDIN_FILENO,&c,1);
				return KEY_INS;
			break;
			case '3':
				read(STDIN_FILENO,&c,1);
				return KEY_DEL;
			break;	
			case '6':
				read(STDIN_FILENO,&c,1);
				return KEY_PDN;
			break;
			case '5':
				read(STDIN_FILENO,&c,1);
				return KEY_PUP;
			break;
			case 'F':
				return KEY_END;
			break;
			case 'H':
				return KEY_HOME;
			break;
			default:
				return KEY_UNDEF;
			break;
		} //end switch
	//} else {
	
//		return KEY_UNDEF;
//	}
	//} else {	
	/*	read(STDIN_FILENO,&c,1);
		switch(c) {
			case 'P':
				return "F1";
			break;
			case 'Q':
				return "F2";
			break;
			case 'R':
				return "F3";
			break;
			case 'S':
				return "F4";
			break;
			default:
				return "undefined";
			break;					
		} //end switch
	} //end else		*/
	return KEY_UNDEF;
} //end function

void cKeyboard::DrawGenericPrompt(cString msg, cString currentchoice, int length) {
	cString	lotsofspace="                ";
	while (lotsofspace.Length()<length) lotsofspace+=" ";
	//CMD 8-9-04: bug 170, high-light response
	cString doit="\r"+msg+" \033[1;33m"+currentchoice+"\033[0;39m"+lotsofspace+"\r";
	write(STDERR_FILENO, (char*)doit, doit.Length()); 
}

cString cKeyboard::ListPrompt(cString label, cString validchoices, cString defaultval, bool excessive) {
	cStringList myvalidchoices;
	myvalidchoices.FromString(validchoices, ",");
	return ListPrompt(label,myvalidchoices,defaultval,excessive);
}

cString cKeyboard::ListPrompt(cString label, cString validchoices, int defaultval, bool excessive) {
	cStringList myvalidchoices;
	myvalidchoices.FromString(validchoices, ",");
	return ListPrompt(label,myvalidchoices,defaultval,excessive);
}

cString cKeyboard::ListPrompt(cString label, cStringList &myvalidchoices, cString defaultval, bool excessive) {
	if(defaultval=="no-default-val" || defaultval=="")
		return ListPrompt(label,myvalidchoices,0,excessive);
	for(int i=0; i<myvalidchoices.Length(); i++)
		if(myvalidchoices[i]==defaultval)
			return ListPrompt(label,myvalidchoices,i,excessive);
	return ListPrompt(label,myvalidchoices,0,excessive);
}

cString cKeyboard::ListPrompt(cString label, cStringList &myvalidchoices, int defaultval, bool excessive) {
	int index=defaultval;
	if(index> myvalidchoices.Length()) index=myvalidchoices.Length();
	if(index<0) index=0;
	char c[3];
	c[0]='\0';
	c[1]='\0';
	c[2]='\0';	
	int n=0;
	//cerr<<label<<endl;
	//CMD 7-20-04: bug ref #137
	if(excessive) {
		cerr<<"Select a choice from the list using \033[1;33m<-\033[0;39m or \033[1;33m->\033[0;39m"<<endl;
		cerr<<"Press RETURN to confirm choice."<<endl;
	}
	bool leftpressed=false, rightpressed=false;
	
	int max=-1;
	for(int i=0; i<myvalidchoices.Length(); i++) if(max<myvalidchoices[i].Length()) max=myvalidchoices[i].Length();
	
	
	DrawGenericPrompt(label,myvalidchoices[index],max);
	cString temp="";
	int esc=0;
	while ( c[0] != '\n' ) {
		n=read(STDIN_FILENO,&c,2);
	
		if(c[0]==27 && n>1) {
			esc=EscapeHandler();
			if(esc==KEY_LEFT) leftpressed=true;
			else if(esc==KEY_RIGHT) rightpressed=true;
			else if(esc==KEY_UNDEF) {
				leftpressed=false;
				rightpressed=false;
			}
			esc=0;
		} else if(c[0]==27) {
			escapepressed=true;
			if(debug) cerr<<"pressed just escape!"<<endl;
			return myvalidchoices[defaultval];	
		}
		
		
		if(c[0]==',' || c[0]=='<' || leftpressed) {
			if(index>0) { index--; }
			else index=myvalidchoices.Length()-1;
		} else if (c[0]=='.' || c[0]=='>' || rightpressed) {
			if(index<myvalidchoices.Length()-1) index++;
			else index=0;
		}
		
		leftpressed=false; rightpressed=false;
		DrawGenericPrompt(label,myvalidchoices[index],max-myvalidchoices[index].Length());
	}
	cerr<<endl;
	return myvalidchoices[index];
}

void cKeyboard::Backspace(int fd, int num ) {
	for(int i=0; i<num; i++) {
		write(fd,"\b \b",3);
	}
}

void cKeyboard::f5throughf8() {
	char c=' ';
	//read(STDIN_FILENO,&c,1);
	switch(c) {
		case '5':
			if(debug) cerr<<"F5"<<endl;
			read(STDIN_FILENO,&c,1);
		break;
		case '7':
			if(debug) cerr<<"F6"<<endl;
			read(STDIN_FILENO,&c,1);
		break;
		case '8':
			if(debug) cerr<<"F7"<<endl;
			read(STDIN_FILENO,&c,1);
		break;
		case '9':
			if(debug) cerr<<"F8"<<endl;
			read(STDIN_FILENO,&c,1);
		break;
	}
}

cString cKeyboard::TextPrompt(cString label, cString defselect, bool password, bool canbreakout) {
	cString original=defselect;
	char c[3];
	c[0]='\0';
	c[1]='\0';
	c[2]='\0';
	int n=0;
	
	//		char c= ' ';
	write(STDERR_FILENO,(char*)label,label.Length());
	//CMD 7-27-04: bug #152, make sure text prompts can handle some symbols
	//CMD 8-2-04: bug #159, make sure text prompts can handle :[]{} and ;
	cString okchars=":;.-_,!@#$%^&*()+={} ~`[]'\"<>?|\\/";
	if(defselect!="") write(STDERR_FILENO,(char*)defselect,defselect.Length());
	while ( c[0] != '\n' ) {
		n=read(STDIN_FILENO,&c,2);
		//CMD 5-14-04: 127 is backspace!
		if(int(c[0])==127) { 
			//remove last char from password captured, unless the password Length is 0.
			if(defselect.Length()!=0) {
				Backspace(STDERR_FILENO, 1);
				
				defselect=defselect.ChopRt(defselect[-1]);
			}
		//CMD 7-27-04: bug #152, make sure text prompts can handle some symbols
		} else if( (isalnum(c[0]) || okchars.Contains(c[0])) && c[0]!='\n' ) { 
			defselect+=c[0];
			if(password) write(STDERR_FILENO,"*",1);
			else write(STDERR_FILENO,(void*)c,1);
		} else if( int(c[0])==27 && n>1) {
			EscapeHandler();
		} else if( int(c[0])==27 && canbreakout) {
			escapepressed=true;
			return original;
		}		
	}
	defselect=defselect.Trim();
	cerr<<endl;
	return defselect;
}

cString cKeyboard::Test() {
	char c[3];
	c[0]='\0';
	c[1]='\0';
	c[2]='\0';
	int n=0;

	cerr<<"press enter to finish"<<endl;
	cString defselect;
	while ( true  ) {
		n=read(STDIN_FILENO,&c,2);
		if(int(c[0])==127) { 
			//remove last char from password captured, unless the password Length is 0.
			if(defselect.Length()!=0) {
				Backspace(STDERR_FILENO, 1);
				
				defselect=defselect.ChopRt(defselect[-1]);
			}
		} else if( (isalnum(c[0]) || c[0]=='.') && c[0]!='\n' ) { 
			defselect+=c[0];
			write(STDERR_FILENO,(void*)c,1);
		} else if( int(c[0])==27 && n>1) {
			switch (EscapeHandler()) {
			
			
				case KEY_UP:
					cerr<<"Arrow Up"<<endl;
					break;
				case KEY_DOWN:
					cerr<<"Arrow Down"<<endl;
					break;
				case KEY_RIGHT:
					cerr<<"Arrow Right"<<endl;
					break;
				case KEY_LEFT:
					cerr<<"Arrow Left"<<endl;
					break;
				case KEY_INS:
					cerr<<"Insert Key"<<endl;
					break;
				case KEY_DEL:
					cerr<<"Delete Key"<<endl;
					break;
				case KEY_PDN:
					cerr<<"Page Down Key"<<endl;
					break;
				case KEY_PUP:
					cerr<<"Page Up Key"<<endl;
					break;
				case KEY_END:
					cerr<<"End Key"<<endl;
					break;
				case KEY_HOME:
					cerr<<"Home Key"<<endl;
					break;
				default:
					cerr<<"Undefined escape code"<<endl;
			} //end switch
		} else if( int(c[0])==27){
			cerr<<"pressed escape key!"<<endl;
		} else if(c[0]=='\n') {
			defselect=defselect.Trim();
			cerr<<endl;
			return defselect;
		} 
		c[0]='\0';
		c[1]='\0';
		c[2]='\0';
		n=0;
	}
	cerr<<"cKeyboard::Test() should never get here"<<endl;
	return "";
}


void cKeyboard::Slide_Indicator_Draw(cString label, int percent, char symbol) {

	cString bar="[....................]";
	int index=percent/5;
	if(index==0) index=1;
	else if(index==bar.Length()) index=19;
	
	bar[index]='|';
	cString doit="\r"+label+" "+bar+" "+cString(percent)+" "+cString(symbol)+" ";
	write(STDERR_FILENO,(char*)doit,doit.Length());
}

int cKeyboard::SlidePrompt(cString usingmsg, cString promptmsg, int currentpct, char symbol) { //, int lowend, int highend, int incrementor) {
	int original=currentpct;	
	cerr<<"Select the "<<usingmsg<<" using \033[1;33m<-\033[0;39m or \033[1;33m->\033[0;39m"<<endl;
	cerr<<"Press RETURN when finished"<<endl;
	cerr<<endl;
	Slide_Indicator_Draw(promptmsg, currentpct,symbol);
	int esc=0;
	bool leftpressed=false, rightpressed=false;

	char c[3];
	c[0]='\0';
	c[1]='\0';
	c[2]='\0';
	int n=0;



	while ( c[0] != '\n' ) {
		n=read(STDIN_FILENO,&c,2);
		if(c[0]==27 && n>1) {
			esc=EscapeHandler();
			if(esc==KEY_LEFT) leftpressed=true;
			else if(esc==KEY_RIGHT) rightpressed=true;
			else if(esc==KEY_UNDEF) {
				leftpressed=false;
				rightpressed=false;
			}
			esc=0;
		} else if (c[0]==27) {
			return original;
		}
			
		if(c[0]==',' || c[0]=='<' || leftpressed) {
			if(currentpct>0) currentpct-=10;	
		} else if (c[0]=='.' || c[0]=='>' || rightpressed) {
			if(currentpct<100) currentpct+=10;
		}
		
		if(currentpct<0) currentpct=0;
		if(currentpct>100) currentpct=100;
		
		leftpressed=false; rightpressed=false;
		Slide_Indicator_Draw(promptmsg, currentpct,symbol);
	}
	return currentpct;
}
#endif

