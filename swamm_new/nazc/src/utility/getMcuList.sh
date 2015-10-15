#!/bin/bash
sqlplus -S aimir/aimir <<EOF
    col nid format 99999999999
    col ipaddr format A15
    set pagesize 0
    set heading off
    set feedback off
    select nid, ipaddr from (select to_number(id) nid, ipaddr from mi_mcu where ipaddr is not null)
        order by nid;
EOF
