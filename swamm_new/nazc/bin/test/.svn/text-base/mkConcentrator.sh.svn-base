#!/bin/bash

MD5SUM=`which md5sum`
BSDIFF=`which bsdiff`
SVN=`which svn`
BASEURL=
REVISION=HEAD
FIXTURE='fixture/fwImages/Concentrator'

while getopts "r:h" Option
do
    case $Option in
        r   ) REVISION=$OPTARG;;
        *   ) echo $"Usage: $0 [-r{revision}]"; exit 1;;
    esac
done

shift $((OPTIND - 1))

BASEURL=`${SVN} info ../.. | sed -n '/URL:/p' | sed -e 's/URL: \(.*\)\/nazc.*/\1/' `

[ "x${BASEURL}" = "x" ] && exit 0

[ ! -d tmp ] && mkdir tmp
cd tmp
${SVN} checkout -r ${REVISION} ${BASEURL}/codiapi
${SVN} checkout -r ${REVISION} ${BASEURL}/nazc

cd nazc
yes | ./configure
if  ! make 
then
    echo "Build Concentrator fail : Revision ${REVISION}"
    exit 1
fi
cd ../..

cp tmp/nazc/bin/NZC_*.tar.gz ${FIXTURE}

cd ${FIXTURE}

mv NZC_I211-v3.1-*.tar.gz NZC_I211-old.tar.gz
mv NZC_O201-v3.1-*.tar.gz NZC_O201-old.tar.gz
mv NZC_O312-v3.1-*.tar.gz NZC_O312-old.tar.gz

gzip -dc NZC_I211-old.tar.gz > NZC_I211-old.tar
gzip -dc NZC_O201-old.tar.gz > NZC_O201-old.tar
gzip -dc NZC_O312-old.tar.gz > NZC_O312-old.tar

gzip -dc NZC_I211-v3.1.tar.gz > NZC_I211-v3.1.tar
gzip -dc NZC_O201-v3.1.tar.gz > NZC_O201-v3.1.tar
gzip -dc NZC_O312-v3.1.tar.gz > NZC_O312-v3.1.tar

${BSDIFF} NZC_I211-old.tar NZC_I211-v3.1.tar  NZC_I211-v3.1.diff
${BSDIFF} NZC_O201-old.tar NZC_O201-v3.1.tar  NZC_O201-v3.1.diff
${BSDIFF} NZC_O312-old.tar NZC_O312-v3.1.tar  NZC_O312-v3.1.diff

ls *.diff NZC_*tar.gz | while read file
do
    ${MD5SUM} ${file} | sed -e 's/ .*//g' > ${file}.sum
done

rm -f *.tar

cd ../../..
rm -rf tmp
