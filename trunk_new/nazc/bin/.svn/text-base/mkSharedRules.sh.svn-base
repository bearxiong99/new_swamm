#!/bin/bash

if [ "x$#" = "x" ]; then exit; fi

NZCTYPE=-1
CORE=-1         # ARM Core
ENDIAN=-1       # Endian
PTR64=-1        # 64bit pointer

while getopts "m:c:e:p:" Option
do
    case $Option in
        m   ) NZCTYPE=$OPTARG;;
        c   ) CORE=$OPTARG;;
        e   ) ENDIAN=$OPTARG;;
        p   ) PTR64=$OPTARG;;
    esac
done

shift $((OPTIND - 1))

while read line
do
    case "$line" in
        SUPPORT_NZC20* ) 
            if [ $NZCTYPE -ge 0 ] 
            then
                echo "SUPPORT_NZC20 = $NZCTYPE"
            else
                echo "$line"
            fi 
            ;;
        ARM* ) 
            if [ $CORE -ge 0 ] 
            then
                echo "ARM = $CORE"
            else
                echo "$line"
            fi 
            ;;
        BIG_ENDIAN* ) 
            if [ $ENDIAN -ge 0 ] 
            then
                echo "BIG_ENDIAN = $ENDIAN"
            else
                echo "$line"
            fi 
            ;;
        SUPPORT_64BIT_ADDRESS* ) 
            if [ $PTR64 -ge 0 ] 
            then
                echo "SUPPORT_64BIT_ADDRESS = $PTR64"
            else
                echo "$line"
            fi 
            ;;
        * ) echo "$line";;
    esac
done
