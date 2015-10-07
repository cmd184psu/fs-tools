/***************************************************************
 * (C) Copyright 1998-2011 Chris Delezenski
 * This software is released under the LGPL v3, see COPYING and LGPL
 ***************************************************************/
#ifndef __OPEN_STRING_LIB_H__
#define __OPEN_STRING_LIB_H__
using namespace std;
#include <fstream>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef WIN32
#include <termios.h>
#include <signal.h>
#endif
//forward declarations
class cString;
class cStringList;
class cVectorList;
class cDualList;

#ifndef MIN
#define MIN(x,y)     (((x) < (y)) ? (x) : (y))
#define MAX(x,y)     (((x) > (y)) ? (x) : (y))
#define MID(x,y,z)   MAX((x), MIN((y), (z)))
#endif
