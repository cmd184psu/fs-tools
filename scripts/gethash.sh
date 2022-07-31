#!/bin/sh

filename=$1

#echo "looking at file=$filename"

STAT=$(which gstat 2>/dev/null)
if [ -z "$STAT" ]; then
  STAT=$(which stat 2>/dev/null)
fi
filesize=$(${STAT} $filename | grep "Size: " | awk '{print $2}')

#gstat $filename

a=$filesize
b=1000000

MIN=$(( a < b ? a : b ))

#echo "filesize=$filesize"
#echo "MIN=$MIN"

COUNT=$(( $MIN / 1000 ))

if [ "$COUNT" -eq 1000 ]; then
    BS="bs=1M"
else
    BS=""
fi

CLI="dd if=$filename $BS count=$COUNT"

#echo $CLI

hash=$($CLI 2>/dev/null | md5sum | awk '{print $1}')

echo $hash
