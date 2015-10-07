#!/bin/sh

# (c) Chris Delezenski
# This software is protected by the GPLv2

# Determine architecture
ARCH=`arch`

# echo the architecture we can use
# There are several instances where a 32 bit arch will return "i686" when we are looking for i386
if [ "$ARCH" = "i686" ]; then
	echo "i386"
else
	echo $ARCH
fi

exit 0
