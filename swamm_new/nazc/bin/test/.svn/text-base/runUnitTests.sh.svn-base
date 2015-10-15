#!/bin/bash

OPT=$*

find . -name 'test*.exe' | while read line
do
    case "$line" in
      *testExternal* ) ;;
      * ) $line $OPT ;;
    esac
done 
