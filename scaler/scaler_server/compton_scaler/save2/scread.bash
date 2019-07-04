#!/bin/bash
 
if [ $# -ne 1 ] ; then
   echo "Usage: scread.bash Arg"
   echo "where arg = channel to read"
   exit
fi

/home/compton/bob/scaler/scread -p $1 -r
