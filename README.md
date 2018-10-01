Parity DAQ in Hall A

This is for the software for the Hall A Parity DAQ. 

Cameron Clarke turned the directory ~apar/devices into a github repository
in August 2018.

Sept 2018 

Bob Michaels made a tarfile of all directories and archived that in MSS.
For obsolete or unused software Bob either deleted or moved them to 
another place.  E.g. ~/caen767/.  

Only stuff remains that is part of the Parity DAQ.   
 
Also, Bob looked at differences to ~/caryn/devices and pulled all her changes over,
then commited them here.  We would prefer to use "git" in the future.  Make a branch
and/or make a "forked" copy of the repository and make "pull" requests.

 In March 2015, this software was imported to Bob Michaels github repository
from CVS.  Prior to 2015 the software had been maintained in CVS at JLab.

Compiling

To clean all object files and executibles

make clean

To make all files

make all

  (and if you just type "make" it only compiles adc16, not sure why.  but "make all" works.)

To make specific packages

18-bit HAPPEX ADC libs

make adc18  

Qweak ADC libs

make vqwk

Green monster aka config

make config/config


