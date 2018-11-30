#!/bin/bash
# Compare versions between $VERS1 and $VERS2, the two args
if [ $# -ne 2 ] ; then
  echo "usage:  comp_vers.com  dir1  dir2"
  echo "to compare in 2 directories"
  exit
fi
VERS1=$1
VERS2=$2
COMP_LOG=$(pwd)/comp_vers.log
echo "Comparsion of $VERS1 and $VERS2 at `date`">$COMP_LOG
echo "< in $VERS1,  > in $VERS2">>$COMP_LOG
echo " ">>$COMP_LOG
cd $VERS1
  for file in `ls *.C *.c *.crl *.h Makefile` 
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
  for file in `ls *.C *.c *.crl *.h Makefile` 
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
