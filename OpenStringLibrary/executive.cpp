/***************************************************************
 * (C) Copyright 1998-2011 Chris Delezenski
 * This software is released under the LGPL v2, see COPYING and LGPL
 ***************************************************************/

#include "internal.h"
#ifndef WIN32

void cExec::Show(ostream &fout) {
	if(useenvironment && environment.Length()>0) {
		for(int i=0; i<environment.Length(); i++) 
			fout<<"env#"<<i<<":"<<environment[i]<<endl;
	} 

	for(int j=0; j<argvalues.Length(); j++) 	
		fout<<"arg#"<<j<<":"<<argvalues[j]<<endl;
}


//Intent: be sure to fork before you exec
//  this function does not return
//unless:
// - in debug mode
// - the exec function fails
// - environment.Length() is 0 or argvalues.Length()==0 or path is NULL.
// - Or "usefork" is set to true
bool cExec::Exec() {
	if(debug) {
		Show(cerr);
		return true;
	}
	
	char **env;
	char **argv;
//	int argc=argvalues.Length();
	
	//Make the path also be the 0th argument, if not set
	if(argvalues.Length()!=0 && path=="") path=argvalues[0];
	
	//conversely, if the path exists, but there are no arguments, make the path the 0th argument
	if(path!="" && argvalues.Length()==0) argvalues+=path;

	//Trap the error condition
	if(argvalues.Length()==0 && path=="") {
		if(Verbose()) {
			cerr<<"cExec::Exec(): path is null"<<endl;
			cerr<<"cExec::Exec(): argvalues length is 0"<<endl;
		}
		Output("cExec::Exec(): returning early because path is NULL and argvalues length is 0");
		return false;
	}

	if(environment.Length()!=0) env=environment.ToCharList();
	else env=NULL;
	
	//Convert from cStringList to char** for the low-level C function EXEC(3)
	argv=argvalues.ToCharList();
	
	
	//Execute with a predefined enviroment
	if(useenvironment) {
		///cout<<"use the environment!"<<endl;
		if(environment.Length()==0 || env==NULL) {
			Output("environment list is empty or env is NULL and we expected to use environment");
			return false;
		}
		execve(path,argv,env);
	} else {
		//execute without a specified path - Assume we will inherit the current environment
		execv(path,argv);
	}

	//only possibility is that exec returned -1
	cString cmdline;
	argvalues.ToString(cmdline," ");
	
	//execve and execv never return
	Output("cExec::Exec("+cmdline+") failed with error code="+cString(strerror(errno)));

	return false;
}

//populate the exec structure from a command line string
bool cExec::FromCommandline( cString cmdline ) {
	if(cmdline=="") {
		if(Verbose()) cerr<<"cExec::FromCommandline(): command line was blank"<<endl;
		return false;
	}

	//Chop the string into argvalues using space as a delimiter
	argvalues.FromStringWithSpaces(cmdline);
	if(argvalues.Length()==0) {
		cerr<<"cExec::FromCommandline(): argvalue length is 0, command line was not read correctly"<<endl;
		return false;
	}	
	path=argvalues[0];
	return true;
}

//Execute the command in the foreground and wait for it
//  Once done, return the status in two ways: status code passed back by reference and true if status is 0
bool cExec::ForkExecFG(cString cmdline, int &status) {
	cString msg="cExec::ForkExecFG("+cmdline.Limit(30,"...")+") ";
	pid_t pid=fork();
	
	switch(pid) {
		case -1:
			Output("cExec::ForkExecFG - Unable to fork");
			return false;
		break;
		case 0:
			argvalues.ClearAll();
			FromCommandline(cmdline);
			
			if(environment.Length()!=0) {
				useenvironment=true;
			}
			
			Exec();
			Output("cExec::ForkExecFG, after calling Exec:: shouldnt get here, exec failed");
			exit(1);
		break;
	}
	

	bool r=WaitForExit(msg,status,pid);
	return r;
}

//Execute the command in the background, use signal handling to catch the children signals (sigchld)
bool cExec::ForkExecBG(cString cmdline) {
	cString msg="cExec::ForkExecBG("+cmdline.Limit(30,"...")+") ";
	pid_t pid=fork();
	
	switch(pid) {
		case -1:
			Output(msg+" - Unable to fork");
			return false;
		break;
		case 0:
			argvalues.ClearAll();
			FromCommandline(cmdline);
			
			if(environment.Length()!=0) {
				useenvironment=true;
			}
			
			Exec();
			Output(msg+" after calling Exec:: shouldnt get here, exec failed");
			exit(1);
		break;
	}
	
	successful_forks++;
	InstallSignal((*this));
//	bool r=WaitForExit(pid);
	return true;
}

