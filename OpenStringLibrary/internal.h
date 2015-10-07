/***************************************************************
 * (C) Copyright 1998-2011 Chris Delezenski
 * This software is released under the LGPL v3, see COPYING and LGPL
 ***************************************************************/
#ifndef __OSL___INTERNAL_____H
#define __OSL___INTERNAL_____H

using namespace std;

#include "config.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <iostream> 
#include <fstream>
#include <string>
#include <unistd.h>
#include <iostream>
#include <unistd.h>
#include <time.h>
#include <utime.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/stat.h>


#ifndef WIN32
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <termios.h>
#include <syslog.h>
#include <errno.h>
#endif 

#ifndef MIN
#define MIN(x,y)     (((x) < (y)) ? (x) : (y))
#define MAX(x,y)     (((x) > (y)) ? (x) : (y))
#define MID(x,y,z)   MAX((x), MIN((y), (z)))
#endif


#include "dynstring.h"
#include "stringlist.h"
#include "vectorlist.h"
#include "duallist.h"

#ifndef WIN32
#include "fileunit.h"
#include "filelist.h"
#include "keyboard.h"
#include "timeanddate.h"
#endif 

#include "executive.h"
#include <stdlib.h>
#include "phptranslation.h"
#endif
