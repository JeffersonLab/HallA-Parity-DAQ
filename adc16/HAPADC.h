/************************************************************************
 *  HAPADC.h                                                            *
 *   Define the VME map of one HAPPEX ADC, for tests                    *
 *                                                                      *  
 * October, 2000.  R. Michaels / A. Vacheret                            *
 * Modifications:                                                       *
 *                                                                      *  
 ************************************************************************/

//
#define DAC_ON 8
#define DAC_OFF 0
#define LO_GAIN 0
#define HI_GAIN 16
//

/* number of HAPPEX ADCs */
#ifdef  COUNTINGHOUSE
#define NADC 2    // must be <10, or requires changes to static arrays.
#endif
#ifdef  INJECTOR
#define NADC 0    // must be <10, or requires changes to static arrays.
#endif
/* dropped, Oct 28, 2009 */
#ifdef  LEFTSPECT
#define NADC 0    // must be <10, or requires changes to static arrays.
#endif
#ifdef  RIGHTSPECT
#define NADC 0    // must be <10, or requires changes to static arrays.
#endif

#ifdef TESTCRATE
#define NADC 0  
#endif

#define MAXRETRY 20   // max number of attempts to write csr

#define DEF_TIMEOUT 2000  // max number of polls of done bit during readout

#define DEF_DACVAL 3000  // dac value for non-dac noise adcs

//   Relative Addresses of ADCs
#ifdef   TESTCRATE
#define  ADC0 0x850000
#define  ADC1 0x830000   // Undefined.
#define  ADC2 0xff0000
#define  ADC3 0xff0000
#define  ADC4 0xff0000
#define  ADC5 0xff0000
#define  ADC6 0xff0000
#define  ADC7 0xff0000
#define  ADC8 0xff0000
#define  ADC9 0x440000

#define  ADCLAB0 26
#define  ADCLAB1 28   // Undefined
#define  ADCLAB2 0xf
#define  ADCLAB3 0xf
#define  ADCLAB4 0xf
#define  ADCLAB5 0xf
#define  ADCLAB6 0xf
#define  ADCLAB7 0xf
#define  ADCLAB8 0xf
#define  ADCLAB9 20

#endif

#ifdef   COUNTINGHOUSE
#define  ADC0 0x4d0000
#define  ADC1 0x520000
#define  ADC2 0xff0000
#define  ADC3 0xff0000
#define  ADC4 0xff0000
#define  ADC5 0xff0000
#define  ADC6 0xff0000
#define  ADC7 0xff0000
#define  ADC8 0xff0000
#define  ADC9 0xff0000

#define  ADCLAB0 22
#define  ADCLAB1 11
#define  ADCLAB2 0xf
#define  ADCLAB3 0xf
#define  ADCLAB4 0xf
#define  ADCLAB5 0xf
#define  ADCLAB6 0xf
#define  ADCLAB7 0xf
#define  ADCLAB8 0xf
#define  ADCLAB9 0xf
#endif

#ifdef   INJECTOR
#define  ADC0 0x4d0000
#define  ADC1 0x4e0000
#define  ADC2 0x980000
#define  ADC3 0x400000
#define  ADC4 0x4d0000
#define  ADC5 0x480000
#define  ADC6 0x4c0000
#define  ADC7 0x4b0000
#define  ADC8 0x510000
#define  ADC9 0x490000
#define  ADCLAB0 16
#define  ADCLAB1 8
#define  ADCLAB2 6
#define  ADCLAB3 23
#define  ADCLAB4 24
#define  ADCLAB5 25
#define  ADCLAB6 26
#define  ADCLAB7 27
#define  ADCLAB8 21
#define  ADCLAB9 29
#endif

#ifdef   LEFTSPECT
#define  ADC0 0x850000
#define  ADC1 0x430000
#define  ADC2 0x820000
#define  ADC3 0x500000
#define  ADC4 0x440000
#define  ADC5 0x4c0000
#define  ADC6 0x4b0000
#define  ADC7 0x510000
#define  ADC8 0x490000
#define  ADC9 0x5F0000
#define  ADCLAB0 26
#define  ADCLAB1 19
#define  ADCLAB2 27
#define  ADCLAB3 9
#define  ADCLAB4 20
#define  ADCLAB5 45
#define  ADCLAB6 46
#define  ADCLAB7 47
#define  ADCLAB8 48
#define  ADCLAB9 49
#endif

#ifdef   RIGHTSPECT
#define  ADC0 0x830000   
#define  ADC1 0x4E0000
#define  ADC2 0x4B0700
#define  ADC3 0x480000
#define  ADC4 0x820000
#define  ADC5 0x4b0000
#define  ADC6 0x510000
#define  ADC7 0x490000
#define  ADC8 0x8F0000
#define  ADC9 0x4C0000
#define  ADCLAB0 28
#define  ADCLAB1 17
#define  ADCLAB2 13
#define  ADCLAB3 8
#define  ADCLAB4 64
#define  ADCLAB5 65
#define  ADCLAB6 27 //66
#define  ADCLAB7 67
#define  ADCLAB8 68
#define  ADCLAB9 69
#endif


