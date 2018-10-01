/* Header for JLab 18-bit ADC for HAPPEX */
/* Nov 2006,  1st version, Bob Michaels */

/* modifications */

/* Sept 2007,  RWM,   modified to have crate-dependent parameters 
                      like NADC, and ADC* 

*/


#define ADC18_MAXBOARDS 10

/* number of HAPPEX 18-bit ADCs */
#ifdef  TESTCRATE
#define NADC 3   
#endif
// Mar 7, 2010.  Kludge: 3 ADCs but actually 2
#ifdef  COUNTINGHOUSE
#define NADC 3    
#endif
#ifdef  INJECTOR
#define NADC 0    
#endif
// dropped L-HRS Oct 28, 2009
#ifdef  LEFTSPECT
#define NADC 3    
#endif
// reduced to 4, Oct 28, 2009
#ifdef  RIGHTSPECT
#define NADC 5   
#endif

#define TIME_INT 11.0	 /* sample time (us) for internal timing sequence */	

//   Relative Addresses of ADCs

#ifdef   TESTCRATE

#define  ADC0 0xCE0000
#define  ADC1 0xB44000
#define  ADC2 0xB04000
#define  ADC3 0xfff000
#define  ADC4 0xfff000
#define  ADC5 0xfff000
#define  ADC6 0xfff000
#define  ADC7 0xfff000   // Undefined.
#define  ADC8 0xff0000
#define  ADC9 0xff0000

#define  ADCLAB0 17
#define  ADCLAB1 9
#define  ADCLAB2 10
#define  ADCLAB3 0xf
#define  ADCLAB4 0xf
#define  ADCLAB5 0xf
#define  ADCLAB6 0xf   
#define  ADCLAB7 0xf
#define  ADCLAB8 0xf
#define  ADCLAB9 0xf

#endif

#ifdef   COUNTINGHOUSE
#define  ADC0 0xCB0000
#define  ADC1 0xCB0000
#define  ADC2 0xCA0000
#define  ADC3 0xfff000
#define  ADC4 0xfff000
#define  ADC5 0xfff000
#define  ADC6 0xfff000
#define  ADC7 0xfff000
#define  ADC8 0xfff000
#define  ADC9 0xfff000
#define  ADCLAB0 1
#define  ADCLAB1 0
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
#define  ADC0 0xfff000
#define  ADC1 0xfff000
#define  ADC2 0xfff000
#define  ADC3 0xfff000
#define  ADC4 0xfff000
#define  ADC5 0xfff000
#define  ADC6 0xfff000
#define  ADC7 0xfff000
#define  ADC8 0xfff000
#define  ADC9 0xfff000
#define  ADCLAB0 0xf
#define  ADCLAB1 0xf
#define  ADCLAB2 0xf
#define  ADCLAB3 0xf
#define  ADCLAB4 0xf
#define  ADCLAB5 0xf
#define  ADCLAB6 0xf
#define  ADCLAB7 0xf
#define  ADCLAB8 0xf
#define  ADCLAB9 0xf
#endif

#ifdef   LEFTSPECT
#define  ADC0 0x22C000
#define  ADC1 0x15C000
#define  ADC2 0xB24000
#define  ADC3 0xfff000
#define  ADC4 0xfff000
#define  ADC5 0xfff000
#define  ADC6 0xfff000
#define  ADC7 0xfff000
#define  ADC8 0xfff000
#define  ADC9 0xfff000
#define  ADCLAB0 4
#define  ADCLAB1 5
#define  ADCLAB2 7
#define  ADCLAB3 0xf
#define  ADCLAB4 0xf
#define  ADCLAB5 0xf
#define  ADCLAB6 0xf
#define  ADCLAB7 0xf
#define  ADCLAB8 0xf
#define  ADCLAB9 0xf
#endif

#ifdef   RIGHTSPECT
#define  ADC0 0xb14000
#define  ADC1 0xb34000
#define  ADC2 0x218000
// Oct 28, old ADC3 (02c000) was removed; put 318000 here.
#define  ADC3 0x318000
#define  ADC4 0xCD0000
#define  ADC5 0xfff000
#define  ADC6 0xfff000
#define  ADC7 0xfff000
#define  ADC8 0xfff000
#define  ADC9 0xfff000
#define  ADCLAB0 11
#define  ADCLAB1 8
#define  ADCLAB2 2
#define  ADCLAB3 3
#define  ADCLAB4 15
#define  ADCLAB5 0xf
#define  ADCLAB6 0xf
#define  ADCLAB7 0xf
#define  ADCLAB8 0xf
#define  ADCLAB9 0xf
#endif


struct adc18_struct {
  long mod_id;		/* 00 */
  long csr;		/* 04 */
  long ctrl;		/* 08 */
  long config;		/* 0C */
  long evt_ct;		/* 10 */
  long evt_word_ct;	/* 14 */
  long dac_value;	/* 18 */
  long reserved_1;	/* 1C */
  long dac_memory;	/* 20 */
  long spare[3];		/* 24,28,2C */
  long delay_1;		/* 30 */
  long delay_2;		/* 34 */
  long reserved_2;	/* 38 */
  long reserved_3;	/* 3C */
  long pedestal[4];	/* 40,44,48,4C  (New, Jan 2008) */
  long pattern;
  long sample;
};

volatile struct adc18_struct *adc18p[ADC18_MAXBOARDS];
volatile unsigned long *adc18_data[ADC18_MAXBOARDS];
volatile int adc18_ok[ADC18_MAXBOARDS];

// Integer Labels of ADCs 
LOCAL unsigned long ADCLABEL[10] = {ADCLAB0, ADCLAB1, ADCLAB2, 
				    ADCLAB3, ADCLAB4, ADCLAB5, 
				    ADCLAB6, ADCLAB7, ADCLAB8, ADCLAB9};

// Addresses of ADCs 
LOCAL unsigned long ADCADDR[10] = {ADC0, ADC1, ADC2, ADC3, ADC4, 
				   ADC5, ADC6, ADC7, ADC8, ADC9};

static int t_mode;
