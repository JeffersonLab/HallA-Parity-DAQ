/************************************************************************
 *  STR7200.c                                                           *
 *   subroutines which are used to interface with the                   *
 *   32 channel scaler card STRUCK model STR7200                        *
 *                                                                      *  
 * Date  : February 05 99                                               *   
 * Modifications:                                                       *
 * 02/05/99    R.T.     Start from the SIS3801 subroutines              *
 *                      Note: The SIS3801 scaler is used as the SIS3800 *
 *                            by using the boot file #4                 *
 *                            (jumpers array J500)                      *
 *                                                                      *
 * June 3, 99  R.M.     Cleaned up code, and made routines and variables*
 *                      truly unique to avoid clash with 3801 mode      *
 *                                                                      *
 * June 10, 2002 J. Liu Start from SIS3800.c and scaler32Lib.c routines *
 * 20 Jun, 2002  PMK    Change to A32 mode.  Change back to A24 mode.   *
 * 21aug2002     pmk    Add a routine to clear a single channel from    *
 *                      software.                                       *
 *                      Added a routine to display the addresses for    *
 *                      some of the registers; this was to aid the      *
 *                      debugging of the memory assignments.            *
 ************************************************************************/

#include <vxWorks.h>
#include <types.h>
#include <vme.h>
#include <stdioLib.h>
#include <logLib.h>
#include <math.h>
#include <string.h>
#include <ioLib.h>
#include "vxLib.h"

/****************************************************************************/

#include "STR7200.h" 

IMPORT  STATUS sysBusToLocalAdrs(int, char *, char **);

/* Define global variables */
int Nscale7200 = 0;

volatile struct STR7200CSREG *s7200p[NMAX7200];/*pointer to the scaler memory map*/

/* Raphael's choice for initial csr of SIS3800 board  */
/*      0x70      :  Disable IRQ source 0, 1, and 2;
 *          00    :  nothing;
 *            f   :  Disable broadcast mode, Input Test mode, and 25 MHz test pulses;
 *             d  :  Clear the Control Input mode, and the user LED.
 *              00:  nothing.
 */ 
unsigned long mycsr    = 0x7000fd00;
int input_control_mode = 3;

UINT32 AddrType = 0x39; //address increment for A24 non-privileged data access

/*******************************************************************************
*
* scale32Init - Initialize Scale 32 library. Disable/Clear Counters
*
*
* RETURNS: OK, or ERROR if the scaler address is invalid.
*/
STATUS initSTR7200(UINT32 addr, UINT32 addr_inc, int nscalers)
{
  
  int ii, res, rdata, errFlag = 0;
  unsigned long laddr;
  
  /* Check for valid address */
  if(addr==0) {
    printf("Init: ERROR: Must specify a local VME-based (A32) address for Scaler 0\n");
    return(ERROR);
  }else{
    if((addr_inc==0)||(nscalers==0))
      nscalers = 1; /* assume only one scaler to initialize */
    
    Nscale7200 = 0;
    
    /*get the scaler's address using sysBusToLocalAdrs routine. passing 
     *the hardware address as an variable*/

    res = sysBusToLocalAdrs(AddrType,(char *)addr,(char **)&laddr);
    printf("first res = %d \n",res);
    if (res != 0) {
      printf("initSTR7200: ERROR in sysBusToLocalAdrs(0x%x,0x%x,&laddr) \n",AddrType,addr); 
      return(ERROR);
    } else {
      for(ii=0;ii<nscalers;ii++){
	s7200p[ii] = (struct STR7200CSREG *)(laddr + SCAL_BASE_ADDR_OFFSET + ii*addr_inc);
	/* Check if Board exists at that address */
	res = vxMemProbe((char *) s7200p[ii],0,4,(char *)&rdata);	
	printf("second res = %d \n", res);
	if(res < 0) {
	  printf("initSTR7200: ERROR: No addressable board at addr=0x%x\n",(UINT32) s7200p[ii]);
	  s7200p[ii] = NULL;
	  errFlag=1;
       	  break;
	}
	Nscale7200++;
	printf("Initialized Scaler ID %d at address 0x%08x \n",ii,(UINT32) s7200p[ii]);
      }
    }
  }
  
  /* Disable/Clear all scalers */
  for(ii=0;ii<Nscale7200;ii++){
    s7200p[ii]->reset=1;
    s7200p[ii]->csr=mycsr;
    s7200p[ii]->clear=1;
    setControlInpModeSTR7200(input_control_mode,ii);
    printf("Scaler STR7200[%d] started\n", ii);
  }
  
  if(errFlag > 0) {
    printf("scale32Init: ERROR: Unable to initialize any Scalers \n");
    if(Nscale7200 >0)
      printf("scale32Init: %d Scaler(s) successfully initialized\n",Nscale7200);
    return(ERROR);
  } else {
    return(OK);
  }
}

