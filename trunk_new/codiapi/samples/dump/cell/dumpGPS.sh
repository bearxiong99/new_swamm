#!/bin/bash

while read line
do
  case $line in
    OUT:*)
        _candd=${line#*:}
        command=${_candd%:*}
        dvalue=${_candd#*:}
        case $command in
            CODI)
                echo "REPLACE INTO atc_codi_tbl \
                    (_id, latitude, nsIndicator, longitude, ewIndicator) \
                    VALUES \
                    ('$dvalue', 37.479950 , 'N', 126.882584, 'E');"
            ;;
            DUMP)
                OIFS=$IFS; IFS=,
                echo "$dvalue\n" | ( \
                    read codi cell lqi rssi gpsHeader utcTime latitude nsIndicator longitude ewIndicator \
                        satellites hdop msl units geoidalSeparation units2 ageOfDiff csum; \
                    echo "REPLACE INTO atc_cell_tbl \
                        (_id, _codi, latitude, nsIndicator, longitude, ewIndicator, \
                            lastDate, lastUtcTime, lqi, rssi) \
                        VALUES \
                        ('$cell', '$codi', $latitude, '$nsIndicator', $longitude, '$ewIndicator', \
                            '`date "+%d%m%y"`', $utcTime, $lqi, $rssi );" \
                )
                IFS=$OIFS
            ;;
        esac
    ;;
  esac
done
