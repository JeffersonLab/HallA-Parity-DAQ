/* scaler lib for PVDIS */
 
#include <vxWorks.h>
#include <types.h>
#include <vme.h>
#include <stdioLib.h>
#include <logLib.h>
#include <math.h>
#include <string.h>
#include <ioLib.h>
#include <time.h>

#include "GlobSca.h"



/***********************************************************************/
void GlobalScalerClr(void) {

  int isca,ichan;


   for (isca=0; isca<TOTSCALERS; isca++) {
     for (ichan=0; ichan<32; ichan++) {
       global_scaler_sum[32*isca + ichan] = 0;
     }
   }

}



/***********************************************************************/
void GlobalScalerIncr(int isca, int ichan, int data) {

  if (isca >= 0 && isca < TOTSCALERS && ichan >= 0 && ichan < 32) {

      global_scaler_sum[32*isca + ichan] += data;

  }

}

/***********************************************************************/
int GlobalScalerData(int isca, int ichan) {

  if (isca >= 0 && isca < TOTSCALERS && ichan >= 0 && ichan < 32) {

     return global_scaler_sum[32*isca + ichan];

  }

  return 0;

}

