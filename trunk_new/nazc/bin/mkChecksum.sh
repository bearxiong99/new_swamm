#!/bin/bash

if [ "x$#" = "x" ]; then exit; fi

MD5SUM=`which md5sum`

#xargs "$*" | ${MD5SUM} "$*"

while read line 
do
    find $line -type f -exec ${MD5SUM} {} \; 2> /dev/null | grep -v .svn | sed -e 's/\*//g' 
done 

