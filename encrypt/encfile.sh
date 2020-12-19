#!/bin/sh

if ! [ -e "${HOME}/.p" ]; then
	echo "unable to find .p file" >&2
	exit 1
fi

if [ -z "$1" ]; then
	echo "missing file to encrypt" >&2
	exit 1
fi 

export PASS=`cat ${HOME}/.p`

FILENAME=`basename $1 .enc`

if ! [ -e "${FILENAME}" ]; then
	echo "file to encrypt: ${FILENAME} does not exist." >&2
	exit 1
fi

if [ -e "${FILENAME}.enc" ]; then
	echo "output file: ${FILENAME}.enc already exists." >&2
	exit 1
fi

# encryption
# from stackoverflow:
#openssl aes-256-cbc -a -salt -in secrets.txt -out secrets.txt.enc
echo "openssl aes-256-cbc -a -salt -k ${PASS} -in $FILENAME -out $FILENAME.enc "

#cat $1 | openssl aes-128-cbc -a -salt -k ${PASS} > $1.enc