/*******************************************************************************
 * Modified from P. King's original routine for Grenoble scalers
 * initSTR7200SomeMore - Initialize STR7200 library. Disable/Clear Counters
 *
 *
 * RETURNS: OK, or ERROR if the scaler address is invalid.
 */

STATUS 
initSTR7200SomeMore (UINT32 addr, UINT32 addr_inc, int nscalers)
{
  int ii, res, rdata, errFlag = 0;
  int base_id;
  int loop3;
  int board_is_in_list;
  unsigned long laddr;
  UINT32 full_laddr;
  
  printf("addr = %d \n",addr);
  /* Check for valid address */
  if(addr==0) {
    printf("initSTR7200SomeMore: ERROR: Must specify a local VME-based (A32) address for Scaler 0\n");
    return(ERROR);
  }else{
    if((addr_inc==0)||(nscalers==0))
      nscalers = 1; /* assume only one scaler to initialize */

    //    Nscale7200 = 0;  //Do not set Nscale7200 to zero in this routine
    base_id   = Nscale7200;      //The first scaler added will have the ID value "Nscale7200".
    nscalers += base_id;       //Add these to get the maximum ID value for this loop.
    
    /* get the scaler's address */
    res = sysBusToLocalAdrs(AddrType,(char *)addr,(char **)&laddr);
    printf("first res = %d \n",res);
    if (res != 0) {
      printf("scale7200InitSomeMore: ERROR in sysBusToLocalAdrs(0x39,0x%x,&laddr) \n",addr);
      return(ERROR);
    } else {
      for (ii=base_id ;ii<nscalers ;ii++) {
	/* Check if the board is already in the list.  */
	board_is_in_list = 0;
	full_laddr = (UINT32)(laddr + SCAL_BASE_ADDR_OFFSET + (ii-base_id)*addr_inc);
	for (loop3=0; loop3<ii; loop3++){
	  if ((UINT32) s7200p[loop3] == full_laddr ){
	    board_is_in_list = 1;
	  }
	}
	if (board_is_in_list == 1){
	  printf("initSTR7200SomeMore: ERROR: Board addr=0x%x is already listed\n", full_laddr );
	  s7200p[ii] = NULL;
	  break;
	}
	
	s7200p[ii] = (struct STR7200CSREG *)(full_laddr);
	/* Check if Board exists at that address */
	res = vxMemProbe((char *) s7200p[ii],0,4,(char *)&rdata);
	printf("second res = %d \n", res);
	if(res < 0) {
	  printf("initSTR7200SomeMore: ERROR: No addressable board at addr=0x%x\n",(UINT32) s7200p[ii]);
	  s7200p[ii] = NULL;
	  errFlag=1;
       	  break;
	}
	Nscale7200++;
	printf("Initialized Scaler ID %d at address 0x%08x \n",ii,(UINT32) s7200p[ii]);
      }
    }	
  }
  
  /* Disable/Clear all scalers */
  for(ii=0;ii<Nscale7200;ii++) {
    s7200p[ii]->reset=1;
    s7200p[ii]->csr=mycsr;
    s7200p[ii]->clear=1;
    setControlInpModeSTR7200(input_control_mode,ii);
    printf("Scaler STR7200[%d] started\n", ii);
  }
  
  if(errFlag > 0) {
    printf("initSTR7200SomeMore: ERROR: Unable to initialize any Scalers \n");
    if(Nscale7200 >0)
      printf("initSTR7200SomeMore: a total of %d Scaler(s) successfully initialized\n",Nscale7200);
    return(ERROR);
  } else {
    return(OK);
  }
}

