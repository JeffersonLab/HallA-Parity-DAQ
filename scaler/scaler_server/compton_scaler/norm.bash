#!/bin/bash

export EPICS_CA_ADDR_LIST=129.57.255.13
 
if [ $# -ne 1 ] ; then
   echo "Usage: scread.bash Arg"
   echo "where arg = channel to read"
   exit
fi
current=$(eval /usr/local/epics/bin/Linux/caget -t hac_bcm_average)
scal=$(eval /home/compton/bob/scaler/scread -p $1 -r)
#echo $scal
#echo $current
echo if \($current \>0.5 \) $scal / $current else 0| bc -q