//Execute the command in the foreground, but use pipes to capture the output
bool cExec::ForkExecCaptureFG(cString cmdline,cStringList &thelist, int &status) {
	cString msg="cExec::ForkExecCaptureFG("+cmdline.Limit(30,"...")+") ";
	
	
	int pipe_in[2],pipe_out[2];

	if(pipe(pipe_in)==-1) return -1;
	if(pipe(pipe_out)==-1) return -1;
	
	pid_t pid=fork();
	
	switch(pid) {
		case -1:
			Output("cExec::ForkExecCaptureFG - Unable to fork");
			return false;
		break;
		case 0:
		
			close(pipe_in[0]);
			close(pipe_out[1]);
			
			if(dup2(pipe_in[1], 1) == -1 ) exit(1);
			if(dup2(pipe_out[0], 0) == -1 ) exit(1);
		
		
			argvalues.ClearAll();
			FromCommandline(cmdline);
			
			if(environment.Length()!=0) {
				useenvironment=true;
			}
			
			Exec();
			Output("cExec::ForkExecCaptureFG, after calling Exec:: shouldnt get here, exec failed");
			exit(1);
		break;
	}
	
	close(pipe_in[1]);
	close(pipe_out[0]);
	//pipeout=pipe_in[0];
	//pipein=pipe_out[1];
//	signal(SIGCHLD,child_handler);
	
	FILE* fptr=fdopen(pipe_in[0],"r");
	thelist.FromFile(fptr);

	
	
	//close(pipe_in[0]);
	fclose(fptr);
	close(pipe_out[1]);
	return WaitForExit(msg,status,pid);
}

//Execute the command in the foreground, capture the output, but eliminate all output except what for which we're looking
//	This is akin to:
//		command line | grep something
bool cExec::ForkExecCaptureFGGrep(cString cmdline,cStringList &thelist, cString grepfor, int &status) {
	cExec myexec;
	bool r=myexec.ForkExecCaptureFG(cmdline,thelist, status);
	if(!r) return false;
	for(int i=0; i<thelist.Length(); i++) if(!thelist[i].Contains(grepfor)) thelist[i]="";
	thelist.Compact();
	return thelist.Length()>0 && r;
}

// Wait for a specific exit status
bool cExec::WaitForExit(cString _msg, int &status, pid_t& pid) {
	cString msg=_msg;
	status=0;
	do {
		//Output(msg+" - [parent] about to run waitpid("+cString(pid)+")");
		pid = waitpid(pid, &status, WUNTRACED | WCONTINUED);
		if (pid == -1) { 
			Output(msg+" waitpid failed with error "+cString(strerror(errno)));
			status=-42;
		}
		if (WIFEXITED(status)) {
			//Output(msg+" exited, status="+cString(WEXITSTATUS(status)));
		} else if (WIFSIGNALED(status)) {
			Output(msg+" killed by signal "+cString(WTERMSIG(status)));
		} else if (WIFSTOPPED(status)) {
			Output(msg+" stopped by signal "+cString(WSTOPSIG(status)));
		} else if (WIFCONTINUED(status)) {
			Output(msg+" continued");
		}
	} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	return status==0;
}
bool cExec::WaitForExit(cString _msg) {
	int status=0;
	pid_t pid;
	return WaitForExit(_msg,status,pid);
}

void cExec::child_handler(int signum) {
	pid_t pid=-1;
	int status=0;
	WaitForExit("child_handler",status,pid);
}

