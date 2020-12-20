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

if [ "`uname`" == "Darwin" ]; then
	OPENSSL=/usr/local/bin/gopenssl
	if ! [ -e ${OPENSSL} ]; then
		echo "ERR: please install openssl via:" >&2
		echo " brew install openssl && (cd /usr/local/bin && sudo ln -s /usr/local/Cellar//openssl@1.1/1.1.1h/bin/openssl gopenssl) "
		exit 1
	fi
else
	OPENSSL=/usr/bin/openssl
	if ! [ -e /usr/bin/openssl ]; then
		echo "ERR: please install open ssl via:" >&2	
		echo " yum install -y openssl # RHEL/CentOS" >&2
		echo "   or"
		echo " apt-get install openssl # ubuntu / raspian" >&2
		exit 1
	fi
fi

# encryption
# from stackoverflow:
#openssl aes-256-cbc -a -salt -in secrets.txt -out secrets.txt.enc
echo "${OPENSSL} aes-256-cbc -a -salt -k ${PASS} -in $FILENAME -out $FILENAME.enc "

#cat $1 | openssl aes-128-cbc -a -salt -k ${PASS} > $1.enc
