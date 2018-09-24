
/* launch with:

ld < 'bmwClient.o'

sp bmwClient

 */

/*
  $Header: /group/halla/parity/cvs/devices/bmw/bmwClient.c,v 1.8 2010/03/10 17:07:48 paschke Exp $
 */

/***** includes *****/
#include "bmw.h"
#include "/adaqfs/home/apar/devices/module_map.h"

STATUS defineSeqBMW(int iseq, int coil, int amp, int nperiods) {
  fprintf ( stdout, "bmwDummy::: this is a dummy routine, with no functionality \n");
  return (OK);
}



STATUS defineSeq2CoilBMW(int iseq, int coil1, int amp, int coil2, int amp2, int nperiods) {
  fprintf ( stdout, "bmwDummy::: this is a dummy routine, with no functionality \n");
  return (OK); 
}

STATUS activateSeqBMW(int iseq, int active) {
  fprintf ( stdout, "bmwDummy::: this is a dummy routine, with no functionality \n");
  return (OK); 
}

STATUS initBMW(){
  fprintf ( stdout, "bmwDummy::: this is a dummy routine, with no functionality \n");
  return (OK); 
}


STATUS bmwClient() {
  fprintf ( stdout, "bmwDummy::: this is a dummy routine, with no functionality \n");
  return (OK); 
}

STATUS bmwClient_script (){
  fprintf ( stdout, "bmwDummy::: this is a dummy routine, with no functionality \n");
  return (OK);
}


/******************************************************************************

getConfigBMW - print coils, amplitude, frequency and period for all sequences

*/

STATUS getConfigBMW (){
  fprintf ( stdout, "bmwDummy::: this is a dummy routine, with no functionality \n");
  return (OK);
}
 


/******************************************************************************

getConfigSeqBMW - read out coils, amplitude, frequency and period for a specific sequence

*/

STATUS getConfigSeqBMW (int iseq){
  fprintf ( stdout, "bmwDummy::: this is a dummy routine, with no functionality \n");
  return (OK);
}
 
  
/******************************************************************************

setNotice - set FlexIO or DAC outputs for bmw script

*/

STATUS setNotice(int flag, int val) {
  fprintf ( stdout, "bmwDummy::: this is a dummy routine, with no functionality \n");
  return(OK);
}

STATUS trigBMW (int iseq){
  fprintf ( stdout, "bmwDummy::: this is a dummy routine, with no functionality \n");
  return (OK);
}
