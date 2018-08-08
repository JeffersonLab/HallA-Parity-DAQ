
/* This is for version 1 of TS */
/* It is what we've been using forever. */

struct vme_ts1 {
    unsigned long csr;       
    unsigned long trig;
    unsigned long roc;
    unsigned long sync;
    unsigned long test;
    unsigned long state;
    unsigned long blank_1;           
    unsigned long blank_2;           
    unsigned long prescale[8];
    unsigned long timer[5];
    unsigned long blank_3;           
    unsigned long blank_4;           
    unsigned long blank_5;           
    unsigned long sc_as;
    unsigned long scale_0a;
    unsigned long scale_1a;
    unsigned long blank_6;           
    unsigned long blank_7;          
    unsigned long scale_0b;
    unsigned long scale_1b;
  };

/* This is for version 2 of TS */
/* However, don't invoke it here because "makelist" does that */
#ifdef WHENUSED
volatile struct vme_ts2 {
    volatile unsigned long csr;       
    volatile unsigned long csr2;       
    volatile unsigned long trig;
    volatile unsigned long roc;
    volatile unsigned long sync;
    volatile unsigned long trigCount;
    volatile unsigned long trigData;
    volatile unsigned long lrocData;
    volatile unsigned long prescale[8];
    volatile unsigned long timer[5];
    volatile unsigned long intVec;
    volatile unsigned long rocBufStatus;
    volatile unsigned long lrocBufStatus;
    volatile unsigned long rocAckStatus;
    volatile unsigned long userData1;
    volatile unsigned long userData2;
    volatile unsigned long state;
    volatile unsigned long test;
    volatile unsigned long blank1;
    volatile unsigned long scalAssign;
    volatile unsigned long scalControl;
    volatile unsigned long scaler[18];
    volatile unsigned long scalEvent;
    volatile unsigned long scalLive1;
    volatile unsigned long scalLive2;
    volatile unsigned long id;
  } VME_TS2;
#endif

/* The deployed TS is a TS1 */
volatile struct vme_ts1  *ts1;

