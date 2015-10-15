#!/bin/bash

SMSMSG=/disk1/nuricgi/dcu/exec/smsmsg

echo "Content-type: text/html"
echo ""

# Save the old internal field separator.
  OIFS="$IFS"

# Set the field separator to & and parse the QUERY_STRING at the ampersand.
  Args=
  if [ ${#QUERY_STRING} -gt 0 ]; then
    IFS="${IFS}&"
    set $QUERY_STRING
    Args="$*"
    IFS="$OIFS"
  fi

# Next parse the individual "name=value" tokens.

  KEY=""
  CMD=""
  PARAMS=""

  for i in $Args ;do

#       Set the field separator to =
        IFS="${OIFS}="
        set $i
        IFS="${OIFS}"

        # Don't allow "/" changed to " ". Prevent hacker problems.
        #V=$(echo $2 | sed 's|[\]||g' | sed 's|%20| |g')
        V=$(echo $2 | sed 's|%20| |g')
        case $1 in
                key) KEY="-k $V"
                       ;;
                # Filter for "/" not applied here
                cmd) CMD="-c $V"
                       ;;
                param) PARAMS="${PARAMS} -p $V"
                       ;;
        esac
  done

  RES=$($SMSMSG $KEY $CMD $PARAMS 2>&1)
  if [ "$?" == "0" ]; then
    echo $RES
  else
    echo "Execute error : $Args"
  fi

exit 0