//This will be like "pipe to command", with a pipe to insert data to the child
pid_t cExec::DualPipeToCommand(const cStringList &args, int & pipein, int & pipeout) {
	int pipeset_input[2];
	int pipeset_output[2];

	//write output from parent to pipe
	//read input in child from stdin
	if(pipe(pipeset_input) == -1) {
		Output("pipe call error");
		return -1;
	}
	//added
	if(pipe(pipeset_output) == -1) {
		Output("pipe call error");
		return -1;
	}
	
	cString getme;
	
	int pid=fork();
	switch(pid) {
		case -1:
	//		if(Verbose()) {
	//			 Output("fork failed "+<<endl;
	//			 perror("fork");
	//		}
			return -1;
		break;
		case 0:
			// if child then write down pipe 
			close(pipeset_input[0]);  // first close the read end of the pipe 
			close(pipeset_output[1]); // first close the write end of the output pipe
			
			if(dup2(pipeset_input[1], 1) == -1 ) {// stdout == write end of the pipe 
				//if(Verbose()) cerr<<"dup2 failed"<<endl;
				exit(1);
			}
			if(dup2(pipeset_output[0], 0) == -1 ) {// stdin == read end of the pipe 
				//if(Verbose()) cerr<<"dup2 failed"<<endl;
				exit(1);
			}
			
			cExec execme;
			execme.argvalues=args;
			execme.Exec();
			exit(1);
	         break;
	}
	// parent reads pipe 
	close(pipeset_input[1]);  // first close the write end of the pipe 
	close(pipeset_output[0]); // first close teh read end of the output pipe
	pipeout=pipeset_input[0]; //read from the output of the program (child process or cmd)
	pipein=pipeset_output[1]; //write to the input of the program (child process or cmd)
	return pid;
}

//This function is an alternative to popen(3) and serves as merely a wrapper for DualPipeToCommand()
//  Use this function when trying to avoid executing a shell (/bin/sh), as popen is known for doing this
//  Use popen(3) instead if using pipes, redirection or backgrounding.  ( <>, |, & )
//	These options normally found in shell are actually functions of the shell and require a great deal more code.. or use of /bin/sh

pid_t cExec::PopenAlt(cStringList &args, int &pipeout) {
	int pipein=0;
	pid_t ret=DualPipeToCommand(args,pipein,pipeout);
	close(pipein);
	return ret;
}

//There are several situations where we only want the very first line of some particular output
cString cExec::GetFirstLine_PopenAlt(cStringList &args,int &status) {
	cString thefirstline;
	int pipeout=0;
	pid_t pid=PopenAlt(args,pipeout);
	
	thefirstline.GetLine(pipeout);
	close(pipeout);
	waitpid(pid,&status,0);
	
	return thefirstline;
}

//A convenient wrapper for calling the shell.  Useful for debugging automated programs
bool cExec::ShellOut(cString shell, cString warning) {	
	pid_t bashpid=fork();
	switch(bashpid) {
		case -1:
			cerr<<"Unable to fork"<<endl;
			return false;
		break;
		case 0:
			cExec exec;
			if(warning!="") cerr<<warning<<endl;
			exec.FromCommandline(shell);
			exec.Exec();
			cerr<<"failed to exec"<<endl;
			exit(1);
		break;
	}
	int status=0;
	waitpid(bashpid,&status,0);
	cConsolePrompt::PressEnter(true);
	return status==0;
}


//Open a pipe to a command and feed data to it		
int cExec::OpenPipeTo(cString cmdline, pid_t &pid) {
//	InstallSignal((*this));
	signal(SIGCHLD, cExec::child_handler);
   
        int pfds[2];

        pipe(pfds);

	pid=fork();
	
	cExec execute;
	switch(pid) {
		case -1:
			cerr<<"unable to fork, die!"<<endl;
			return -1;
		break;
		
		case 0:
			execute.FromCommandline(cmdline);
			close(0);       //close normal stdin
			dup(pfds[0]);   //make stdin same as pfds[0]
		
			//close the other end  
			close(pfds[1]); //we don't need this
			
			if(!execute.Exec()) {
				cerr<<"Failed to exec "<<cmdline<<endl;
			}
			_exit(2);
		break;
		default: 

			//don't close stdout, b/c we're not going to use it
			//close(1);       // close normal stdout
			
			//not sure we need to dup this
			//dup(pfds[1]);   //make stdout same as pfds[1]
			close(pfds[0]); //we don't need this
			
			return pfds[1];
		break;
	}
	return -1;
}

//Using CHROOT(2), execute a program in a particular directory, using another directory as the new "root"
//	This function mirrors the UNIX chroot command	
bool cExec::ChrootJail(cString directory, cString chtodir, cString cmdline) {
	pid_t pid=fork();
	cExec executable;
	if(pid==0) {
		//only the child gets jailed
		chroot(directory);
		
		if(chtodir!="") if(chdir(chtodir)!=0) exit(1); //terminate child if we can't chdir to the requested directory
		executable.FromCommandline(cmdline);
		if(!executable.Exec()) _exit(1); //terminate child, if process failed, return 1
		_exit(0); //terminate child, should not get here
	}
	int status=-1;
	waitpid(pid,&status,0);

	return status==0;
}
#endif
