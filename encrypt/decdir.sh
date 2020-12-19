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
echo "if ! openssl aes-256-cbc -d -a -salt "'${PASS}'" -in ../${DIR}.tgz.enc -out ${DIR}.tgz; then"
echo " echo error with decryption"
echo " exit 1"
echo "fi"
echo "tar -xvpf ${DIR}.tgz"
echo "rm -fv ${DIR}.tgz"
echo "cp -avpf "'*'" .."
echo "popd"
echo "rm -rf "'${TEMP_DIR}'
