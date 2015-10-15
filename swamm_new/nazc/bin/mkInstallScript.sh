#!/bin/bash

if [ -z "$1" ]; then exit; fi

while read -r line
do
    case "$line" in
        PKGFILE= ) echo "PKGFILE=$1";;
        * ) echo "$line";;
    esac
done < install
