/* scaler2.h       July 1998   R.Michaels

   Executible statement definitions, to put after declarations. 
   Contains:
      VME offset addresses vmeoff[] 
      Types of scalers scalertype[] = Types of scalers --
           1151   =  LeCroy model 1151 
           7200   =  Struck model 7200
           560    =  CAEN model V560           
      The correspondence between VME addresses and scaler
      type is specified here.
   Note: The SIS3800 and SIS3801 scalers use SIS3800.h etc
   and routines downloaded from ../SIS3801.

*/

int ix;
static int addr_assign=0;

for (ix=0; ix< MAXSCALER; ix++) {
  vmeoff[ix] = 0;
  vmeheader[ix] = 0;
  scalertype[ix] = 0;
}

scalertype[0]=3800;       /* For now, just ONE scaler, it is #0 */
/*vmeoff[0] = 0xe0ceb100; *//* here is ceb1 scaler, if it is a LeCroy 1151 */
vmeoff[0] = 0xe0a40000;  /* here is the SIS3800 scaler address */

if(addr_assign==0) {
  addr_assign=1;
/* Note, the 3800, 3801 lies outside the scope of this somewhat old software */
/* However this is useful if the scaler is not a 3800,3801 */
  for (ix=0; ix<NUMSCALER; ix++) {
    if(scalertype[ix]==1151) 
       slecroy1151[ix] = (struct lecroy_1151_scaler *)vmeoff[ix];
    if(scalertype[ix]==7200) 
       sstr7200[ix] = (struct str_7200_scaler *)vmeoff[ix];
    if(scalertype[ix]==560) 
       scaen560[ix] = (struct caen_v560_scaler *)vmeoff[ix];
  }
}









