#!/bin/sh

if ! [ -e "${HOME}/.p" ]; then
	echo "unable to find .p file" >&2
	exit 1
fi

if [ -z "$1" ]; then
	echo "missing file to decrypt" >&2
	exit 1
fi 

export PASS=`cat ${HOME}/.p`

FILENAME=`basename $1 .enc`

if ! [ -e "${FILENAME}.enc" ]; then
	echo "encrypted file: ${FILENAME}.enc does not exist." >&2
	exit 1
fi

if [ -e "${FILENAME}" ]; then
	echo "target file: ${FILENAME} already exists." >&2
	exit 1
fi

#echo "openssl enc -aes256 -salt -k ${PASS} -in $FILENAME -out $FILENAME.enc "

#wrong
#echo "cat $FILENAME.enc | openssl aes-128-cbc -a -d -salt -k ${PASS} > $FILENAME"
#echo "openssl aes-128-cbc -a -d -salt -k ${PASS} -in $FILENAME.enc -out $FILENAME"
echo "openssl aes-256-cbc -d -a -salt -k ${PASS} -in ${FILENAME}.enc -out ${FILENAME}"