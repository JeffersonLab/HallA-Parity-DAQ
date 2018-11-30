/* scaler1.h    */

/* If you run 3801 mode NUMSCALER needs to be 2 (there are 2 virtual 
   scalers, since data are sorted by helicity.)
   And for 3800 mode NUMSCALER may be 1 if there is 1 scaler. */
#define NUMSCALER 1

#define MAXSCALER 2
int vmeoff[MAXSCALER];
int vmeheader[MAXSCALER];
int scalertype[MAXSCALER];