/***************************************************************************
 *   enableSTR7200(id) : enable the global count for one board
 *   input parameters     : id -- index of the scaler
 *   return 0
 *
 */
int enableSTR7200(int id)
{
  if(s7200p[id]!=NULL){
    s7200p[id]->enCnt=1;
    return 0;
  }
  return 1;
}

/***************************************************************************
 *   countDisableSTR7200(id) : enable the global count for one board
 *   input parameters     : id -- index of the scaler
 *   return 0
 *
 */
int countDisableSTR7200(int id, UINT32 disable_register)
{
  if(s7200p[id]!=NULL){
    s7200p[id]->disCntR = disable_register;
    return 0;
  }
  return 1;
}

/***************************************************************************
 * disableSTR7200(int id): disable the global count for one board
 * input parameters     : id -- index of the scaler
 * return(0)
 * Notes           :  This is the state after Power on or KEY_RESET 
 */

int disableSTR7200(int id) 
{
  s7200p[id]->disCnt=1;
  return 0;
}



/************************************************************************
 * 
 *  dump registers to screen
 */
int dump7200(int id)
{
  int i,j;
  
  printf("ADDR(%#8x) : csr register        : %#8x \n", &s7200p[id]->csr,s7200p[id]->csr)  ; 
  printf("ADDR(%#8x) : VME IRQ control reg : %#8x \n", &s7200p[id]->IntVerR,s7200p[id]->IntVerR ) ; 
  
  
  printf("Here are 32 Channels:\n");  
  j=0;
  for(i=0;i<8;i++) {
    printf("%d to %d :  %d %d %d %d \n",
	   j,j+4,s7200p[id]->readCounter[j],s7200p[id]->readCounter[j+1],
	   s7200p[id]->readCounter[j+2],s7200p[id]->readCounter[j+3]) ;   
    j+=4;
  }
  
  return 0;
}



/************************************************************************/

/* 
 *  dump registers to screen
 *  But only those registers which are readable.
 */
void dumpcsr7200(int id)
{  
  printf("ADDR(%#8x) : csr register        : %#8x \n", &s7200p[id]->csr,s7200p[0]->csr)  ; 
  printf("ADDR(%#8x) : VME IRQ control reg : %#8x \n", &s7200p[id]->IntVerR,s7200p[0]->IntVerR ) ; 
}

/*******************************************************************************
 *   setControlInpMode(int inpmode)  : set configuration for control input pins
 *              input parameters     : inpmode - type of Mode 0-3
 *              return               : 0
 *
 *  
 */
int setControlInpModeSTR7200(int inpmode, int id)
{ 
  int temp;
  
  if ((inpmode>3)||(inpmode<0)) return ERROR;
  temp=inpmode<<2;
  s7200p[id]->csr=0x00000C00; /* clear Mode bits to Mode 0*/
  s7200p[id]->csr=temp;       /* set new Mode */
  return 0;
}

/*******************************************************************************
 *   ReadSTR7200  : Read Counters (routine called by coda). 
 *              input parameters     : index of the scaler and a pointer to
 *                                      the output data
 *              return               : number of scaler values read
 *  
 */
int ReadSTR7200(int id, UINT32 *data){ 
  int ii, nvalues = 0;
  
  if((id<0) || (s7200p[id] == NULL)) {
    logMsg("ReadSTR7200: ERROR : Scaler %d not initialized \n",id,0,0,0,0,0);
    return(-1);
  }
  /* First read the first word from the counter. */
  *data++ = s7200p[id]->readCounter[0];
  nvalues++;
  /* Now read the rest from the shadow register. *
   * (This break-up of reads is recommended in   *
   *  the manual.)                               */
  for(ii=1;ii<32;ii++){
    *data++ = s7200p[id]->readShadow[ii];
    nvalues++;
  }
  return(nvalues);
}

/*******************************************************************************
 *   PrintSTR7200  : Read Counters (routine called by coda). 
 *              input parameters     : index of the scaler and a pointer to
 *                                      the output data
 *              return               : number of scaler values read
 *  
 */
