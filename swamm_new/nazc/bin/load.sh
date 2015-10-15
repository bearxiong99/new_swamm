#!/bin/bash

read Type User Low Sys Idle  < /proc/stat
sleep 1

while true
do
    read type user low sys idle  < /proc/stat

    duser=`expr $user - $User`
    dlow=`expr $low - $Low`
    dsys=`expr $sys - $Sys`
    didle=`expr $idle - $Idle`

    dt=`expr $duser + $dlow + $dsys + $didle`

    up=`expr $duser \* 100 \/ $dt`
    lp=`expr $dlow \*  100 \/ $dt`
    sp=`expr $dsys \*  100 \/ $dt`
    ip=`expr $didle \* 100 \/ $dt`

    printf "User=%2d%% Low=%2d%% Sys=%2d%% Idle=%2d%%\n" $up $lp $sp $ip

    User=$user
    Low=$low
    Sys=$sys
    Idle=$idle

    sleep 1
done
