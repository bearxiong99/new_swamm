#!/bin/bash

OLDREV=0
STEP=0

CORETYPE=`uname -m`

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

    # update mobile.module.type  (Issue #1264)
    case "$CORETYPE" in
        armv5tel)   # PXA255
            if [ ! -e /app/conf/mobile.module.type ]; then
              echo -n " [$STEP] Patch mobile.module.type ....."; 
              case `head -1 /app/conf/mobile.type` in
                1)  # GSM
                    echo -n "mc55" > /app/conf/mobile.module.type;
                ;;
                2)  # CDMA
                    echo -n "dtss800" > /app/conf/mobile.module.type;
                ;;
              esac
              echo " [Done]"; let STEP=$STEP+1
            fi
        ;;
    esac

    # update mobile APN options (Issue #1264 ~2458)
    case "$CORETYPE" in
    armv7l)   # TI AM3352
        echo -n " [$STEP] Patch APN options ....."; 
        PDIR=/app/sw/ppp/peers
        TMP=/tmp/$$.tmp
        ls $PDIR | while read apnfile
        do
            rm -f $TMP 2> /dev/null
            CHK=0
            if grep -e "^user.*''" $PDIR/$apnfile > /dev/null; then CHK=1; fi
            if grep -e "^password.*''" $PDIR/$apnfile > /dev/null; then CHK=1; fi

            if [ "$CHK" -gt 0 ]
            then
                exec 3<> $TMP
                cat $PDIR/$apnfile | while read line
                do
                    if echo $line | grep -e "^user.*''" > /dev/null 
                    then
                        echo "user 'guest' # username (no data acceleration)" >&3
                    elif echo $line | grep -e "^password.*''" > /dev/null 
                    then
                        echo "password 'guest' # a common GPRS/EDGE password" >&3
                    else
                        echo "$line" >&3
                    fi
                done
                exec 3>&-
                cp $TMP $PDIR/$apnfile 2> /dev/null
                rm -f $TMP
            fi
        done 
        echo " [Done]"; let STEP=$STEP+1
        ;;
    esac


echo " [$STEP] NZC Patch Script DONE"
exit 0
