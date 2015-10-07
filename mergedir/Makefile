# 
# Makefile for mergedir
# written by Chris Delezenski
# GPL'ed (c) 2000-2011 Chris Delezenski

CFLAGS=-g -O2 -Wall  $(INCLUDES) $(EXTRAINCLUDES) $(DEFS)
CPPFLAGS=$(CFLAGS)
INCLUDES=-I./ -I../ -I/usr/include
CC=gcc
VER=0.3
NAME=mergedir
OTHERLIBES=
LIBES= -lstdc++ -lOpenString
LFLAGS= -L../ -L/usr/lib
INSTALLDIR=/usr/bin/
TESTDIR=../TEST/$(NAME)
COMPRESSED=/usr/src/redhat/SOURCES/$(NAME)-$(VER).tgz
RL=/usr/local/bin/rstlink
RLS=/usr/local/bin/rstlink -release
PROGRAM=$(NAME)
OBJS=$(NAME).o
SRCS=$(NAME).cpp
MAN=$(NAME).5

all: $(PROGRAM)
#$(MAN)

$(PROGRAM): $(OBJS)
	$(CC) $(LFLAGS) $(CFLAGS) -o $(PROGRAM) $(OBJS) $(LIBES)
	
clean: 
	rm -rf $(OBJS) core *.*~ *.bak $(PROGRAM) $(TESTDIR) $(MAN)

install: all
	strip $(PROGRAM)
	install  $(PROGRAM) $(INSTALLDIR)
	#cp $(MAN) /usr/local/man/man5/
$(OBJ):

restore:
	$(RL) $(SRCS)
release:
	$(RLS) $(SRCS)

test: 
	mkdir My\ Documents My\ Scans My\ Documents/subdir1 My\ Scans/subdir1
	touch My\ Documents/file1.txt
	touch My\ Documents/file2.txt
	touch My\ Documents/file3.txt
	touch My\ Scans/file1.txt
	touch My\ Scans/file4.txt
	touch My\ Scans/subdir1/whatever.txt
	touch My\ Documents/subdir1/whatsup.txt
	
#$(MAN): $(PROGRAM)
#	./$(PROGRAM) -man > $(MAN)
