#!/bin/bash
 
if [ $# -ne 1 ] ; then
   echo "Usage: scread.bash Arg"
   echo "where arg = channel to read"
   exit
fi

/adaqfs/home/apar/scaler/readout/scread -p $1 -r
