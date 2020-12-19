#!/bin/sh

if [ -e /usr/local/bin/ ]; then
	INSTALLTO='/usr/local/bin'
else
	INSTALLTO='/usr/bin'
fi

install -m 755 encdir.sh $INSTALLTO/
install -m 755 decdir.sh $INSTALLTO/
install -m 755 encfile.sh $INSTALLTO/
install -m 755 decfile.sh $INSTALLTO/
