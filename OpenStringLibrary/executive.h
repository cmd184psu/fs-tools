/***************************************************************
 * (C) Copyright 1998-2011 Chris Delezenski
 * This software is released under the LGPL v2, see COPYING and LGPL
 ***************************************************************/
#ifndef __EXECUTIVE__H
#define __EXECUTIVE__H


// Unfortunately, this class is not currently available for Windows
#ifndef WIN32
#include <syslog.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#ifdef OSX
#include <signal.h>
#endif


#include <fcntl.h>
#define TWIDDLE_DEFAULT cString("|/-\\|/-\\")
#define MSG_DEFAULT cString("default:wait")
/**********************************************************************************************************/
// Executive Class
// (c) Chris Delezenski <chris.delezenski@gmail.com>
// 
// This class wraps the execution of programs making use of pipe(2), exec(3), popen(3), and system(3)
// The goal is to provide an api where "should be easy" operations involving external programs 
// can be manipulated and interacted with.
// Secondary goal is to eliminate the need for system(3) and popen(3) as these functions are known for 
// security deficiencies, since they both rely on /bin/sh
/*************************************************************************************************************/
class cStringList;
class cExec {

	protected:
		bool useenvironment;
		cString path;
		bool debug;
		bool usefork;
		bool verbose;
		
		static void child_handler(int signum);
		int successful_forks;
	//	static cDualList pid_status;
		static void Output(cString _msg) {
			#ifndef WIN32
			syslog(LOG_INFO,"%s",(char*)(cString(getpid())+" : "+_msg));
			#endif
			cerr<<cString(getpid())<<" : "<<_msg<<endl;
		}
	public:
		//Program arguments
		cStringList argvalues;
		
		//Environment for the program.  Used for forcing our own environment on a new process.
		// Typically this list contains elements such as HOME=/home/cmd
		cStringList environment;

		
		//execute the list if not debug
		//otherwise just show it
		bool Exec();
		void SetDebug() { debug=true; }
		cExec() { useenvironment=false; debug=false; path=""; usefork=false; verbose=false; }
		void UseEnvironment() { useenvironment=true; }

		bool IsDebug() { return debug; }
		bool IsEnvironment() {return useenvironment; } 
		
		
		void Show(ostream &fout);
		
		
		//The path for the program is the program itself
		//There may be situations where we'd want to execute a particular program but use a different path
		//    Calling the same program with a different 0th argument can change the behavior of the program
		//    A good example of this would be /usr/bin/reboot (symlink to consolehelper)
		void SetPath(const cString &_path) { path=_path; }
		
		
		
		void Show() { Show(cout); }
		bool FromCommandline(cString cmdline);
		bool Verbose() { return verbose; }
		void SetVerbose(bool _verbose) { verbose=_verbose; }
		
		//void SetCallingProgram(cString _msg) { msg=_msg; }
		
		//Some programs don't behave correctly in a straight fork/exec and it is necessary to force system(3) to be used anyway
		static bool System(cString cmdline, bool forcesystem3=false) {
			if(cmdline.ContainsAnyOf("<>|&*")) forcesystem3=true;
			if(forcesystem3) return system(cmdline)==0;
			cExec myexec;
			return myexec.ForkExecFG(cmdline);
		}
		
		
		bool ForkExecFG(cString cmdline, int &status);
		bool ForkExecFG(cString cmdline) { int status=-42; return ForkExecFG(cmdline,status); }
		bool ForkExecBG(cString cmdline);

		bool ForkExecCaptureFG(cString cmdline, cStringList &output, int &status);
		static bool ForkExecCaptureFGGrep(cString cmdline,cStringList &thelist, cString grepfor) {
			int status=-42;
			bool r=cExec::ForkExecCaptureFGGrep(cmdline,thelist,grepfor,status);
			return r;
		}
		static bool ForkExecCaptureFGGrep(cString cmdline,cStringList &thelist, cString grepfor, int &status);
		
		int NumOfForks() { return successful_forks; }
	
		//int NumberOfBackgroundedProcesses() { return pid_status.Length()-successful_forks; }
		//int GetExitCode(pid_t index_pid) { return pid_status[cString(index_pid)].AtoI(); }
		//int operator[](pid_t index_pid) { return  GetExitCode(index_pid); }
		static void InstallSignal(cExec &_exe) {
			signal((int)SIGCHLD,_exe.child_handler);
		}
		bool WaitForExit(cString _msg);
		static bool WaitForExit(cString _msg, int &status, pid_t& pid);
		
		
		//The following functions are pipe manipulations
		//  More information is provided in the .cpp file
		static int OpenPipeTo(cString cmdline, pid_t &pid);

		static pid_t DualPipeToCommand(const char * cmd, int & pipein, int & pipeout) {
			cExec myexe;
			myexe.FromCommandline(cmd);
			return DualPipeToCommand(myexe.argvalues,pipein,pipeout);
		}
		static pid_t DualPipeToCommand(const cStringList &args, int & pipein, int & pipeout);

		static cString GetFirstLine_PopenAlt(cString cmdline, int &retval) {
			cExec myexe;
			myexe.FromCommandline(cmdline);
			return GetFirstLine_PopenAlt(myexe.argvalues,retval);
		}
		static cString GetFirstLine_PopenAlt(cStringList &args, int &retval);

