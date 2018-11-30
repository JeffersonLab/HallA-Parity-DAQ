/* scaler2.h */

int ix, res;
unsigned long laddr;
static int addr_assign=0;

/* Address offset in VME */

vmeoff[0]=0xab1000;

/* if using 3800 mode 
   scalertype[0]=3800;  */

/* if using 3801 mode, there are 2 virtual scalers */
/* scalertype[0]=380101;
   scalertype[1]=380102; */

 scalertype[0]=3800;







