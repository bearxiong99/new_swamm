#!/bin/bash

INIT=init.tar.gz
CONF=conf.tar.gz
PPP=ppp.tar.gz
DB=db.tar.gz
CONFFILE="ethernet.type mobile.module.type mobile.mode server.addr var.conf"
INITFILE="localtime interfaces"

echo "Freeze Initfiles ..."
(cd /app/init; tar zcf ${INIT} ${INITFILE}; mv /app/init/${INIT} /app/sw)
echo "Freeze Configurations ..."
(cd /app/conf; tar zcf ${CONF} ${CONFFILE}; mv /app/conf/${CONF} /app/sw)
echo "Freeze DB Files ..."
(cd /app/sqlite; tar zcf ${DB} *.db; mv /app/sqlite/${DB} /app/sw)
echo "Freeze PPP Informations ..."
(cd /app/sw; tar zcf ${PPP} ppp)

echo "Done"
