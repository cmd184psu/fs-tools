# 
# Makefile for destroyall
# written by Chris Delezenski
# GPL'ed (c) 2000-2011 Chris Delezenski
#-Wall -Werror
CFLAGS=-g -O2   $(INCLUDES) $(EXTRAINCLUDES) $(DEFS)
CPPFLAGS=$(CFLAGS)
INCLUDES=-I./ -I../ -I/usr/include
CC=gcc
VER=0.3
NAME=addprefix
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
	install $(PROGRAM) $(INSTALLDIR)
	#cp $(MAN) /usr/local/man/man5/
$(OBJ):

restore:
	$(RL) $(SRCS)
release:
	$(RLS) $(SRCS)

test: restore
	mkdir -p $(TESTDIR)
	cp $(SRCS) $(TESTDIR)
	cp Makefile $(TESTDIR)
	
#$(MAN): $(PROGRAM)
#	./$(PROGRAM) -man > $(MAN)
