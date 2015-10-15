#!/bin/bash

OLDREV=0
STEP=0

echo " [$STEP] NZC Patch Script"; let STEP=$STEP+1

echo -n " [$STEP] Get Revision Number ........."
    if [ -f /app/sw/revision.info ]; then
        mv /app/sw/revision.info /app/conf
    fi
    if [ -f /app/conf/revision.old ]; then
        OLDREV=`cat /app/conf/revision.old`
        OLDREV=${OLDREV/[MSP]/}
    fi
    if [ -f /app/conf/revision.info ]; then
        cp /app/conf/revision.info /app/conf/revision.old
    fi
echo " [${OLDREV}]"; let STEP=$STEP+1

cd /app/sw

    # delete old cli.ini (Issue #2491)
    if [ "$OLDREV" -lt 3796 ]; then
        echo -n " [$STEP] Patch CLI.ini ............"; 
        if [ -f /app/sw/cli.ini ]; then rm /app/sw/cli.ini; fi
        if [ -f /app/conf/cli.ini ]; then rm /app/conf/cli.ini; fi
        echo " [Done]"; let STEP=$STEP+1
    fi

    # delete old passwd (Issue #2494)
    if [ "$OLDREV" -lt 3505 ]; then
        echo -n " [$STEP] Patch password ...........";
        if [ -f /app/conf/passwd ]; then rm /app/conf/passwd; fi
        if [ -f /app/conf/user.conf ]; then rm /app/conf/user.conf; fi
        echo " [Done]"; let STEP=$STEP+1
    fi

    # remove old logging files (RF-14)
    if [ "$OLDREV" -lt 4146 ]; then
        echo -n " [$STEP] Patch LOGGING System ....."; 
        rm -f /app/log/CMH*.log 2> /dev/null
        rm -f /app/log/ACS*.log 2> /dev/null
        rm -f /app/log/EVT*.log 2> /dev/null
        rm -f /app/log/LOG*.log 2> /dev/null
        rm -f /app/log/MOB*.log 2> /dev/null
        rm -f /app/log/SYN*.log 2> /dev/null
        rm -f /app/log/MTR*.log 2> /dev/null
        rm -f /app/log/UG*.log 2> /dev/null
        echo " [Done]"; let STEP=$STEP+1
    fi

echo " [$STEP] NZC Patch Script DONE"
exit 0
