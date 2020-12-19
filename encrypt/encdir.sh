#!/bin/sh

if ! [ -e "${HOME}/.p" ]; then
	echo "unable to find .p file" >&2
	exit 1
fi

if [ -z "$1" ]; then
	echo "missing file extension to archive" >&2
	exit 1
fi 

FILEEXT=$1

if [ -z "$2" ]; then
	echo "missing file to encrypt" >&2
	exit 1
fi 

export PASS=`cat ${HOME}/.p`

FILEEXT=$1
FILENAME=$2

echo "if [ -e "'${HOME}'"/.p ]; then"
echo " export PASS="'"-k `cat ${HOME}/.p`"'
echo "fi"
echo "export TEMP_DIR=`mktemp -d`/"
echo "export CWD=`pwd`"
echo "export FILENAME=$FILENAME"
echo "cp -avpf *.$FILEEXT "'$TEMP_DIR'
echo '(cd $TEMP_DIR && tar -zcvp .  > $CWD/$FILENAME )'
echo 'rm -vf $TEMP_DIR'
echo "openssl aes-256-cbc -a -salt "'${PASS}'" -in $FILENAME -out $FILENAME.enc "
echo "rm -vf "'${FILENAME}'