// gain setting for each adc
#ifdef   COUNTINGHOUSE
#define ADC0_DEF_GAINBIT LO_GAIN
#define ADC1_DEF_GAINBIT LO_GAIN
#define ADC2_DEF_GAINBIT HI_GAIN  // changed 13sep09, rupesh
#define ADC3_DEF_GAINBIT LO_GAIN 
#define ADC4_DEF_GAINBIT LO_GAIN
#define ADC5_DEF_GAINBIT LO_GAIN
#define ADC6_DEF_GAINBIT LO_GAIN
#define ADC7_DEF_GAINBIT LO_GAIN
#define ADC8_DEF_GAINBIT LO_GAIN
#define ADC9_DEF_GAINBIT LO_GAIN
#elif defined(RIGHTSPECT)
#define ADC0_DEF_GAINBIT HI_GAIN
#define ADC1_DEF_GAINBIT LO_GAIN
#define ADC2_DEF_GAINBIT HI_GAIN
#define ADC3_DEF_GAINBIT LO_GAIN
#define ADC4_DEF_GAINBIT LO_GAIN
#define ADC5_DEF_GAINBIT LO_GAIN
#define ADC6_DEF_GAINBIT LO_GAIN
#define ADC7_DEF_GAINBIT LO_GAIN
#define ADC8_DEF_GAINBIT LO_GAIN
#define ADC9_DEF_GAINBIT LO_GAIN
#else
#define ADC0_DEF_GAINBIT LO_GAIN
#define ADC1_DEF_GAINBIT LO_GAIN
#define ADC2_DEF_GAINBIT LO_GAIN
#define ADC3_DEF_GAINBIT LO_GAIN
#define ADC4_DEF_GAINBIT LO_GAIN
#define ADC5_DEF_GAINBIT LO_GAIN
#define ADC6_DEF_GAINBIT LO_GAIN
#define ADC7_DEF_GAINBIT LO_GAIN
#define ADC8_DEF_GAINBIT LO_GAIN
#define ADC9_DEF_GAINBIT LO_GAIN
#endif
	    
// DAC noise settings for each adc
#define ADC0_DEF_DACBIT DAC_ON 
#define ADC1_DEF_DACBIT DAC_ON
#define ADC2_DEF_DACBIT DAC_ON
#define ADC3_DEF_DACBIT DAC_ON
#define ADC4_DEF_DACBIT DAC_ON
#define ADC5_DEF_DACBIT DAC_ON
#define ADC6_DEF_DACBIT DAC_ON
#define ADC7_DEF_DACBIT DAC_ON
#define ADC8_DEF_DACBIT DAC_ON
#define ADC9_DEF_DACBIT DAC_ON

// diagnostic mux setting for each adc
#define ADC0_DEF_MUXBIT 3  // channel number muxed to diagnostic IntOut
#define ADC1_DEF_MUXBIT 3
#define ADC2_DEF_MUXBIT 3
#define ADC3_DEF_MUXBIT 3
#define ADC4_DEF_MUXBIT 3
#define ADC5_DEF_MUXBIT 3
#define ADC6_DEF_MUXBIT 3
#define ADC7_DEF_MUXBIT 3
#define ADC8_DEF_MUXBIT 3
#define ADC9_DEF_MUXBIT 3
	    


/* HAPPEX custom 16-bit ADC (board made by Princeton/Harvard) */
struct vme_happex_adc {
  volatile unsigned short adcchan[4];   /* four channels of 16-bit data */
  volatile unsigned short dac;         /* DAC value */
  volatile unsigned short dummy1;
  volatile unsigned short csr;         /* Control, Status Register */
  volatile unsigned short dummy2;
  volatile unsigned short rng;         /* DAC random number (usually not used) */
  volatile unsigned short done;        /* Done = digitization complete */
};

// EXTERN
extern int CODA_RUN_IN_PROGRESS;


// GLOBALS

int ADC_NUMSLOTS = NADC;

// LOCALS

// forward declaration of local methods
LOCAL int writeCSRHAPADC(int, int);
LOCAL int ADC_TIMEOUT  = DEF_TIMEOUT;

//
// arrays of default values
//

// Integer Labels of ADCs (maximum of 10 for now)
LOCAL unsigned long ADCLABEL[10] = {ADCLAB0, ADCLAB1, ADCLAB2, ADCLAB3, ADCLAB4, ADCLAB5, ADCLAB6, ADCLAB7, ADCLAB8, ADCLAB9};

// Addresses of ADCs (maximum of 10 for now)
LOCAL unsigned long ADCADDR[10] = {ADC0, ADC1, ADC2, ADC3, ADC4, ADC5, ADC6, ADC7, ADC8, ADC9};

LOCAL unsigned long ADC_GAINBIT[10] = {ADC0_DEF_GAINBIT, ADC1_DEF_GAINBIT, ADC2_DEF_GAINBIT, ADC3_DEF_GAINBIT, ADC4_DEF_GAINBIT, ADC5_DEF_GAINBIT, ADC6_DEF_GAINBIT, ADC7_DEF_GAINBIT, ADC8_DEF_GAINBIT, ADC9_DEF_GAINBIT}; 

LOCAL unsigned long ADC_MUXBIT[10] = {ADC0_DEF_MUXBIT, ADC1_DEF_MUXBIT, ADC2_DEF_MUXBIT, ADC3_DEF_MUXBIT, ADC4_DEF_MUXBIT, ADC5_DEF_MUXBIT, ADC6_DEF_MUXBIT, ADC7_DEF_MUXBIT, ADC8_DEF_MUXBIT, ADC9_DEF_MUXBIT}; 

LOCAL unsigned long ADC_DACBIT[10] = {ADC0_DEF_DACBIT, ADC1_DEF_DACBIT, ADC2_DEF_DACBIT, ADC3_DEF_DACBIT, ADC4_DEF_DACBIT, ADC5_DEF_DACBIT, ADC6_DEF_DACBIT, ADC7_DEF_DACBIT, ADC8_DEF_DACBIT, ADC9_DEF_DACBIT};
LOCAL int did_init[10] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

//
// local arrays
//

volatile struct vme_happex_adc *adcbrds[NADC];

LOCAL dacvalue[NADC];


