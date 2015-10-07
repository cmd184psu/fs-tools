#!/bin/sh

ARCH=`arch`

if [ "$ARCH" = "x86_64" ]; then
	echo "lib64"
else
	echo "lib"
fi

