#!/usr/bin/env bash

usage() {
	echo "./dircomp dir1 dir2"
	echo
	echo "Perform a very lightweight and dangerous analysis"
	echo "improperly comparing two directories "
	echo "without hashing each file, nor performing a diff"
}

function getFileSize() {
	ls -la "$@" | awk '{print $5}'
}
function inspectDir() {
	if ! [ -e $1 ]; then
		echo "Directory does not exist: $1" >&2
		exit 1
	fi
	#declare -a SORTED_FILE_LISTING ()
	#find $1 -iname "*" -type f | sort >&2
	readarray -t SORTED_FILE_LISTING < <( find $1 -iname "*" -type f | sort)
	echo "Length of directory (in terms of root files) is: ${#SORTED_FILE_LISTING[@]}"
	for((i=0; i<${#SORTED_FILE_LISTING[@]}; i++)); do
		fn=`echo ${SORTED_FILE_LISTING[$i]} | xargs`
		s=`getFileSize "${fn}"`
 		fn=`basename "$fn"`
		echo "i=$i file:::${fn} size:::$s"
	done	
}

while [[ $# -gt 0 ]]; do
    key="$1"
    case $key in
        usage|--usage)
            usage 
            exit 0
        ;;
		--source|source|src)
			SRC=$2
			shift;shift
		;;
		--target|target|tgt)
			TGT=$2
			shift;shift
		;;	
		*)
			echo "invalid flag: $1"
			exit 1
		;;
	esac
done

if [ -z "$SRC" ]; then
	echo "Missing source material"
	exit 1
fi

echo "looking at source folder: $SRC"
CONTENT_SRC=`inspectDir $SRC`
#printf "%s\n" $CONTENT_SRC

echo

if [ -z "$TGT" ]; then
	echo "Missing target material"
	exit 1
fi

echo "looking at target folder: $TGT"
CONTENT_TGT=`inspectDir $TGT`
#printf "%s\n" $CONTENT_TGT

HASH1=`echo $CONTENT_SRC | md5sum | awk '{print $1}'`
HASH2=`echo $CONTENT_TGT | md5sum | awk '{print $1}'`

echo "hash1=$HASH1"
echo "hash2=$HASH2"

if [ "$CONTENT_SRC" = "$CONTENT_TGT" ]; then
	echo "Directories are equal (probably)"
else
	echo "Directories are not equal"
fi	

echo "Complete"
