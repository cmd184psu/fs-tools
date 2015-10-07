/***************************************************************
 * (C) Copyright 1998-2010 Chris Delezenski
 * This software is released under the LGPL v2, see COPYING and LGPL
 ***************************************************************/
#ifndef __CPROMPT__H
#define __CPROMPT__H


#define SS_INFD_DEFAULT 0
#define SS_OUTFD_DEFAULT 1
#define SS_STATUSFD_DEFAULT 2


class cPrompt {

	private:
		off_t transferred;
		off_t expected;       // 0 means unknown
		time_t start_time;
		time_t current_time;
	
		int percent;
		int width;
		int filled;
		int twiddler;      // -1 means don't draw a twiddler
		off_t twiddlenext;
		off_t twiddlecount;
		int hours;
		int minutes;
		int seconds;
		time_t lasttimerupdate;
		char linebuf[81];
		unsigned int linelen;
		bool updated;
		int bailon;
		int infd, outfd, statusfd;
//		void die(const char * const msg);
		void UpdateBarState();
	public:
		bool Update();
		cStreamStatus(int _expected, int _inputfd=SS_INFD_DEFAULT, int _outputfd=SS_OUTFD_DEFAULT, int _statusfd=SS_STATUSFD_DEFAULT) {
			transferred=0;
			bailon=0;
			start_time=time(NULL);
			if(!Init(_expected,_inputfd, _outputfd, _statusfd)) exit(1);
		}
		bool Init(int _expected, int _inputfd, int _outputfd, int _statusfd);
		unsigned long int Transfer();
		bool Write();
		size_t fWrite(const void *ptr, size_t size, size_t nmemb, FILE* stream);
		void BailOn(int _bytes) { bailon=_bytes; }
		
		
		static int ProgressBar(pid_t &pid,cString msg) {
			signal(SIGCHLD, SIG_DFL);
			int pfds[2];
			pipe(pfds);
			pid = fork();
			
			switch(pid) {
				case -1: cerr<<"unable to fork, die!"<<endl;
					return -1;
				break;
				case 0:
					close(0);
					dup(pfds[0]);
					close(pfds[1]);
					
					
					streamstatus(100);
					
					
					_exit(0);
				break;
				default: 
					close(pfds[0]);
					return pfds[1];
				break
			}
			return fd;
		}
};




#endif