		static pid_t PopenAlt(cString cmdline, int &pipeout) {
			cExec myexe;
			myexe.FromCommandline(cmdline);
			return PopenAlt(myexe.argvalues,pipeout);
		}
		static pid_t PopenAlt(cStringList &args, int &pipeout);
		
		
		// An example of a very low-level copy
		static bool CopyFile(cString src, cString target) {
			int inF, ouF;
			char line[512];
			int bytes;
			if((inF = open(src, O_RDONLY)) == -1) {
				perror("open");
				return false;
			}
			
			if((ouF = open(target, O_WRONLY | O_CREAT)) == -1) {
				perror("open");
				return false;
			}
			
			while((bytes = read(inF, line, sizeof(line))) > 0)
				write(ouF, line, bytes);
				
			close(inF);
			close(ouF);
			return true;
		}
		static bool ShellOut(cString shell, cString warning="");
		static bool BashOut() {
			return ShellOut("/bin/bash");
		}
		
		static bool SudoBashOut() {
			return ShellOut("/usr/bin/sudo /bin/bash");
		}

		static void WhileWaiting(ostream & wout, cString msg, cString twiddle, pid_t pid) {
			while(true) {
				for(int i=0; i<twiddle.Length(); i++) {
					wout<<msg<<" "<<twiddle[i]<<flush;
					usleep(300);
					wout<<"\r\b"<<flush;
				}
			}
		}
		
		//Exec a program in the background and do something interesting while we wait for that process to complete, such as create a "twiddle"
		static bool ExecWhileWaiting3(cString cmdline, cString msg=MSG_DEFAULT, cString twiddle=TWIDDLE_DEFAULT) {
			pid_t pid=fork();
			switch(pid) {
				case -1:
					cerr<<"fork error"<<endl;
					exit(1);
				break;
				case 0:
					//close(2);
					//close(1);
					//cExec execme;
					//execme.FromCommandline(cmdline);
					//execme.Exec();
					//_exit(1);
					cExec::System(cmdline,true);
					_exit(0);
				break;
			}
			pid_t pid2=fork();
			switch(pid2) {
				case -1:
					cerr<<"fork error"<<endl;
					exit(1);
				break;
				case 0:
					if(msg=="default:wait") msg="waiting for child ("+cString(pid)+")";
					WhileWaiting(cout,msg,twiddle,pid);
					_exit(0);
				break;
			}
			
			int status=-255;
			waitpid(pid,&status,0);
			//	cerr<<"child died, kill my twiddle"<<endl<<"kill("<<pid2<<",9)"<<endl;
			
			kill(pid2,9);
			waitpid(pid2,NULL,0);
			return !(status==128 || status==16 || status==8);
		}

		
		static bool ExecWhileWaiting2(cString cmdline, cString msg=MSG_DEFAULT, cString twiddle=TWIDDLE_DEFAULT) {
			pid_t pid=fork();
			switch(pid) {
				case -1:
					cerr<<"fork error"<<endl;
					exit(1);
				break;
				case 0:
					close(2);
					close(1);
					cExec execme;
					execme.FromCommandline(cmdline);
					execme.Exec();
					_exit(1);
				break;
			}
			pid_t pid2=fork();
			switch(pid2) {
				case -1:
					cerr<<"fork error"<<endl;
					exit(1);
				break;
				case 0:
					if(msg=="default:wait") msg="waiting for child ("+cString(pid)+")";
					WhileWaiting(cout,msg,twiddle,pid);
					_exit(0);
				break;
			}
			
			int status=-255;
			waitpid(pid,&status,0);
			//	cerr<<"child died, kill my twiddle"<<endl<<"kill("<<pid2<<",9)"<<endl;
			
			kill(pid2,9);
			waitpid(pid2,NULL,0);
			return !(status==128 || status==16 || status==8);
		}


		static bool ExecWhileWaiting(cString cmdline, cString msg=MSG_DEFAULT, cString twiddle=TWIDDLE_DEFAULT) {
			pid_t pid=fork();
			switch(pid) {
				case -1:
					cerr<<"fork error"<<endl;
					exit(1);
				break;
				case 0:
					close(2);
					close(1);
					if(cmdline.StartsWith("/bin/cp ")) sleep(1);
					cExec execme;
					execme.FromCommandline(cmdline);
					execme.Exec();
					_exit(1);
				break;
			}
			pid_t pid2=fork();
			switch(pid2) {
				case -1:
					cerr<<"fork error"<<endl;
					exit(1);
				break;
				case 0:
					if(msg=="default:wait") msg="waiting for child ("+cString(pid)+")";
		
					WhileWaiting(cout,msg,twiddle,pid);
					_exit(0);
				break;
			}
			int status=-255;
			waitpid(pid,&status,0);
			//	cerr<<"child died, kill my twiddle"<<endl<<"kill("<<pid2<<",9)"<<endl;
			kill(pid2,9);
			waitpid(pid2,NULL,0);
			return (status==0);
		}
		static bool ChrootJail(cString directory, cString cmdline) { return ChrootJail(directory,"",cmdline); }
		static bool ChrootJail(cString directory, cString chtodir, cString cmdline);

};
#else
class cStringList;
class cExec {
	public:
		static bool ForkExecFG(cString cmdline, int &status) {
			status=system(cmdline);
			return status==0;
		}
		static bool ForkExecBG(cString cmdline, int &status) {
			cmdline+="&";
			status=system(cmdline);
			return status==0;
		}
		static bool ForkExecCaptureFG(cString cmdline, cStringList &output, int &status) {
			output.FromPopen(cmdline);
			status=0;
			return true;
		}
		

};
#endif
#endif
