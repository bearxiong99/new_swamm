#!/bin/bash

RETVAL=0

PORT=29975
DIR=.


start () {

    stop

    ./nweb.exe $PORT $DIR 
    RETVAL=$?
    return $RETVAL
}

stop () {
    ps -eaf | grep nweb | grep -v nweb.sh | grep -v grep | awk '{print $2}' | \
    while read pid
    do
        kill $pid
    done 
}

while getopts "d:p:h" Option
do
    case $Option in
        d   ) DIR=$OPTARG;;
        p   ) PORT=$OPTARG;;
        *   ) echo $"Usage: $0 [-d{direcotry} -p{port}] {start|stop}"; exit 1;;
    esac
done

shift $((OPTIND - 1))

case "$1" in
start)  
        shift
        start $*
        ;;
stop)   stop
        ;;
*)      echo $"Usage: $0 [-d{direcotry} -p{port}] {start|stop}" 
        exit 1
esac

exit $RETVAL

