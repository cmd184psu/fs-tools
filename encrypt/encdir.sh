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

OPENSSL=/usr/local/bin/gopenssl
if [ "`uname`" == "Darwin" ]; then
	OPENSSL=/usr/local/bin/gopenssl
	if ! [ -e "${OPENSSL}" ]; then
		echo "ERR: please install openssl via:" >&2
		echo " brew install openssl && (cd /usr/local/bin && sudo ln -s /usr/local/Cellar//openssl@1.1/1.1.1h/bin/openssl gopenssl) " >&2
		exit 1
	fi
else
	OPENSSL=/usr/bin/openssl
 	if ! [ -e /usr/bin/openssl ]; then
 		echo "ERR: please install open ssl via:" >&2	
 		echo " yum install -y openssl # RHEL/CentOS" >&2
 		echo "   or" >&2
 		echo " apt-get install openssl # ubuntu / raspian" >&2
 		exit 1
	fi
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
echo "${OPENSSL} aes-256-cbc -a -salt "'${PASS}'" -in $FILENAME -out $FILENAME.enc "
echo "rm -vf "'${FILENAME}'

