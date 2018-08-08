/* Header for JLab 18-bit ADC for HAPPEX */
/* Nov 2006,  1st version, Bob Michaels */

/* modifications */

/* Sept 2007,  RWM,   modified to have crate-dependent parameters 
                      like NADC, and ADC* 


  dirty trick, when needed: define 1st adc twice but read it the 2nd time 

  July 30, 2014  RWM, reviving CH config, start with 2 ADCs
  Aug 15,  2015          trying more ADCs
  October 2015           prepareing for Fall 2015 run with CH and RHRS

*/


#define ADC18_MAXBOARDS 10

/* number of HAPPEX 18-bit ADCs */
#ifdef  TESTCRATE
#define NADC 3  
#endif
// October 2015
#ifdef  COUNTINGHOUSE
#define NADC 4
#endif
#ifdef  INJECTOR
#define NADC 0    
#endif
// Update Sept 21, 2017
#ifdef  LEFTSPECT
#define NADC 2   
#endif
// Sept 2016
#ifdef  RIGHTSPECT
#define NADC 2   
#endif
// for UVa Crate, 19Aug10, kdp
#ifdef  UVACRATE
#define NADC 1   
#endif

#define TIME_INT 11.0	 /* sample time (us) for internal timing sequence */	

//   Relative Addresses of ADCs

#ifdef   TESTCRATE

#define  ADC0 0x6d4000
#define  ADC1 0x6a4000
#define  ADC2 0xb48000

#define  ADC3 0x218000
#define  ADC4 0x3b8000
#define  ADC5 0x4e4000

#define  ADC6 0xb04000
#define  ADC7 0xb24000
#define  ADC8 0x22c000

#define  ADC9 0x6a4000

#define  ADCLAB0 17
#define  ADCLAB1 12
#define  ADCLAB2 14
#define  ADCLAB3 0xf
#define  ADCLAB4 0xf
#define  ADCLAB5 0xf
#define  ADCLAB6 0xf   
#define  ADCLAB7 0xf
#define  ADCLAB8 0xf
#define  ADCLAB9 0xf

#endif

#ifdef   COUNTINGHOUSE
#define  ADC0 0x6d4000
#define  ADC1 0x6a4000
#define  ADC2 0xb48000
#define  ADC3 0x4e4000

#define  ADC4 0xfff000   // fff = Undefined. 
#define  ADC5 0xfff000   
#define  ADC6 0xfff000
#define  ADC7 0xfff000  
#define  ADC8 0xfff000
#define  ADC9 0xfff000
#define  ADCLAB0 8
#define  ADCLAB1 10
#define  ADCLAB2 0xa
#define  ADCLAB3 0xb
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
#define  ADC0 0xca8000
#define  ADC1 0xb14000
#define  ADC2 0xfff000
#define  ADC3 0xfff000
#define  ADC4 0xfff000
#define  ADC5 0xfff000
#define  ADC6 0xfff000
#define  ADC7 0xfff000
#define  ADC8 0xfff000
#define  ADC9 0xfff000
#define  ADCLAB0 4
#define  ADCLAB1 15
#define  ADCLAB2 0xf
#define  ADCLAB3 0xf
#define  ADCLAB4 0xf
#define  ADCLAB5 0xf
#define  ADCLAB6 0xf
#define  ADCLAB7 0xf
#define  ADCLAB8 0xf
#define  ADCLAB9 0xf
#endif

#ifdef   RIGHTSPECT
#define  ADC0 0x218000
#define  ADC1 0x3b8000
#define  ADC2 0xB18000
#define  ADC3 0x318000
#define  ADC4 0xCC0000
#define  ADC5 0x118000
#define  ADC6 0xfff000
#define  ADC7 0xfff000
#define  ADC8 0xfff000
#define  ADC9 0xfff000
#define  ADCLAB0 11
#define  ADCLAB1 20
#define  ADCLAB2 2
#define  ADCLAB3 3
#define  ADCLAB4 5
#define  ADCLAB5 0xf
#define  ADCLAB6 0xf
#define  ADCLAB7 0xf
#define  ADCLAB8 0xf
#define  ADCLAB9 0xf
#endif

// only have adcx6 @UVA
// copy RIGHTSPECT and modify it
// switch ADC0 with ADC3
// wipe out other adcs
#ifdef   UVACRATE
#define  ADC0 0x02C000
#define  ADC1 0xfff000
#define  ADC2 0xfff000
// Oct 28, old ADC3 (02c000) was removed; put 318000 here.
#define  ADC3 0xfff000
#define  ADC4 0xfff000
#define  ADC5 0xfff000
#define  ADC6 0xfff000
#define  ADC7 0xfff000
#define  ADC8 0xfff000
#define  ADC9 0xfff000
#define  ADCLAB0 6
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
