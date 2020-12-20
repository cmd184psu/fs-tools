#!/bin/sh

if ! [ -e "${HOME}/.p" ]; then
	echo "unable to find .p file" >&2
	exit 1
fi

if [ -z "$1" ]; then
	echo "missing file to decrypt" >&2
	exit 1
fi 

FILENAME=$1

DIR=`basename $FILENAME .tgz.enc`

export PASS=`cat ${HOME}/.p`


if [ "`uname`" == "Darwin" ]; then
	OPENSSL=/usr/local/bin/gopenssl
	if ! [ -e ${OPENSSL} ]; then
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

echo "export TEMP_DIR=${DIR}/"
echo "export CWD=`pwd`"
echo "export FILENAME=$FILENAME"

echo "if [ -e "'${HOME}'"/.p ]; then"
echo " export PASS="'"-k `cat ${HOME}/.p`"'
echo "fi"
echo "mkdir -p "'${TEMP_DIR}'
echo "if ! [ -e "'${TEMP_DIR}'" ]; then"
echo " exit 1"
echo "fi"
echo "pushd "'${TEMP_DIR}'
echo "if ! ${OPENSSL} aes-256-cbc -d -a -salt "'${PASS}'" -in ${FILENAME} -out ${DIR}.tgz; then"
echo " echo error with decryption"
echo " exit 1"
echo "fi"
echo "tar -xvpf ${DIR}.tgz"
echo "rm -fv ${DIR}.tgz"
echo "popd"
