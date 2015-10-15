#!/bin/bash

NETSTAT=/bin/netstat
TAIL=/usr/bin/tail
ROUTE=/sbin/route

$NETSTAT -nr | $TAIL -n+3 | while read dest gateway mask flag mss window irtt iface 
do
    case "$dest" in
        0.0.0.0 )
            $ROUTE del -net $dest gw $gateway
        ;;
    esac
done  

