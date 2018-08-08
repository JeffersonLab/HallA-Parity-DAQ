#!/bin/bash
# Compare versions between $VERS1 and $VERS2 
VERS1=/adaqfs/home/apar/dev_luis/crl
VERS2=/adaqfs/home/apar/devices/crl
COMP_LOG=$(pwd)/comp_vers.log
echo "Comparsion of $VERS1 and $VERS2 at `date`">$COMP_LOG
echo "< in $VERS1,  > in $VERS2">>$COMP_LOG
echo " ">>$COMP_LOG
cd $VERS1
  for file in `ls *.crl  Makefile` 
  do      
    echo "****** file: $(pwd)/$file ******" >> $COMP_LOG
    echo " " >> $COMP_LOG
    if [ -e $VERS2/$file ] ; then
      diff $(pwd)/$file $VERS2/$file >> $COMP_LOG
    else
      echo " ">>$COMP_LOG
      echo "File $file not found in $VERS2 !!">>$COMP_LOG
      echo "File $file not found in $VERS2 !!"
    fi
  done
# Now see if file in $VERS2 is also in $VERS1
cd $VERS2
  for file in `ls *.crl  Makefile` 
  do      
    echo "****** file: $(pwd)/$file ******" >> $COMP_LOG
    echo " " >> $COMP_LOG
    if [ -e $VERS1/$file ] ; then
          :
    else
      echo " ">>$COMP_LOG
      echo "File $file not found in $VERS1 !!">>$COMP_LOG
      echo "File $file not found in $VERS1 !!"
    fi
  done
