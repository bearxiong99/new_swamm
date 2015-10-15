#!/bin/bash

function usage()
{
    echo "$0 ipaddr difffile script"
    exit 0
}

function errmsg()
{
    echo "Error: $1"
}

if [ $# -lt 2 ]; then usage; fi

PARAMCNT=$#
IPADDR=$1
DIFFFILE=$2

if [ ! -e $DIFFFILE ]
then
    errmsg "Invalid file [${DIFFFILE}]"
    usage
fi

DIRNAME="${0%/*}/"
PUTFILE="./putFile.exe"
EXECMD="./executeShell.exe"

if [ -f "${DIRNAME}putFile.exe" ]
then
    PUTFILE=${DIRNAME}putFile.exe
fi
if [ -f "${DIRNAME}executeShell.exe" ]
then
    EXECMD=${DIRNAME}executeShell.exe
fi

if [ `$PUTFILE 
echo $PUTFILE
echo $EXECMD
exit 0
putFile

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