int PrintSTR7200(int id){ 
  int ii, nvalues = 0;
  UINT32 data[32];
 
  if((id<0) || (s7200p[id] == NULL)) {
    logMsg("ReadSTR7200: ERROR : Scaler %d not initialized \n",id,0,0,0,0,0);
    return(-1);
  }
  /* First read the first word from the counter. */
  data[0] = s7200p[id]->readCounter[0];
  nvalues++;
  /* Now read the rest from the shadow register. *
   * (This break-up of reads is recommended in   *
   *  the manual.)                               */
  for(ii=1;ii<32;ii++){
    data[ii] = s7200p[id]->readShadow[ii];
    nvalues++;
  }
  ii = 0;
  while (ii<32){
    printf("0x%.8x  0x%.8x  0x%.8x  0x%.8x\n", data[ii], data[ii+1],
	   data[ii+2], data[ii+3]);
    ii += 4;
  }

  return(nvalues);
}

/*******************************************************************************
 *   PrintAddrSTR7200()  : Clear all Counters. 
 *              input parameters     : none
 *              return               : 
 *
 *  
 */
void PrintAddrSTR7200(int id){ 

  if(s7200p[id]!=NULL){
    printf("Base addr          == 0x%.8lx\n",&(s7200p[id]->csr));
    printf("  Addr(clear)      == 0x%.8lx\n",&(s7200p[id]->clear));
    printf("  Addr(enCnt)      == 0x%.8lx\n",&(s7200p[id]->enCnt));
    printf("  Addr(gDiscnt1)   == 0x%.8lx\n",&(s7200p[id]->gDiscnt1));
    printf("  Addr(reset)      == 0x%.8lx\n",&(s7200p[id]->reset));
    printf("  Addr(testclkR)   == 0x%.8lx\n",&(s7200p[id]->testclkR));
    printf("  Addr(empty4.0)   == 0x%.8lx\n",&(s7200p[id]->empty4[0]));
    printf("  Addr(clrCount.0) == 0x%.8lx\n",&(s7200p[id]->clrCount[0]));
    printf("  Addr(readShad.0) == 0x%.8lx\n",&(s7200p[id]->readShadow[0]));
    printf("  Addr(readCoun.0) == 0x%.8lx\n",&(s7200p[id]->readCounter[0]));
    printf("  Addr(readClr.0)  == 0x%.8lx\n",&(s7200p[id]->readClrCount[0]));
  }
}

/*******************************************************************************
 *   ClrSTR7200()  : Clear all Counters. 
 *              input parameters     : none
 *              return               : 
 *
 *  
 */
void ClrSTR7200(void){ 
  int id;  
  for (id=0; id<NMAX7200; id++){
    if(s7200p[id]) s7200p[id]->clear=1;
  }  
}

/*******************************************************************************
 *   ClrOneChannelSTR7200()  : Clear all Counters. 
 *              input parameters     : Module ID, scaler channel.
 *              return               : 
 *
 *  
 */
void ClrOneChannelSTR7200(int id, int channel){ 
    if(s7200p[id]) s7200p[id]->clrCount[channel]=1;
}

/*******************************************************************************
 *   EnableBroad7200()  : Enable broadcast. 
 *              input parameters     : none
 *              return               : 
 *
 *  
 */
void EnableBroad7200(void){ 
  /*we are not likely to run in the broadcast mode. so just make a dummy 
   *routine here */
}

/*******************************************************************************
 *   ClrBroad7200()  : Clear broadcast. 
 *              input parameters     : none
 *              return               : 
 *
 *  
 */
void ClrBroad7200(void){ 
  /*we are not likely to run in the broadcast mode. so just make a dummy 
   *routine here */  
}

/*******************************************************************************
 *   EnableClock7200()  : Enable clock 
 *              input parameters     : none
 *              return               : 
 *
 *  
 */
void EnableClock7200(void){ 
  /* dummy routine for now */
}

/*******************************************************************************
*   DisableClock7200()  : Disable clock 
*              input parameters     : none
*              return               : 
*
*  
*/
void DisableClock7200(void){ 
  /* dummy routine for now */
}

