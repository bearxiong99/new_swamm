#!/bin/bash

CONF=conf.tar
PPP=ppp.tar
CONFFILE="localtime ethernet.type mobile.mode mobile.type var.conf"

echo "Freeze Configurations ..."
(cd /app/conf; tar cf ${CONF} ${CONFFILE}; gzip -f ${CONF}; mv /app/conf/${CONF}.gz /app/sw)
echo "Freeze PPP Informations ..."
(cd /app/sw; tar cf ${PPP} ppp; gzip -f ${PPP})

echo "Done"
