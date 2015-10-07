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
