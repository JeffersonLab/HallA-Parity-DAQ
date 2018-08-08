/* # 1 "RampTest.c" */
/* # 1 "/adaqfs/coda/2.6.2/common/include/rol.h" 1 */
typedef void (*VOIDFUNCPTR) ();
typedef int (*FUNCPTR) ();
typedef unsigned long time_t;
typedef struct semaphore *SEM_ID;
static void __download ();
static void __prestart ();
static void __end ();
static void __pause ();
static void __go ();
static void __done ();
static void __status ();
static int theIntHandler ();
/* # 1 "/adaqfs/coda/2.6.2/common/include/libpart.h" 1 */
/* # 1 "/adaqfs/coda/2.6.2/common/include/mempart.h" 1 */
typedef struct danode			       
{
  struct danode         *n;	               
  struct danode         *p;	               
  struct rol_mem_part   *part;	                   
  int                    fd;		       
  char                  *current;	       
  unsigned long          left;	               
  unsigned long          type;                 
  unsigned long          source;               
  void                   (*reader)();          
  long                   nevent;               
  unsigned long          length;	       
  unsigned long          data[1];	       
} DANODE;
typedef struct alist			       
{
  DANODE        *f;		               
  DANODE        *l;		               
  int            c;			       
  int            to;
  void          (*add_cmd)(struct alist *li);      
  void          *clientData;                 
} DALIST;
typedef struct rol_mem_part *ROL_MEM_ID;
typedef struct rol_mem_part{
    DANODE	 node;		 
    DALIST	 list;		 
    char	 name[40];	 
    void         (*free_cmd)();  
    void         *clientData;     
    int		 size;		 
    int		 incr;		 
    int		 total;		 
    long         part[1];	 
} ROL_MEM_PART;
/* # 62 "/adaqfs/coda/2.6.2/common/include/mempart.h" */
/* # 79 "/adaqfs/coda/2.6.2/common/include/mempart.h" */
/* # 106 "/adaqfs/coda/2.6.2/common/include/mempart.h" */
extern void partFreeAll();  
extern ROL_MEM_ID partCreate (char *name, int size, int c, int incr);
/* # 21 "/adaqfs/coda/2.6.2/common/include/libpart.h" 2 */
/* # 67 "/adaqfs/coda/2.6.2/common/include/rol.h" 2 */
/* # 1 "/adaqfs/coda/2.6.2/common/include/rolInt.h" 1 */
typedef struct rolParameters *rolParam;
typedef struct rolParameters
  {
    char          *name;	      
    char          tclName[20];	       
    char          *listName;	      
    int            runType;	      
    int            runNumber;	      
    VOIDFUNCPTR    rol_code;	      
    int            daproc;	      
    void          *id;		      
    int            nounload;	      
    int            inited;	      
    long          *dabufp;	      
    long          *dabufpi;	      
    ROL_MEM_PART  *pool;              
    ROL_MEM_PART  *output;	      
    ROL_MEM_PART  *input;             
    ROL_MEM_PART  *dispatch;          
    volatile ROL_MEM_PART  *dispQ;    
    unsigned long  recNb;	      
    unsigned long *nevents;           
    int           *async_roc;         
    char          *usrString;	      
    void          *private;	      
    int            pid;               
    int            poll;              
    int primary;		      
    int doDone;			      
  } ROLPARAMS;
/* # 70 "/adaqfs/coda/2.6.2/common/include/rol.h" 2 */
extern ROLPARAMS rolP;
static rolParam rol;
extern int global_env[];
extern long global_env_depth;
extern char *global_routine[100];
extern long data_tx_mode;
extern int cacheInvalidate();
extern int cacheFlush();
static int syncFlag;
static int lateFail;
/* # 1 "/adaqfs/coda/2.6.2/common/include/BankTools.h" 1 */
static int EVENT_type;
long *StartOfEvent[32 ],event_depth__, *StartOfUEvent;
/* # 78 "/adaqfs/coda/2.6.2/common/include/BankTools.h" */
/* # 95 "/adaqfs/coda/2.6.2/common/include/BankTools.h" */
/* # 107 "/adaqfs/coda/2.6.2/common/include/BankTools.h" */
/* # 120 "/adaqfs/coda/2.6.2/common/include/BankTools.h" */
/* # 152 "/adaqfs/coda/2.6.2/common/include/BankTools.h" */
/* # 186 "/adaqfs/coda/2.6.2/common/include/BankTools.h" */
/* # 198 "/adaqfs/coda/2.6.2/common/include/BankTools.h" */
/* # 217 "/adaqfs/coda/2.6.2/common/include/BankTools.h" */
/* # 92 "/adaqfs/coda/2.6.2/common/include/rol.h" 2 */
/* # 1 "/adaqfs/coda/2.6.2/common/include/trigger_dispatch.h" 1 */
static unsigned char dispatch_busy; 
static int intLockKey,trigId;
static int poolEmpty;
static unsigned long theEvMask, currEvMask, currType, evMasks[16 ];
static VOIDFUNCPTR wrapperGenerator;
static FUNCPTR trigRtns[32 ], syncTRtns[32 ], doneRtns[32 ], ttypeRtns[32 ];
static unsigned long Tcode[32 ];
static DANODE *__the_event__, *input_event__, *__user_event__;
/* # 50 "/adaqfs/coda/2.6.2/common/include/trigger_dispatch.h" */
/* # 141 "/adaqfs/coda/2.6.2/common/include/trigger_dispatch.h" */
static void cdodispatch()
{
  unsigned long theType,theSource;
  int ix, go_on;
  DANODE *theNode;
  dispatch_busy = 1;
  go_on = 1;
  while ((rol->dispQ->list.c) && (go_on)) {
{ ( theNode ) = 0; if (( &rol->dispQ->list )->c){ ( &rol->dispQ->list )->c--; ( theNode ) = ( 
&rol->dispQ->list )->f; ( &rol->dispQ->list )->f = ( &rol->dispQ->list )->f->n; }; if (!( &rol->dispQ->list 
)->c) { ( &rol->dispQ->list )->l = 0; }} ;     theType = theNode->type;
    theSource = theNode->source;
    if (theEvMask) { 
      if ((theEvMask & (1<<theSource)) && (theType == currType)) {
	theEvMask = theEvMask & ~(1<<theSource);
	intUnlock(intLockKey); ;
	(*theNode->reader)(theType, Tcode[theSource]);
	intLockKey = intLock(); ;
	if (theNode)
	if (!theEvMask) {
	 if (wrapperGenerator) {event_depth__--; *StartOfEvent[event_depth__] = (long) (((char *) 
(rol->dabufp)) - ((char *) StartOfEvent[event_depth__]));	if ((*StartOfEvent[event_depth__] & 1) != 0) { 
(rol->dabufp) = ((long *)((char *) (rol->dabufp))+1); *StartOfEvent[event_depth__] += 1; }; if 
((*StartOfEvent[event_depth__] & 2) !=0) { *StartOfEvent[event_depth__] = *StartOfEvent[event_depth__] + 2; (rol->dabufp) = 
((long *)((short *) (rol->dabufp))+1);; };	*StartOfEvent[event_depth__] = ( 
(*StartOfEvent[event_depth__]) >> 2) - 1;}; ;	 	 rol->dabufp = ((void *) 0) ; if (__the_event__) { if (rol->output) { {if(! ( 
&(rol->output->list) )->c ){( &(rol->output->list) )->f = ( &(rol->output->list) )->l = ( __the_event__ );( 
__the_event__ )->p = 0;} else {( __the_event__ )->p = ( &(rol->output->list) )->l;( &(rol->output->list) 
)->l->n = ( __the_event__ );( &(rol->output->list) )->l = ( __the_event__ );} ( __the_event__ )->n = 0;( 
&(rol->output->list) )->c++;	if(( &(rol->output->list) )->add_cmd != ((void *) 0) ) (*(( &(rol->output->list) 
)->add_cmd)) (( &(rol->output->list) )); } ; } else { { if (( __the_event__ )->part == 0) { free( __the_event__ 
); __the_event__ = 0; } else { {if(! ( & __the_event__ ->part->list )->c ){( & __the_event__ 
->part->list )->f = ( & __the_event__ ->part->list )->l = ( __the_event__ );( __the_event__ )->p = 0;} else {( 
__the_event__ )->p = ( & __the_event__ ->part->list )->l;( & __the_event__ ->part->list )->l->n = ( 
__the_event__ );( & __the_event__ ->part->list )->l = ( __the_event__ );} ( __the_event__ )->n = 0;( & 
__the_event__ ->part->list )->c++;	if(( & __the_event__ ->part->list )->add_cmd != ((void *) 0) ) (*(( & 
__the_event__ ->part->list )->add_cmd)) (( & __the_event__ ->part->list )); } ; } if( __the_event__ 
->part->free_cmd != ((void *) 0) ) { (*( __the_event__ ->part->free_cmd)) ( __the_event__ ->part->clientData); 
} } ;	} __the_event__ = (DANODE *) 0; } if(input_event__) { { if (( input_event__ )->part == 0) { 
free( input_event__ ); input_event__ = 0; } else { {if(! ( & input_event__ ->part->list )->c ){( & 
input_event__ ->part->list )->f = ( & input_event__ ->part->list )->l = ( input_event__ );( input_event__ 
)->p = 0;} else {( input_event__ )->p = ( & input_event__ ->part->list )->l;( & input_event__ 
->part->list )->l->n = ( input_event__ );( & input_event__ ->part->list )->l = ( input_event__ );} ( 
input_event__ )->n = 0;( & input_event__ ->part->list )->c++;	if(( & input_event__ ->part->list )->add_cmd 
!= ((void *) 0) ) (*(( & input_event__ ->part->list )->add_cmd)) (( & input_event__ ->part->list 
)); } ; } if( input_event__ ->part->free_cmd != ((void *) 0) ) { (*( input_event__ 
->part->free_cmd)) ( input_event__ ->part->clientData); } } ; input_event__ = (DANODE *) 0; } {int ix; if 
(currEvMask) {	for (ix=0; ix < trigId; ix++) {	if (currEvMask & (1<<ix)) (*doneRtns[ix])(); } if 
(rol->pool->list.c) { currEvMask = 0; __done(); rol->doDone = 0; } else { poolEmpty = 1; rol->doDone = 1; } } } ; 	}
      } else {
	{if(! ( &rol->dispQ->list )->c ){( &rol->dispQ->list )->f = ( &rol->dispQ->list )->l = ( 
theNode );( theNode )->p = 0;} else {( theNode )->p = ( &rol->dispQ->list )->l;( &rol->dispQ->list 
)->l->n = ( theNode );( &rol->dispQ->list )->l = ( theNode );} ( theNode )->n = 0;( &rol->dispQ->list 
)->c++;	if(( &rol->dispQ->list )->add_cmd != ((void *) 0) ) (*(( &rol->dispQ->list )->add_cmd)) (( 
&rol->dispQ->list )); } ; 	go_on = 0;
      }
    } else { 
      if ((1<<theSource) & evMasks[theType]) {
	currEvMask = theEvMask = evMasks[theType];
	currType = theType;
      } else {
        currEvMask = (1<<theSource);
      }
      if (wrapperGenerator) {
	(*wrapperGenerator)(theType);
      }
      (*(rol->nevents))++;
      intUnlock(intLockKey); ;
      (*theNode->reader)(theType, Tcode[theSource]);
      intLockKey = intLock(); ;
      if (theNode)
	{ if (( theNode )->part == 0) { free( theNode ); theNode = 0; } else { {if(! ( & theNode ->part->list 
)->c ){( & theNode ->part->list )->f = ( & theNode ->part->list )->l = ( theNode );( theNode )->p = 0;} 
else {( theNode )->p = ( & theNode ->part->list )->l;( & theNode ->part->list )->l->n = ( theNode );( & 
theNode ->part->list )->l = ( theNode );} ( theNode )->n = 0;( & theNode ->part->list )->c++;	if(( & 
theNode ->part->list )->add_cmd != ((void *) 0) ) (*(( & theNode ->part->list )->add_cmd)) (( & theNode 
->part->list )); } ; } if( theNode ->part->free_cmd != ((void *) 0) ) { (*( theNode ->part->free_cmd)) ( theNode 
->part->clientData); } } ;       if (theEvMask) {
	theEvMask = theEvMask & ~(1<<theSource);
      } 
      if (!theEvMask) {
	rol->dabufp = ((void *) 0) ; if (__the_event__) { if (rol->output) { {if(! ( 
&(rol->output->list) )->c ){( &(rol->output->list) )->f = ( &(rol->output->list) )->l = ( __the_event__ );( 
__the_event__ )->p = 0;} else {( __the_event__ )->p = ( &(rol->output->list) )->l;( &(rol->output->list) 
)->l->n = ( __the_event__ );( &(rol->output->list) )->l = ( __the_event__ );} ( __the_event__ )->n = 0;( 
&(rol->output->list) )->c++;	if(( &(rol->output->list) )->add_cmd != ((void *) 0) ) (*(( &(rol->output->list) 
)->add_cmd)) (( &(rol->output->list) )); } ; } else { { if (( __the_event__ )->part == 0) { free( __the_event__ 
); __the_event__ = 0; } else { {if(! ( & __the_event__ ->part->list )->c ){( & __the_event__ 
->part->list )->f = ( & __the_event__ ->part->list )->l = ( __the_event__ );( __the_event__ )->p = 0;} else {( 
__the_event__ )->p = ( & __the_event__ ->part->list )->l;( & __the_event__ ->part->list )->l->n = ( 
__the_event__ );( & __the_event__ ->part->list )->l = ( __the_event__ );} ( __the_event__ )->n = 0;( & 
__the_event__ ->part->list )->c++;	if(( & __the_event__ ->part->list )->add_cmd != ((void *) 0) ) (*(( & 
__the_event__ ->part->list )->add_cmd)) (( & __the_event__ ->part->list )); } ; } if( __the_event__ 
->part->free_cmd != ((void *) 0) ) { (*( __the_event__ ->part->free_cmd)) ( __the_event__ ->part->clientData); 
} } ;	} __the_event__ = (DANODE *) 0; } if(input_event__) { { if (( input_event__ )->part == 0) { 
free( input_event__ ); input_event__ = 0; } else { {if(! ( & input_event__ ->part->list )->c ){( & 
input_event__ ->part->list )->f = ( & input_event__ ->part->list )->l = ( input_event__ );( input_event__ 
)->p = 0;} else {( input_event__ )->p = ( & input_event__ ->part->list )->l;( & input_event__ 
->part->list )->l->n = ( input_event__ );( & input_event__ ->part->list )->l = ( input_event__ );} ( 
input_event__ )->n = 0;( & input_event__ ->part->list )->c++;	if(( & input_event__ ->part->list )->add_cmd 
!= ((void *) 0) ) (*(( & input_event__ ->part->list )->add_cmd)) (( & input_event__ ->part->list 
)); } ; } if( input_event__ ->part->free_cmd != ((void *) 0) ) { (*( input_event__ 
->part->free_cmd)) ( input_event__ ->part->clientData); } } ; input_event__ = (DANODE *) 0; } {int ix; if 
(currEvMask) {	for (ix=0; ix < trigId; ix++) {	if (currEvMask & (1<<ix)) (*doneRtns[ix])(); } if 
(rol->pool->list.c) { currEvMask = 0; __done(); rol->doDone = 0; } else { poolEmpty = 1; rol->doDone = 1; } } } ;       }
    }  
  }
  dispatch_busy = 0;
}
static int theIntHandler(int theSource)
{
  if (theSource == 0) return(0);
  {  
    DANODE *theNode;
    intLockKey = intLock(); ;
{{ ( theNode ) = 0; if (( &( rol->dispatch ->list) )->c){ ( &( rol->dispatch ->list) )->c--; ( 
theNode ) = ( &( rol->dispatch ->list) )->f; ( &( rol->dispatch ->list) )->f = ( &( rol->dispatch ->list) 
)->f->n; }; if (!( &( rol->dispatch ->list) )->c) { ( &( rol->dispatch ->list) )->l = 0; }} ;} ;     theNode->source = theSource;
    theNode->type = (*ttypeRtns[theSource])(Tcode[theSource]);
    theNode->reader = trigRtns[theSource]; 
{if(! ( &rol->dispQ->list )->c ){( &rol->dispQ->list )->f = ( &rol->dispQ->list )->l = ( 
theNode );( theNode )->p = 0;} else {( theNode )->p = ( &rol->dispQ->list )->l;( &rol->dispQ->list 
)->l->n = ( theNode );( &rol->dispQ->list )->l = ( theNode );} ( theNode )->n = 0;( &rol->dispQ->list 
)->c++;	if(( &rol->dispQ->list )->add_cmd != ((void *) 0) ) (*(( &rol->dispQ->list )->add_cmd)) (( 
&rol->dispQ->list )); } ;     if (!dispatch_busy)
      cdodispatch();
    intUnlock(intLockKey); ;
  }
}
static int cdopolldispatch()
{
  unsigned long theSource, theType;
  int stat = 0;
  DANODE *theNode;
  if (!poolEmpty) {
    for (theSource=1;theSource<trigId;theSource++){
      if (syncTRtns[theSource]){
	if ( theNode = (*syncTRtns[theSource])(Tcode[theSource])) {
	  stat = 1;
	  {  
	    intLockKey = intLock(); ;
	    if (theNode) 
	 {{ ( theNode ) = 0; if (( &( rol->dispatch ->list) )->c){ ( &( rol->dispatch ->list) )->c--; ( 
theNode ) = ( &( rol->dispatch ->list) )->f; ( &( rol->dispatch ->list) )->f = ( &( rol->dispatch ->list) 
)->f->n; }; if (!( &( rol->dispatch ->list) )->c) { ( &( rol->dispatch ->list) )->l = 0; }} ;} ; 	    theNode->source = theSource; 
	    theNode->type = (*ttypeRtns[theSource])(Tcode[theSource]); 
	    theNode->reader = trigRtns[theSource]; 
	 {if(! ( &rol->dispQ->list )->c ){( &rol->dispQ->list )->f = ( &rol->dispQ->list )->l = ( 
theNode );( theNode )->p = 0;} else {( theNode )->p = ( &rol->dispQ->list )->l;( &rol->dispQ->list 
)->l->n = ( theNode );( &rol->dispQ->list )->l = ( theNode );} ( theNode )->n = 0;( &rol->dispQ->list 
)->c++;	if(( &rol->dispQ->list )->add_cmd != ((void *) 0) ) (*(( &rol->dispQ->list )->add_cmd)) (( 
&rol->dispQ->list )); } ; 	    if (!dispatch_busy) 
	      cdodispatch();
	    intUnlock(intLockKey); ;
	  }
	}
      }
    }   
  } else {
    stat = -1;
  }
  return (stat);
}
/* # 99 "/adaqfs/coda/2.6.2/common/include/rol.h" 2 */
static char rol_name__[40];
static char temp_string__[132];
static void __poll()
{
    {cdopolldispatch();} ;
}
void RampTest__init (rolp)
     rolParam rolp;
{
      if ((rolp->daproc != 7 )&&(rolp->daproc != 6 )) 
	printf("rolp->daproc = %d\n",rolp->daproc);
      switch(rolp->daproc) {
      case 0 :
	{
	  char name[40];
	  rol = rolp;
	  rolp->inited = 1;
	  strcpy(rol_name__, "VME1" );
	  rolp->listName = rol_name__;
	  printf("Init - Initializing new rol structures for %s\n",rol_name__);
	  strcpy(name, rolp->listName);
	  strcat(name, ":pool");
	  rolp->pool  = partCreate(name, 8192  , 20000 ,1);
	  if (rolp->pool == 0) {
	    rolp->inited = -1;
	    break;
	  }
	  strcpy(name, rolp->listName);
	  strcat(name, ":dispatch");
	  rolp->dispatch  = partCreate(name, 0, 32, 0);
	  if (rolp->dispatch == 0) {
	    rolp->inited = -1;
	    break;
	  }
	  strcpy(name, rolp->listName);
	  strcat(name, ":dispQ");
	  rolp->dispQ = partCreate(name, 0, 0, 0);
	  if (rolp->dispQ == 0) {
	    rolp->inited = -1;
	    break;
	  }
	  rolp->inited = 1;
	  printf("Init - Done\n");
	  break;
	}
      case 9 :
	  rolp->inited = 0;
	break;
      case 1 :
	__download();
	break;
      case 2 :
	__prestart();
	break;
      case 4 :
	__pause();
	break;
      case 3 :
	__end();
	break;
      case 5 :
	__go();
	break;
      case 6 :
	__poll();
	break;
      case 7 :
	__done();
	break;
      default:
	printf("WARN: unsupported rol action = %d\n",rolp->daproc);
	break;
      }
}
/* # 7 "RampTest.c" 2 */
/* # 1 "/adaqfs/coda/2.6.2/common/include/VME_source.h" 1 */
static int VME_handlers,VMEflag;
static int VME_isAsync;
static unsigned long VME_prescale = 1;
static unsigned long VME_count = 0;
extern int sysBusToLocalAdrs(int space,long *localA,long **busA);
volatile struct vme_ts {
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
  } VME_TS;
struct vme_tir {
    unsigned short tir_csr;
    unsigned short tir_vec;
    unsigned short tir_dat;
    unsigned short tir_oport;
    unsigned short tir_iport;
  };
struct vme_scal {
    unsigned short reset;
    unsigned short blank1[7];
    unsigned short bim[8];
    unsigned short blank2[16];
    unsigned long preset[16];
    unsigned long scaler[16];
    unsigned short blank3[29];
    unsigned short id[3];
  };
volatile struct vme_ts  *ts;
struct vme_tir *tir[2];
struct vme_scal *vscal[32];
struct vme_scal *vlscal[32];
volatile unsigned long *tsmem;
unsigned long ts_memory[4096];
/* # 1 "/adaqfs/coda/2.6.2/common/include/tsUtil.h" 1 */
volatile struct vme_ts2 {
    volatile unsigned int csr;       
    volatile unsigned int csr2;       
    volatile unsigned int trig;
    volatile unsigned int roc;
    volatile unsigned int sync;
    volatile unsigned int trigCount;
    volatile unsigned int trigData;
    volatile unsigned int lrocData;
    volatile unsigned int prescale[8];
    volatile unsigned int timer[5];
    volatile unsigned int intVec;
    volatile unsigned int rocBufStatus;
    volatile unsigned int lrocBufStatus;
    volatile unsigned int rocAckStatus;
    volatile unsigned int userData[2];
    volatile unsigned int state;
    volatile unsigned int test;
    volatile unsigned int blank1;
    volatile unsigned int scalAssign;
    volatile unsigned int scalControl;
    volatile unsigned int scaler[18];
    volatile unsigned int scalEvent;
    volatile unsigned int scalLive[2];
    volatile unsigned int id;
  } VME_TS2;
struct ts_state {
  char go[12];
  char trigger[12];
  int ready;
  int busy;
  int seq;
  int sync;
  int feb;
  int inhibit;
  int clear;
  int l1e;
  char buffers[8];
  int branch[5];
  int strobe[5];
  int ack[5];
};    
int tsInit(unsigned int addr, int iflag);
int tsMemInit();
int tsLive(int sflag);
void tsLiveClear();
unsigned int tsCsr(unsigned int Val);
unsigned int tsCsr2Set(unsigned int cval);
unsigned int tsCsr2Clear(unsigned int cval);
void tsClearStatus();
unsigned int tsEnableInput(unsigned int Val, int tflag);
unsigned int tsRoc(unsigned char b4,unsigned char b3,unsigned char b2,unsigned char b1); unsigned int tsSync(unsigned int Val);
unsigned int tsPrescale(int Num, unsigned int Val);
unsigned int tsTimerWrite(int Num, unsigned int Val);
unsigned int tsMemWrite(int Num, unsigned int Val);
void tsGo(int iflag);
void tsStop(int iflag);
void tsReset(int iflag);
unsigned int tsStatus (int iflag);
void tsTimerStatus();
unsigned int tsState(int iflag);
unsigned int tsScalRead(int Num, int sflag);
void tsScalClear(unsigned int mask);
void tsScalLatch();
void tsScalClearAll();
/* # 125 "/adaqfs/coda/2.6.2/common/include/VME_source.h" 2 */
extern struct vme_ts2 *tsP;
/* # 1 "/adaqfs/coda/2.6.2/common/include/vme_triglib.h" 1 */
unsigned int tirVersion = 0;  
int
tirInit(unsigned int tAddr)
{
  int stat = 0;
  unsigned long laddr;
  unsigned short rval;
  if (tAddr == 0) {
    tAddr = 0x0ed0 ;
  }
  stat = sysBusToLocalAdrs(0x29,tAddr,&laddr);
  if (stat != 0) {
     printf("tirInit: ERROR: Error in sysBusToLocalAdrs res=%d \n",stat);
  } else {
     printf("TIR address = 0x%x\n",laddr);
     tir[1] = (struct vme_tir *)laddr;
  }
  stat = vxMemProbe((char *)laddr,0,2,(char *)&rval);
  if (stat != 0) {
    printf("tirInit: ERROR: TIR card not addressable\n");
    return(-1);
  } else {
    tir[1]->tir_csr = 0x80;  
    tirVersion = (unsigned int)rval;
    printf("tirInit: tirVersion = 0x%x\n",tirVersion);
  }
  return(0);
}
static inline void 
vmetriglink(int code, VOIDFUNCPTR isr)
{
  switch (code) {
  case 1 :
    if(tir[1]) {
      tir[1]->tir_csr = 0x80;
      tir[1]->tir_vec = 0xec ;
    }else{
      printf(" vmetriglink: ERROR: tir[1] uninitialized\n");
      return;
    }
    break;
  case 2 :
    if(tsP) {
      tsP->intVec = 0xec ;
    }else{
      printf(" vmetriglink: ERROR: ts uninitialized\n");
      return;
    }    
    break;
  default:
    printf(" vmetriglink: ERROR: source type %d undefined\n",code);
    return;
  }
  if((intDisconnect(((VOIDFUNCPTR *) ( 0xec  )) ) !=0))
     printf("Error disconnecting Interrupt\n");
  intConnect(((VOIDFUNCPTR *) ( 0xec  )) ,isr,0);
}
static inline void 
vmetenable(int code, unsigned int intMask)
{
 int lock_key;
  lock_key = intLock();
 if(code == 1 )
   sysIntEnable(5 );
 if(code == 2 ) {
   tsP->intVec = 0xec ;
   sysIntEnable(5 );
 }
  intEnable(11);          
 if(code == 1 )
   tir[1]->tir_csr = 0x6;
 if(code == 2 )
   tsP->csr2 |= 0x1800;
  intUnlock(lock_key);
/* # 189 "/adaqfs/coda/2.6.2/common/include/vme_triglib.h" */
}
static inline void 
vmetdisable(int code,unsigned int intMask)
{
 if(code == 1 )
   tir[1]->tir_csr = 0x80;
 if(code == 2 )
   tsP->csr2 &= ~(0x1800);
}
static inline void 
vmetack(int code, unsigned int intMask)
{
 if(code == 1 )
   tir[1]->tir_dat = 0x8000;
 if(code == 2 )
   tsP->lrocBufStatus = 0x100;
}
static inline unsigned long 
vmettype(int code)
{
 unsigned long tt;
 if(code == 1 ) {
   tt = (((tir[1]->tir_dat)&0x3c)>>2);
   syncFlag = (tir[1]->tir_dat)&0x01;
   lateFail = ((tir[1]->tir_dat)&0x02)>>1;
 }
 if(code == 2 ) {
   tt = (((tsP->lrocData)&0xfc)>>2);
   syncFlag = (tsP->lrocData)&0x01;
   lateFail = ((tsP->lrocData)&0x02)>>1;
 }
  return(tt);
}
static inline int 
vmettest(int code)
{
  unsigned short sval=0;
  unsigned int   lval=0;
 if(code == 1 ) {
   sval = tir[1]->tir_csr;
   if( (sval != 0xffff) && ((sval&0x8000) != 0) ) {
     return (1);
   } else {
     return (0);
   }
 }
 if(code == 2 ) {
   lval = tsP->lrocBufStatus;
   if( (lval != 0xffffffff) && ((lval&0x8000) != 0) ) {
     return (1);
   } else {
     return (0);
   }
 }
  return(0);
}
/* # 129 "/adaqfs/coda/2.6.2/common/include/VME_source.h" 2 */
void VME_int_handler()
{
  theIntHandler(VME_handlers);                    
}
/* # 8 "RampTest.c" 2 */
unsigned long READOUT_VQWK;
unsigned long firstEvents;
/* # 1 "../module_map.h" 1 */
extern int CODA_RUN_IN_PROGRESS;
extern int vxTicks;
extern unsigned long ncnt;
extern int ADC_NUMSLOTS;    
extern int showHAPADC();    
extern unsigned long* readoutHAPADC(unsigned long*, int);   
extern int setDACHAPADC(int, int);    
extern int usesDACHAPADC(int);  
extern int InitHAPTB();
extern int dumpRegHAPTB();
extern int getOverSampleCurrentHAPTB();
extern int getOverSampleHAPTB();
extern int getRampDelayHAPTB();
extern int getIntTimeHAPTB();
extern int getDACHAPTB();
extern int setDACHAPTB(int, int);
extern int setTimeHAPTB(int, int);
extern int setOverSampleHAPTB(int);
extern int cagetFFB(int);
extern int caputFFB(int,double);
extern double caget(int);
extern int caput(int,double);
extern int cagetFFB_modState();
extern int cagetFFB_waveState(int);
extern int cagetFFB_freq(int);
extern int cagetFFB_amp(int);
extern int cagetFFB_period(int);
extern int cagetFFB_load(int);
extern int cagetFFB_enterTrig(int);
extern int cagetFFB_leaveTrig(int);
extern int cagetFFB_trig(int);
extern int cagetFFB_relay(int);
extern int caputFFB_modState(int);
extern int caputFFB_freq(int,int);
extern int caputFFB_amp(int,int);
extern int caputFFB_period(int,int);
extern int caputFFB_load(int,int);
extern int caputFFB_enterTrig(int,int);
extern int caputFFB_leaveTrig(int,int);
extern int caputFFB_trig(int,int);
extern int caputFFB_relay(int,int);
extern void FIO_WriteChan(int chan, int data);
extern int  bmw_object; 
extern int  bmw_clean;
extern int  bmw_clean_pulse; 
extern int  bmw_freq; 
extern int  bmw_phase; 
extern int  bmw_period; 
extern int  bmw_phase_num; 
extern int  bmw_phase_cnt; 
extern int  bmw_cycle_number;
extern int  bmw_arm_trig;
extern int InitAUXTB();
extern int dumpRegAUXTB();
extern int getAUXFLAG();
extern int setAUXFLAG(int, int);
extern int getDataScan(int);
extern int getCleanScan();
extern int setDataScan(int,int);
extern int setCleanScan(int);
struct vme_flex_io
{  
  volatile unsigned short csr1;   
  volatile unsigned short data1;  
  volatile unsigned short csr2;   
  volatile unsigned short data2;  
  volatile unsigned short irq;    
};
struct SIS3800
{  
  volatile unsigned long csr             ;  
  volatile unsigned long IntVerR         ;  
  unsigned long empty0          ;  
  volatile unsigned long disCntR         ;  
  unsigned long empty1[4]       ;  
  volatile unsigned long clear           ;  
  volatile unsigned long vmeclk          ;  
  volatile unsigned long enclk           ;  
  volatile unsigned long disclk          ;  
  volatile unsigned long bclear          ;  
  volatile unsigned long bvmeclk         ;  
  volatile unsigned long benclk          ;  
  volatile unsigned long bdisclk         ;  
  volatile unsigned long clear1_8        ;  
  volatile unsigned long clear9_16       ;  
  volatile unsigned long clear17_24      ;  
  volatile unsigned long clear25_32      ;  
  volatile unsigned long gEncnt1         ;  
  volatile unsigned long gDiscnt1        ;  
  unsigned long empty2[2]       ;    
  volatile unsigned long reset           ;  
  unsigned long empty3          ;  
  volatile unsigned long testclkR        ;  
  unsigned long empty4[37]      ;  
  volatile unsigned long clrCount[32]    ;  
  volatile unsigned long clrOverCount[32];  
  volatile unsigned long readShadow[32]  ;  
  volatile unsigned long readCounter[32] ;  
  volatile unsigned long readClrCount[32];  
  volatile unsigned long readOver1_8     ;  
  unsigned long empty5[7]       ;  
  volatile unsigned long readOver9_16    ;  
  unsigned long empty6[7]       ;  
  volatile unsigned long readOver17_24   ;  
  unsigned long empty7[7]       ;  
  volatile unsigned long readOver25_32   ;  
};
struct fifo_scaler 
{  
  volatile unsigned long csr             ;  
  volatile unsigned long irq             ;  
  unsigned long empty0          ;  
  volatile unsigned long cdr             ;  
  volatile unsigned long wfifo           ;  
  unsigned long empty1          ;  
  unsigned long empty2          ;  
  unsigned long empty3          ;  
  volatile unsigned long clear           ;  
  volatile unsigned long next            ;  
  volatile unsigned long enable          ;  
  volatile unsigned long disable         ;  
  volatile unsigned long Bclear          ;  
  volatile unsigned long Bnext           ;  
  volatile unsigned long Benable         ;  
  volatile unsigned long Bdisable        ;  
  unsigned long empty4[4]       ;  
  volatile unsigned long gEncnt1         ;  
  volatile unsigned long gDiscnt1        ;  
  unsigned long empty5[2]       ;    
  volatile unsigned long reset           ;  
  unsigned long empty6          ;  
  volatile unsigned long test            ;  
  unsigned long empty7[37]      ;  
  volatile unsigned long fifo[64]        ;  
};
struct lecroy_1151_scaler {
  volatile unsigned short reset;
  volatile unsigned short blank1[7];
  volatile unsigned short bim[8];
  volatile unsigned short blank2[16];
  volatile unsigned long preset[16];
  volatile unsigned long scaler[16];
  volatile unsigned short blank3[29];
  volatile unsigned short id[3];
};
struct caen_v560_scaler {
  unsigned short empty1[2];  
  volatile unsigned short IntVectR ;  
  volatile unsigned short IntLevR  ;  
  volatile unsigned short EnIntR   ;  
  volatile unsigned short DisIntR  ;  
  volatile unsigned short ClrIntR  ;  
  volatile unsigned short RequestR ;  
  volatile unsigned long  CntR[16] ;  
  volatile unsigned short ClrR     ;  
  volatile unsigned short setVETOR ;  
  volatile unsigned short clrVETOR ;  
  volatile unsigned short incCntR  ;  
  volatile unsigned short StatusR  ;  
  unsigned short empty2[80];  
  volatile unsigned short FixCodeR ;  
  volatile unsigned short ModTypeR ;  
  volatile unsigned short VersionR ;  
};
/* # 28 "RampTest.c" 2 */
/* # 1 "usrstrutils.c" 1 */
char *internal_configusrstr=0;
char *file_configusrstr=0;
void getflagpos(char *s,char **pos_ret,char **val_ret);
int getflag(char *s)
{
  char *pos,*val;
  getflagpos(s,&pos,&val);
  if(!pos) return(0);
  if(!val) return(1);
  return(2);
}
char *getstr(char *s){
  char *pos,*val;
  char *end;
  char *ret;
  int slen;
  getflagpos(s,&pos,&val);
  if(!val){
    return(0);
  }
  end = strchr(val,',');	 
  if(end)
    slen = end - val;
  else				 
    slen = strlen(val);
  ret = (char *) malloc(slen+1);
  strncpy(ret,val,slen);
  ret[slen] = '\0';
  return(ret);
}
unsigned int getint(char *s)
{
  char *sval;
  int retval;
  sval = getstr(s);
  if(!sval) return(0);		 
  retval = strtol(sval,0,0);
  if(retval == 0x7FFFFFFF  && (sval[1]=='x' || sval[1]=='X')) { 
     sscanf(sval,"%x",&retval);
   }
  free(sval);
  return(retval);
}
void getflagpos_instring(char *constr, char *s,char **pos_ret,char **val_ret)
{
  int slen;
  char *pos,*val;
  slen=strlen(s);
  pos = constr;
  while(pos) {
    pos = strstr(pos,s);
    if(pos) {			 
      if((pos != constr && pos[-1] != ',') ||
	 (pos[slen] != '=' && pos[slen] != ',' && pos[slen] != '\0')) {
	pos += 1;	continue;
      } else break;		 
    }
  }
  *pos_ret = pos;
  if(pos) {
    if(pos[slen] == '=') {
      *val_ret = pos + slen + 1;
    } else 
      *val_ret = 0;
  } else
    *val_ret = 0;
  return;
}
void getflagpos(char *s,char **pos_ret,char **val_ret)
{
  getflagpos_instring(file_configusrstr,s,pos_ret,val_ret);
  if(*pos_ret) return;
  getflagpos_instring(rol->usrString,s,pos_ret,val_ret);
  if(*pos_ret) return;
  getflagpos_instring(internal_configusrstr,s,pos_ret,val_ret);
  return;
}
void init_strings()
{
  char *ffile_name;
  int fd;
  char s[256], *flag_line;
  if(!internal_configusrstr) {	 
    internal_configusrstr = (char *) malloc(strlen("" )+1);
    strcpy(internal_configusrstr,"" );
  }
  ffile_name = getstr("ffile" );
  fd = fopen(ffile_name,"r");
  if(!fd) {
    free(ffile_name);
    if(file_configusrstr) free(file_configusrstr);  
    file_configusrstr = (char *) malloc(1);
    file_configusrstr[0] = '\0';
  } else {
    flag_line = 0;
    while(fgets(s,255,fd)){
      char *arg;
      arg = strchr(s,';' );
      if(arg) *arg = '\0';  
      arg = s;			 
      while(*arg && isspace(*arg)){
	arg++;
      }
      if(*arg) {
	flag_line = arg;
	break;
      }
    }
    if(file_configusrstr) free(file_configusrstr);  
    if(flag_line) {		 
      file_configusrstr = (char *) malloc(strlen(flag_line)+1);
      strcpy(file_configusrstr,flag_line);
    } else {
      file_configusrstr = (char *) malloc(1);
      file_configusrstr[0] = '\0';
    }
    fclose(fd);
    free(ffile_name);
  }
}
/* # 29 "RampTest.c" 2 */
/* # 1 "tir_oport_utils.c" 1 */
int tir_oport_state;    
int clear_tir_oport(){
  tir_oport_state = 0;
  tir[1]->tir_oport = tir_oport_state;
  return tir_oport_state;
};
int read_tir_oport(){
  tir_oport_state = (tir[1]->tir_oport) & 0xFF;
  return tir_oport_state;
};
int set_tir_oport(int mask){
  tir_oport_state |= mask;
  tir[1]->tir_oport = tir_oport_state;
  return tir_oport_state;
};
int unset_tir_oport(int mask){
  tir_oport_state |= mask;
  tir_oport_state ^= mask;  
  tir[1]->tir_oport = tir_oport_state;
  return tir_oport_state;
};
int toggle_tir_oport(int mask){
  tir_oport_state ^= mask;  
  tir[1]->tir_oport = tir_oport_state;
  return tir_oport_state;
};
/* # 30 "RampTest.c" 2 */
/* # 1 "/adaqfs/home/apar/scaler/SISnohel/SIS.h" 1 */
struct SISbuffer{
  long numread;
  long hbit;
  long upbit;
  long qrt;
  long data[32 ];
};
struct SISring{
  long clock;
  long qrt_hel;   
  long data[6 ];
};
int ring_map[6 ];   
/* # 31 "RampTest.c" 2 */
int numberSteps12=0 ;
int numberSteps16=0 ;
int totalSteps12=0 ;
int totalSteps16=0 ;
int gainStep=0 ;
static int BAD18DATA = -999;
static int conv_gain[25 ],int_gain[25 ];
int oport_state=0;
static int noscaler, scaler2, bmwscan, tbreadout, evtypefix, flexio, flexio2;
static int qweakadc, adc18bit, numadc18, adc16bit, mtread, rampdac12, rampdac16;
static int autoled, lefthrs, righthrs, counthouse;
unsigned short ranny[2000000 ];
extern int bigendian_out;
extern int rocActivate;
extern struct SISbuffer SIS1;
static FUNCPTR handlerPtr;
VOIDFUNCPTR isr;
volatile struct vme_flex_io *flex_io;
volatile struct fifo_scaler *pscaler[2 ];
unsigned int addr_vqwk;
unsigned int addrinc_vqwk  = 0x010000;
unsigned int baseaddr_vqwk = 0x840000;
unsigned int nvqwk = 0;
static void __download()
{
    daLogMsg("INFO","Readout list compiled %s", DAYTIME);
    *(rol->async_roc) = 0;  
  {   
unsigned long iadc;
{ 
  int i,j,tmode,mode,res;
  unsigned short temp;
  unsigned long laddr;
  extern void srand();
  extern int rand();
  int chkcsr, retry;
  CODA_RUN_IN_PROGRESS = 0;
  if (1 ) {
      printf("Assuming EB is on a Little-Endian cpu\n");      
      bigendian_out = 0;
  }
init_strings();
noscaler=0;scaler2=0;bmwscan=0;tbreadout=0;evtypefix=0;flexio=0,flexio2=0;
qweakadc=0; adc18bit=0;adc16bit=0;mtread=0;rampdac12=0;rampdac16=0; autoled=0; 
lefthrs=0; righthrs=0;
counthouse=0;
noscaler  = getflag("noscaler" );    
scaler2   = getflag("scaler2" );     
bmwscan   = getflag("bmwscan" );     
tbreadout = getflag("tbreadout" );   
evtypefix = getflag("evtypefix" );   
flexio    = getflag("flexio" );      
flexio2   = getflag("flexio2" );     
qweakadc  = getflag("qweakadc" );    
adc18bit  = getflag("adc18bit" );    
adc16bit  = getflag("adc16bit" );    
mtread    = getflag("mtread" );      
rampdac12 = getflag("rampdac12" );   
rampdac16 = getflag("rampdac16" );   
autoled   = getflag("autoled" );     
lefthrs   = getflag("lefthrs" );     
righthrs  = getflag("righthrs" );    
counthouse = getflag("counthouse" );  
printf("*** User String Configuration ***\n");
if (lefthrs) {
  printf("This is the LEFT HRS crate\n");
} 
if (righthrs) {
  printf("This is the RIGHT HRS crate\n");
} 
if (counthouse) {
  printf("This is the counting house crate \n");
}
if (qweakadc) {
  printf("Will use the Qweak ADCs \n");
}
if (adc18bit) {
  printf("Will use 18-bit ADC \n");
} 
if (adc16bit) {
  printf("Will use 16-bit ADC \n");
} 
if(noscaler == 0) {
  printf("WILL readout scaler: %d\n",noscaler);
} else {
  printf("Will NOT readout scaler: %d\n",noscaler);
}
if(scaler2 == 0) {
  printf("Will NOT readout second scaler\n");
} else {
  printf("WILL readout second scaler\n");
}
if(tbreadout==1) {
  printf("WILL write timing board config to file: %d\n",tbreadout);
} else {
  printf("Will NOT write timing board config to file: %d\n",tbreadout);
}
if(evtypefix) {
  printf("WILL use EVTYPE fix for Injector DAQ: %d\n",evtypefix);
}
if(flexio==1) {
  printf("WILL readout Flexible IO Module: %d\n",flexio);
} else {
  printf("Will NOT readout Flexible IO Module: %d\n",flexio);
}
if(flexio2==1) {
  printf("WILL readout Flexible IO Module2: %d\n",flexio2);
} else {
  printf("Will NOT readout Flexible IO Module2: %d\n",flexio2);
}
if (autoled) {
  printf("Will check for auto_led cycles \n");
  rampdac12 = 0;   
  rampdac16 = 0;
  bmwscan = 1;   
 }
bmwscan=1;
if(bmwscan==1) {
  printf("WILL readout Beam Modulation and Scan Words: %d\n",bmwscan);
} else {
  printf("Will NOT readout Beam Modulation and Words: %d\n",bmwscan);
}
if (rampdac12) printf("Will ramp the 12-bit DAC from %d to %d \n",500 ,8500 );
if (rampdac16) printf("Will ramp the 16-bit DAC from %d to %d \n",100 ,64100 );
tirInit(0x0ad0 );
if(flexio == 1) {
  res = sysBusToLocalAdrs(0x29,0x0ed0 ,&laddr);
  if (res != 0) {
    printf("Error in sysBusToLocalAdrs res=%d \n",res);
    printf("ERROR:  FlexIO POINTER NOT INITIALIZED");
  } else {
    printf("FlexIO address = 0x%x\n",laddr);
    flex_io = (struct vme_flex_io *)laddr;
  }
  flex_io->csr1 = 0x8000;  
  flex_io->csr1 = 0x3;     
}
if(flexio2 == 1) {
  res = sysBusToLocalAdrs(0x29,0x0ed0 ,&laddr);
  if (res != 0) {
    printf("Error in sysBusToLocalAdrs res=%d \n",res);
    printf("ERROR:  FlexIO2 POINTER NOT INITIALIZED");
  } else {
    printf("FlexIO2 address = 0x%x\n",laddr);
    flex_io = (struct vme_flex_io *)laddr;
  }
  flex_io->csr2 = 0x8000;  
  flex_io->csr2 = 0x3;     
}
  if (adc16bit) {
    showHAPADC(-1);
  }
if(noscaler == 0) {
  res = sysBusToLocalAdrs(0x39,0xa40000 ,&laddr);
  if (res != 0) {
    printf("Error in sysBusToLocalAdrs res=%d \n",res);
    printf("ERROR:  SCALER POINTER NOT INITIALIZED");
  } else {
    printf("SIS3801 Scaler address = 0x%x\n",laddr);
    pscaler[0] = (struct fifo_scaler *)laddr;
  }
  pscaler[0]->reset=1;
  pscaler[0]->csr=0xff00fc00;
  pscaler[0]->clear=1;
  pscaler[0]->enable=1;
  mode = 2;
  tmode = mode << 2;
  pscaler[0]->csr=0x00000C00;   
  pscaler[0]->csr=tmode;        
  pscaler[0]->csr=0x10000;      
  pscaler[0]->csr=0x40000;      
  if(scaler2==1) {
    res = sysBusToLocalAdrs(0x39,0xa50000 ,&laddr);
    if (res != 0) {
      printf("Error in sysBusToLocalAdrs res=%d \n",res);
      printf("ERROR:  SCALER POINTER NOT INITIALIZED");
    } else {
      printf("SIS3801 Scaler address = 0x%x\n",laddr);
      pscaler[1] = (struct fifo_scaler *)laddr;
    }
    pscaler[1]->reset=1;
    pscaler[1]->csr=0xff00fc00;
    pscaler[1]->clear=1;
    pscaler[1]->enable=1;
    mode = 2;
    tmode = mode << 2;
    pscaler[1]->csr=0x00000C00;   
    pscaler[1]->csr=tmode;        
    pscaler[1]->csr=0x10000;      
    pscaler[1]->csr=0x40000;      
  }
/* # 333 "RampTest.c" */
}
srand(9421);        
 tir[1]->tir_csr = 0x80;
 } 
    { VME_handlers =0;VME_isAsync = 0;VMEflag = 0;} ;
{ 
oport_state = 0;   
tir[1]->tir_oport=oport_state;
   printf("Setting HAPTB and Oversampling"); 
   dumpRegHAPTB();
 } 
    setDACHAPTB(1,500 );
    setDACHAPTB(2,32100 );
    daLogMsg("INFO","User Download Executed");
  }   
}       
static void __prestart()
{
{ dispatch_busy = 0; bzero((char *) evMasks, sizeof(evMasks)); bzero((char *) syncTRtns, 
sizeof(syncTRtns)); bzero((char *) ttypeRtns, sizeof(ttypeRtns)); bzero((char *) Tcode, sizeof(Tcode)); 
wrapperGenerator = 0; theEvMask = 0; currEvMask = 0; trigId = 1; poolEmpty = 0; __the_event__ = (DANODE *) 0; 
input_event__ = (DANODE *) 0; } ;     *(rol->nevents) = 0;
  {   
    daLogMsg("INFO","Entering User Prestart");
{ void partrig ();void partrig_done (); doneRtns[trigId] = (FUNCPTR) ( partrig_done ) ; 
trigRtns[trigId] = (FUNCPTR) ( partrig ) ; Tcode[trigId] = ( 1 ) ; ttypeRtns[trigId] = vmettype ; {printf("Linking async VME trigger to id %d 
\n", trigId ); VME_handlers = ( trigId );VME_isAsync = 1;vmetriglink( 1 ,VME_int_handler);} 
;trigId++;} ;     {evMasks[ 1 ] |= (1<<( VME_handlers ));} ;
{ 
  int j, iadc;
  unsigned short tmp_rand;
  int ped_test;
  int fdbfile;
  int bad_randmake=0;
  extern void srand();
  extern int rand();
  CODA_RUN_IN_PROGRESS = 0;
if (adc16bit) {
  printf("Generating Random Integer Array\n");
  printf("Number of Array elements = %d\n",2000000 );
  srand(ncnt);   
  bad_randmake=0;
  for (j=0;j< 2000000 ;j++) {
    tmp_rand = (unsigned short) (61440.0*rand()/(32768+1.0));
    while(tmp_rand==0 || tmp_rand==-1) {
      bad_randmake=1;
      tmp_rand = (unsigned short) (61440.0*rand()/(32768+1.0));
    }
    ranny[j] = 2048 + tmp_rand;
  }
  if(bad_randmake==1) {
    int badfile;
    badfile=fopen("~apar/bryan/badrand/badrand.hist","a");
    fprintf(badfile,"Another bad random number generation at ncnt=%d\n",
	  ncnt);
    fclose(badfile);
  }
}
  if(tbreadout==1) {
    fdbfile=fopen("~apar/db/timebrd/timebrd.cfg","w");
fprintf(fdbfile,"# Timing board information:\n# Integrate Time = %.2f ms, Ramp Delay = %.2f us\n\ \ \ \ oversamp %d 
", 	    (double)getIntTimeHAPTB()*0.0025,
	    (double)getRampDelayHAPTB()*2.5,
	    (int)getOverSampleHAPTB()+1);
    fclose(fdbfile);
  }
 numadc18 = 0; 
 if (adc18bit) { 
     numadc18 = adc18_getNum(); 
      for (iadc = 0; iadc < numadc18; iadc++) { 
         conv_gain[iadc] = adc18_getconv(iadc);
         int_gain[iadc] = adc18_getintgain(iadc);
     }
      adc18_zeromem();
      adc18_initall();    
      taskDelay(2*60); 
      for (iadc = 0; iadc < numadc18; iadc++) { 
         printf("Hello !  Setting up ADC18 # %d \n",iadc); 
         adc18_stop(iadc); 
         adc18_timesrc(iadc,0,0,0);   
         adc18_gatemode(iadc,1);             
         adc18_intgain(iadc,3);       
         adc18_setconv(iadc,3);     
         adc18_setsample(iadc,1); 
         adc18_loaddac(iadc,0);  
         adc18_flush(iadc); 
      } 
      if (righthrs) { 
         printf("Setting ADC18 pedestals on R-HRS \n"); 
         adc18_setpedestal(0,1821,1547,1769,1581); 
         adc18_setpedestal(1,1446,1197,1935,1571); 
      }else if (counthouse) {     
         printf("Setting ADC18 pedestals in counting room crate \n"); 
         adc18_setpedestal(0,1800,1000,1500,1000);
         adc18_setpedestal(1,1800,1400,1500,1800);
         adc18_setpedestal(2,1700,1300,1800,1600);  
         adc18_setpedestal(3,1800,2000,1800,2000);  
      } 
 }    
  oport_state = 0;   
  tir[1]->tir_oport=oport_state;
 } 
{ 
if (counthouse && qweakadc) nvqwk=5;
if (righthrs && qweakadc) nvqwk=2;
if (lefthrs && qweakadc) nvqwk=1;
printf("Lefthrs %d qweakadc %d Number of Qweak ADCs %d\n",lefthrs,qweakadc,nvqwk); if (nvqwk==0){
  READOUT_VQWK = 0;
} else {
  READOUT_VQWK = 1;
}
if (!qweakadc) READOUT_VQWK=0;
 } 
    daLogMsg("INFO","Begin setting up the VQWK modules.");
if(( READOUT_VQWK == 1) ) {
   firstEvents = 2;
   addr_vqwk = baseaddr_vqwk;
    daLogMsg("INFO","Begin setting up the VQWK modules.");
logMsg("Set up %d ADCs beginning with ladd=0x%8x with address steps of 
0x%8x",nvqwk,addr_vqwk,addrinc_vqwk,0,0,0); { 
   rol->dabufp = (long *) 0;
{	{if(__user_event__ == (DANODE *) 0) { {{ ( __user_event__ ) = 0; if (( &( rol->pool ->list) 
)->c){ ( &( rol->pool ->list) )->c--; ( __user_event__ ) = ( &( rol->pool ->list) )->f; ( &( rol->pool 
->list) )->f = ( &( rol->pool ->list) )->f->n; }; if (!( &( rol->pool ->list) )->c) { ( &( rol->pool ->list) 
)->l = 0; }} ;} ; if(__user_event__ == (DANODE *) 0) { logMsg ("TRIG ERROR: no pool buffer 
available\n"); return; } rol->dabufp = (long *) &__user_event__->length; __user_event__->nevent = -1; } } ; 
StartOfUEvent = (rol->dabufp); *(++(rol->dabufp)) = ((( (unsigned int)(0x90 +rol->pid) ) << 16) | ( 0x01 ) << 8) 
| (0xff & 0 );	((rol->dabufp))++;} ;    if (rol->dabufp){
     int id=0;
     while (id<nvqwk){
       rol->dabufp += vqwkDumpStatus(id,rol->dabufp);
       id++;
     }
   }else {
     logMsg("No buffer, ROC configuration event not written",0,0,0,0,0,0);
   }
{ *StartOfUEvent = (long) (((char *) (rol->dabufp)) - ((char *) StartOfUEvent));	if 
((*StartOfUEvent & 1) != 0) { (rol->dabufp) = ((long *)((char *) (rol->dabufp))+1); *StartOfUEvent += 1; }; if 
((*StartOfUEvent & 2) !=0) { *StartOfUEvent = *StartOfUEvent + 2; (rol->dabufp) = ((long *)((short *) 
(rol->dabufp))+1);; };	*StartOfUEvent = ( (*StartOfUEvent) >> 2) - 1; if (rol->output) { {if(! ( 
&(rol->output->list) )->c ){( &(rol->output->list) )->f = ( &(rol->output->list) )->l = ( __user_event__ );( 
__user_event__ )->p = 0;} else {( __user_event__ )->p = ( &(rol->output->list) )->l;( &(rol->output->list) 
)->l->n = ( __user_event__ );( &(rol->output->list) )->l = ( __user_event__ );} ( __user_event__ )->n = 
0;( &(rol->output->list) )->c++;	if(( &(rol->output->list) )->add_cmd != ((void *) 0) ) (*(( 
&(rol->output->list) )->add_cmd)) (( &(rol->output->list) )); } ; } else { { if (( __user_event__ )->part == 0) { free( 
__user_event__ ); __user_event__ = 0; } else { {if(! ( & __user_event__ ->part->list )->c ){( & __user_event__ 
->part->list )->f = ( & __user_event__ ->part->list )->l = ( __user_event__ );( __user_event__ )->p = 0;} else 
{( __user_event__ )->p = ( & __user_event__ ->part->list )->l;( & __user_event__ ->part->list 
)->l->n = ( __user_event__ );( & __user_event__ ->part->list )->l = ( __user_event__ );} ( 
__user_event__ )->n = 0;( & __user_event__ ->part->list )->c++;	if(( & __user_event__ ->part->list 
)->add_cmd != ((void *) 0) ) (*(( & __user_event__ ->part->list )->add_cmd)) (( & __user_event__ 
->part->list )); } ; } if( __user_event__ ->part->free_cmd != ((void *) 0) ) { (*( __user_event__ 
->part->free_cmd)) ( __user_event__ ->part->clientData); } } ;	} __user_event__ = (DANODE *) 0; }; ;  } 
    daLogMsg("INFO","VQWK ADCs initialized");
} 
    daLogMsg("INFO","User Prestart Executed");
  }   
if (__the_event__) rol->dabufp = ((void *) 0) ; if (__the_event__) { if (rol->output) { {if(! ( 
&(rol->output->list) )->c ){( &(rol->output->list) )->f = ( &(rol->output->list) )->l = ( __the_event__ );( 
__the_event__ )->p = 0;} else {( __the_event__ )->p = ( &(rol->output->list) )->l;( &(rol->output->list) 
)->l->n = ( __the_event__ );( &(rol->output->list) )->l = ( __the_event__ );} ( __the_event__ )->n = 0;( 
&(rol->output->list) )->c++;	if(( &(rol->output->list) )->add_cmd != ((void *) 0) ) (*(( &(rol->output->list) 
)->add_cmd)) (( &(rol->output->list) )); } ; } else { { if (( __the_event__ )->part == 0) { free( __the_event__ 
); __the_event__ = 0; } else { {if(! ( & __the_event__ ->part->list )->c ){( & __the_event__ 
->part->list )->f = ( & __the_event__ ->part->list )->l = ( __the_event__ );( __the_event__ )->p = 0;} else {( 
__the_event__ )->p = ( & __the_event__ ->part->list )->l;( & __the_event__ ->part->list )->l->n = ( 
__the_event__ );( & __the_event__ ->part->list )->l = ( __the_event__ );} ( __the_event__ )->n = 0;( & 
__the_event__ ->part->list )->c++;	if(( & __the_event__ ->part->list )->add_cmd != ((void *) 0) ) (*(( & 
__the_event__ ->part->list )->add_cmd)) (( & __the_event__ ->part->list )); } ; } if( __the_event__ 
->part->free_cmd != ((void *) 0) ) { (*( __the_event__ ->part->free_cmd)) ( __the_event__ ->part->clientData); 
} } ;	} __the_event__ = (DANODE *) 0; } if(input_event__) { { if (( input_event__ )->part == 0) { 
free( input_event__ ); input_event__ = 0; } else { {if(! ( & input_event__ ->part->list )->c ){( & 
input_event__ ->part->list )->f = ( & input_event__ ->part->list )->l = ( input_event__ );( input_event__ 
)->p = 0;} else {( input_event__ )->p = ( & input_event__ ->part->list )->l;( & input_event__ 
->part->list )->l->n = ( input_event__ );( & input_event__ ->part->list )->l = ( input_event__ );} ( 
input_event__ )->n = 0;( & input_event__ ->part->list )->c++;	if(( & input_event__ ->part->list )->add_cmd 
!= ((void *) 0) ) (*(( & input_event__ ->part->list )->add_cmd)) (( & input_event__ ->part->list 
)); } ; } if( input_event__ ->part->free_cmd != ((void *) 0) ) { (*( input_event__ 
->part->free_cmd)) ( input_event__ ->part->clientData); } } ; input_event__ = (DANODE *) 0; } {int ix; if 
(currEvMask) {	for (ix=0; ix < trigId; ix++) {	if (currEvMask & (1<<ix)) (*doneRtns[ix])(); } if 
(rol->pool->list.c) { currEvMask = 0; __done(); rol->doDone = 0; } else { poolEmpty = 1; rol->doDone = 1; } } } ;     *(rol->nevents) = 0;
    rol->recNb = 0;
}       
static void __end()
{
  {   
  int iadc;
   vmetdisable(  1  ,  0  );  ;
  rocActivate=0;
{ 
  if (adc18bit) {
     for (iadc = 0; iadc < numadc18; iadc++) {
       adc18_stop(iadc);   
     }
  }
 } 
   unset_tir_oport(0x04 );
  CODA_RUN_IN_PROGRESS = 0;
    daLogMsg("INFO","User End Executed");
  }   
if (__the_event__) rol->dabufp = ((void *) 0) ; if (__the_event__) { if (rol->output) { {if(! ( 
&(rol->output->list) )->c ){( &(rol->output->list) )->f = ( &(rol->output->list) )->l = ( __the_event__ );( 
__the_event__ )->p = 0;} else {( __the_event__ )->p = ( &(rol->output->list) )->l;( &(rol->output->list) 
)->l->n = ( __the_event__ );( &(rol->output->list) )->l = ( __the_event__ );} ( __the_event__ )->n = 0;( 
&(rol->output->list) )->c++;	if(( &(rol->output->list) )->add_cmd != ((void *) 0) ) (*(( &(rol->output->list) 
)->add_cmd)) (( &(rol->output->list) )); } ; } else { { if (( __the_event__ )->part == 0) { free( __the_event__ 
); __the_event__ = 0; } else { {if(! ( & __the_event__ ->part->list )->c ){( & __the_event__ 
->part->list )->f = ( & __the_event__ ->part->list )->l = ( __the_event__ );( __the_event__ )->p = 0;} else {( 
__the_event__ )->p = ( & __the_event__ ->part->list )->l;( & __the_event__ ->part->list )->l->n = ( 
__the_event__ );( & __the_event__ ->part->list )->l = ( __the_event__ );} ( __the_event__ )->n = 0;( & 
__the_event__ ->part->list )->c++;	if(( & __the_event__ ->part->list )->add_cmd != ((void *) 0) ) (*(( & 
__the_event__ ->part->list )->add_cmd)) (( & __the_event__ ->part->list )); } ; } if( __the_event__ 
->part->free_cmd != ((void *) 0) ) { (*( __the_event__ ->part->free_cmd)) ( __the_event__ ->part->clientData); 
} } ;	} __the_event__ = (DANODE *) 0; } if(input_event__) { { if (( input_event__ )->part == 0) { 
free( input_event__ ); input_event__ = 0; } else { {if(! ( & input_event__ ->part->list )->c ){( & 
input_event__ ->part->list )->f = ( & input_event__ ->part->list )->l = ( input_event__ );( input_event__ 
)->p = 0;} else {( input_event__ )->p = ( & input_event__ ->part->list )->l;( & input_event__ 
->part->list )->l->n = ( input_event__ );( & input_event__ ->part->list )->l = ( input_event__ );} ( 
input_event__ )->n = 0;( & input_event__ ->part->list )->c++;	if(( & input_event__ ->part->list )->add_cmd 
!= ((void *) 0) ) (*(( & input_event__ ->part->list )->add_cmd)) (( & input_event__ ->part->list 
)); } ; } if( input_event__ ->part->free_cmd != ((void *) 0) ) { (*( input_event__ 
->part->free_cmd)) ( input_event__ ->part->clientData); } } ; input_event__ = (DANODE *) 0; } {int ix; if 
(currEvMask) {	for (ix=0; ix < trigId; ix++) {	if (currEvMask & (1<<ix)) (*doneRtns[ix])(); } if 
(rol->pool->list.c) { currEvMask = 0; __done(); rol->doDone = 0; } else { poolEmpty = 1; rol->doDone = 1; } } } ; }  
static void __pause()
{
  {   
   vmetdisable(  1  ,  0  );  ;
    daLogMsg("INFO","User Pause Executed");
  }   
if (__the_event__) rol->dabufp = ((void *) 0) ; if (__the_event__) { if (rol->output) { {if(! ( 
&(rol->output->list) )->c ){( &(rol->output->list) )->f = ( &(rol->output->list) )->l = ( __the_event__ );( 
__the_event__ )->p = 0;} else {( __the_event__ )->p = ( &(rol->output->list) )->l;( &(rol->output->list) 
)->l->n = ( __the_event__ );( &(rol->output->list) )->l = ( __the_event__ );} ( __the_event__ )->n = 0;( 
&(rol->output->list) )->c++;	if(( &(rol->output->list) )->add_cmd != ((void *) 0) ) (*(( &(rol->output->list) 
)->add_cmd)) (( &(rol->output->list) )); } ; } else { { if (( __the_event__ )->part == 0) { free( __the_event__ 
); __the_event__ = 0; } else { {if(! ( & __the_event__ ->part->list )->c ){( & __the_event__ 
->part->list )->f = ( & __the_event__ ->part->list )->l = ( __the_event__ );( __the_event__ )->p = 0;} else {( 
__the_event__ )->p = ( & __the_event__ ->part->list )->l;( & __the_event__ ->part->list )->l->n = ( 
__the_event__ );( & __the_event__ ->part->list )->l = ( __the_event__ );} ( __the_event__ )->n = 0;( & 
__the_event__ ->part->list )->c++;	if(( & __the_event__ ->part->list )->add_cmd != ((void *) 0) ) (*(( & 
__the_event__ ->part->list )->add_cmd)) (( & __the_event__ ->part->list )); } ; } if( __the_event__ 
->part->free_cmd != ((void *) 0) ) { (*( __the_event__ ->part->free_cmd)) ( __the_event__ ->part->clientData); 
} } ;	} __the_event__ = (DANODE *) 0; } if(input_event__) { { if (( input_event__ )->part == 0) { 
free( input_event__ ); input_event__ = 0; } else { {if(! ( & input_event__ ->part->list )->c ){( & 
input_event__ ->part->list )->f = ( & input_event__ ->part->list )->l = ( input_event__ );( input_event__ 
)->p = 0;} else {( input_event__ )->p = ( & input_event__ ->part->list )->l;( & input_event__ 
->part->list )->l->n = ( input_event__ );( & input_event__ ->part->list )->l = ( input_event__ );} ( 
input_event__ )->n = 0;( & input_event__ ->part->list )->c++;	if(( & input_event__ ->part->list )->add_cmd 
!= ((void *) 0) ) (*(( & input_event__ ->part->list )->add_cmd)) (( & input_event__ ->part->list 
)); } ; } if( input_event__ ->part->free_cmd != ((void *) 0) ) { (*( input_event__ 
->part->free_cmd)) ( input_event__ ->part->clientData); } } ; input_event__ = (DANODE *) 0; } {int ix; if 
(currEvMask) {	for (ix=0; ix < trigId; ix++) {	if (currEvMask & (1<<ix)) (*doneRtns[ix])(); } if 
(rol->pool->list.c) { currEvMask = 0; __done(); rol->doDone = 0; } else { poolEmpty = 1; rol->doDone = 1; } } } ; }  
static void __go()
{
  {   
    daLogMsg("INFO","Entering User Go Go (bob)");
{ 
  int i, iadc, nscaler, timeout, didreadsca;
  int nmax, nloop;
  unsigned short temp;
  rocActivate=1;
set_tir_oport(0x02 );
if(noscaler == 0) {  
  if(scaler2==1) {
    nscaler = 2;
  } else {
    nscaler = 1;
  }
  for (i=0; i<nscaler; i++) {
    taskDelay(2);    
    taskDelay(2);   
    pscaler[i]->clear = 1;
    taskDelay(2);   
    pscaler[i]->next = 1;   
    taskDelay(2);   
    pscaler[i]->csr = 0x10000;  
  }
  if (counthouse) {  
    nmax = 100;
    for (i = 0; i <nscaler; i++) {
      for (nloop = 0; nloop < nmax; nloop++) {
        didreadsca = 0;
        timeout = 0;
        while ( (pscaler[i]->csr& 0x100 ) != 0x100  && timeout++ < 10) {
           didreadsca = 1;
        }
        if (didreadsca == 0) {  
            pscaler[i]->clear = 1;
            pscaler[i]->next = 1;   
            pscaler[i]->csr = 0x10000;  
	} else {
	    goto okscaler1;
	}
      }
okscaler1:
      logMsg("Scaler %d cleared %d times \n",i,nloop,0,0,0,0);
    }
  }
}
 unset_tir_oport(0x02 );
if(flexio == 1) {
  flex_io->csr1 = 0x100;  
  flex_io->csr1 = 0x03;  
}
if(flexio2 == 1) {
  flex_io->csr2 = 0x100;  
  flex_io->csr2 = 0x03;  
}
if (adc18bit) for (iadc = 0; iadc<numadc18; iadc++) {
  printf("Flushing adc18 number %d\n",iadc);
  adc18_flush(iadc);
}
set_tir_oport(0x04 );
 } 
if(( READOUT_VQWK == 1) ) {
    daLogMsg("INFO","Clear the ADCs during go");
{ 
  int id=0;
  int gateclkmode = 2 ;
  int gateflag = (gateclkmode & 2);
  int clkflag  = (gateclkmode & 1);
  logMsg("Setting gateflag to %d and clockflag to %d.\n",
	 gateflag, clkflag,0,0,0,0);
  while (id < nvqwk){
    vqwkSetGateClockSources(id,gateflag,clkflag);
    id++;
  }
  id=0;
  while (id < nvqwk){
    vqwkClearAllChannels(id);
    id++;
  }
 } 
} 
{ 
   vmetenable(  1  ,  0  );  ;
  CODA_RUN_IN_PROGRESS = 1;
 } 
  }   
if (__the_event__) rol->dabufp = ((void *) 0) ; if (__the_event__) { if (rol->output) { {if(! ( 
&(rol->output->list) )->c ){( &(rol->output->list) )->f = ( &(rol->output->list) )->l = ( __the_event__ );( 
__the_event__ )->p = 0;} else {( __the_event__ )->p = ( &(rol->output->list) )->l;( &(rol->output->list) 
)->l->n = ( __the_event__ );( &(rol->output->list) )->l = ( __the_event__ );} ( __the_event__ )->n = 0;( 
&(rol->output->list) )->c++;	if(( &(rol->output->list) )->add_cmd != ((void *) 0) ) (*(( &(rol->output->list) 
)->add_cmd)) (( &(rol->output->list) )); } ; } else { { if (( __the_event__ )->part == 0) { free( __the_event__ 
); __the_event__ = 0; } else { {if(! ( & __the_event__ ->part->list )->c ){( & __the_event__ 
->part->list )->f = ( & __the_event__ ->part->list )->l = ( __the_event__ );( __the_event__ )->p = 0;} else {( 
__the_event__ )->p = ( & __the_event__ ->part->list )->l;( & __the_event__ ->part->list )->l->n = ( 
__the_event__ );( & __the_event__ ->part->list )->l = ( __the_event__ );} ( __the_event__ )->n = 0;( & 
__the_event__ ->part->list )->c++;	if(( & __the_event__ ->part->list )->add_cmd != ((void *) 0) ) (*(( & 
__the_event__ ->part->list )->add_cmd)) (( & __the_event__ ->part->list )); } ; } if( __the_event__ 
->part->free_cmd != ((void *) 0) ) { (*( __the_event__ ->part->free_cmd)) ( __the_event__ ->part->clientData); 
} } ;	} __the_event__ = (DANODE *) 0; } if(input_event__) { { if (( input_event__ )->part == 0) { 
free( input_event__ ); input_event__ = 0; } else { {if(! ( & input_event__ ->part->list )->c ){( & 
input_event__ ->part->list )->f = ( & input_event__ ->part->list )->l = ( input_event__ );( input_event__ 
)->p = 0;} else {( input_event__ )->p = ( & input_event__ ->part->list )->l;( & input_event__ 
->part->list )->l->n = ( input_event__ );( & input_event__ ->part->list )->l = ( input_event__ );} ( 
input_event__ )->n = 0;( & input_event__ ->part->list )->c++;	if(( & input_event__ ->part->list )->add_cmd 
!= ((void *) 0) ) (*(( & input_event__ ->part->list )->add_cmd)) (( & input_event__ ->part->list 
)); } ; } if( input_event__ ->part->free_cmd != ((void *) 0) ) { (*( input_event__ 
->part->free_cmd)) ( input_event__ ->part->clientData); } } ; input_event__ = (DANODE *) 0; } {int ix; if 
(currEvMask) {	for (ix=0; ix < trigId; ix++) {	if (currEvMask & (1<<ix)) (*doneRtns[ix])(); } if 
(rol->pool->list.c) { currEvMask = 0; __done(); rol->doDone = 0; } else { poolEmpty = 1; rol->doDone = 1; } } } ; }
void partrig(unsigned long EVTYPE,unsigned long EVSOURCE)
{
    long EVENT_LENGTH;
  {   
unsigned long i, j, iadc, event_ty, event_no;
unsigned long icnt, nevt, nword, ncnt, nadc18cnt, csrA, csrB;
unsigned long ltest;
unsigned short junk;
static int dac16val= 17080 ;
static int dac12val= 400 ;
static int maxdac16=65535;
static int maxdac12=3650;
int inumdat,ioverflow;
int timeout,didreadsca;
int maxloop=50;
int verbose=0;
unsigned short rannum;
int tirdata;
static int k;
static int rannyint;
int errstat;
unsigned long sdata[2 *32*12];
unsigned long adc18data[100 ];
int pulseclean;
int localBMWobj;
int localBMWfreq;
int localBMWphase;
int localBMWperiod;
int localBMWcycnum;
int nscaler;
int iocTime;
int i1,i2,i3,i4;
int diffstate, baseval, prevbaseval;
static int dacnoise18=10000;
static int MINDAC18=10000;
static int MAXDAC18=30000;
static int dacsign18 = 1;
static int setdac18_local = 0;
 int what1=0;
unsigned long tempdata;
int* intpointer;
int    timer[21];
set_tir_oport(0x80 );
{ 
if(evtypefix==1) EVTYPE &= 1;   
event_ty = EVTYPE;
event_no = *rol->nevents;
iocTime = vxTicks;
  if (adc18bit) {
     for (iadc = 0; iadc < numadc18; iadc++) {
        adc18_go(iadc);
     }
  }
 } 
rol->dabufp = (long *)0;
{	{if(__the_event__ == (DANODE *) 0 && rol->dabufp == ((void *) 0) ) { {{ ( __the_event__ ) = 0; if (( 
&( rol->pool ->list) )->c){ ( &( rol->pool ->list) )->c--; ( __the_event__ ) = ( &( rol->pool 
->list) )->f; ( &( rol->pool ->list) )->f = ( &( rol->pool ->list) )->f->n; }; if (!( &( rol->pool ->list) 
)->c) { ( &( rol->pool ->list) )->l = 0; }} ;} ; if(__the_event__ == (DANODE *) 0) { logMsg ("TRIG ERROR: no pool buffer 
available\n"); return; } rol->dabufp = (long *) &__the_event__->length; if (input_event__) { 
__the_event__->nevent = input_event__->nevent; } else { __the_event__->nevent = *(rol->nevents); } } } ; 
StartOfEvent[event_depth__++] = (rol->dabufp); if(input_event__) {	*(++(rol->dabufp)) = (( EVTYPE ) << 16) | (( 0x01 ) << 8) | 
(0xff & (input_event__->nevent));	} else {	*(++(rol->dabufp)) = (syncFlag<<24) | (( EVTYPE ) << 16) 
| (( 0x01 ) << 8) | (0xff & *(rol->nevents));	}	((rol->dabufp))++;} ; *rol->dabufp++ = 0xffb0b444;
{ 
if ( !mtread )  {
ncnt++;
*rol->dabufp++ = 0xffdaf000;     
if(flexio==1) {
  *rol->dabufp++ = flex_io->data1;
  flex_io->csr1 = 0x100;
  flex_io->csr1 = 0x03;
  junk = flex_io->csr1;
} else if(flexio2==1) {
  *rol->dabufp++ = flex_io->data2;
  flex_io->csr2 = 0x100;
  flex_io->csr2 = 0x03;
  junk = flex_io->csr2;
} else {
  tirdata = tir[1]->tir_iport;
  *rol->dabufp++ = tirdata & 0xfff;
 }
/* # 906 "RampTest.c" */
set_tir_oport(0x02 );
if(noscaler == 0) {
  if(scaler2==1) {
    nscaler=2;
  } else {
    nscaler=1;
  }
  errstat = 0;
  didreadsca = 0;
  for (i = 0; i <nscaler; i++) {
    timeout = 0;
    while ( (pscaler[i]->csr& 0x100 ) != 0x100  && timeout++ < 10) {
      didreadsca = 1;
      for (j = 0; j < 32; j++) {
	sdata[i*32+j] = ((int)pscaler[i]->fifo[0])& 0xffffff ;  
      }
    }
  }
  if (timeout > 1) {
    errstat = 1;
    for (i = 0; i <nscaler; i++){
      pscaler[i]->clear = 1;
    }
  }
  if (didreadsca == 0) errstat |= 2;
  *rol->dabufp++ = 0xfffca000 + nscaler + (errstat<<4);  
  for (i = 0; i <nscaler; i++){
    for (j = 0; j < 32; j++) {
      *rol->dabufp++ = sdata[i*32+j];
      if (i==0) SIS1.data[j] += sdata[i*32+j]; 
    }
  }
/* # 965 "RampTest.c" */
/* # 974 "RampTest.c" */
 }
 unset_tir_oport(0x02 );
 if (adc18bit) {   
   set_tir_oport(0x01 );
   *rol->dabufp++ = 0xfadc1800+numadc18;
   for (iadc = 0; iadc < numadc18; iadc++) {
     i1 = adc18_DataAvail(iadc);
     csrA = adc18_getcsr(iadc); 
     if (setdac18_local) {
       dacnoise18 = dacnoise18 + dacsign18 * 10;
       if (dacnoise18 > MAXDAC18) dacsign18 = -1;
       if (dacnoise18 < MINDAC18) dacsign18 = 1;
     }
     icnt=0;
     timeout=0;
     for (j=0; j<maxloop; j++) {
       icnt++;
       if ( !adc18_Busy(iadc) ) break;
     } 
     if(icnt > maxloop-4 ) timeout=1; 
     nevt = adc18_getevtcnt(iadc);
     nword = adc18_getwordcnt(iadc);
     nadc18cnt = nevt*nword; 
     csrB = adc18_getcsr(iadc); 
     if (timeout && event_no > 1 ) {   
logMsg("(huh ?) TIMEOUT: ADC %d Info %d %d %d 0x%x 0x%x\n",iadc,i1,nevt,nword,csrA,csrB); logMsg("-- timeout %d event_no %d %d %d %d %d \n",timeout, event_no, 0,0,0,0);      }
     if (verbose==1 && ((i2%100)==0)) {   
logMsg("NORMAL: ADC %d Info %d %d %d 0x%x 0x%x\n",iadc,i1,nevt,nword,csrA,csrB);      }
       *rol->dabufp++ = 0xfb0b4000; 
       *rol->dabufp++ = tirdata & 0xfff;
       *rol->dabufp++ = iadc;
       *rol->dabufp++ = maxloop;
       *rol->dabufp++ = icnt;
       *rol->dabufp++ = csrA;
       *rol->dabufp++ = csrB;
       *rol->dabufp++ = nevt;
       *rol->dabufp++ = nword;
       if(timeout) { 
         *rol->dabufp++ = 0xfa180bad;  
       } else {
         *rol->dabufp++ = nadc18cnt;
       }
       if (timeout) {
         if (nword==0) nword= 14 ;
         for (j=0; j<nword; j++) {
	   *rol->dabufp++ =  BAD18DATA;   
         }
       } else {
	 for (j=0; j<nword; j++) {
           tempdata = adc18_getdata(iadc);
	   *rol->dabufp++ =  tempdata;
         }
       }
       if (adc18_DataAvail(iadc)) {
	 *rol->dabufp++ = 0xfa18bad1;   
       } else {
	 *rol->dabufp++ = 0xfa18b0b1; 
       }
cont1: 
     nevt = adc18_getevtcnt(iadc);
   }             
   unset_tir_oport(0x01 );
 }   
 if (adc16bit) {
   *rol->dabufp++ = 0xffadc000 + ADC_NUMSLOTS;      
   for (i=0; i < ADC_NUMSLOTS; i++) rol->dabufp = readoutHAPADC(rol->dabufp,i);
 }
*rol->dabufp++ = 0xfffbd000;     
*rol->dabufp++ = getDataHAPTB();
*rol->dabufp++ = getRampDelayHAPTB();
*rol->dabufp++ = getIntTimeHAPTB();
*rol->dabufp++ = (getOverSampleCurrentHAPTB() << 8) + getOverSampleHAPTB();
*rol->dabufp++ = getDACHAPTB(2);  
*rol->dabufp++ = getDACHAPTB(1);  
if(bmwscan==1) {
  *rol->dabufp++ = 0xfdacf000;   
  *rol->dabufp++ = getCleanSCAN();
  *rol->dabufp++ = getDataSCAN(1);  
  *rol->dabufp++ = getDataSCAN(2);  
  *rol->dabufp++ = iocTime;
  *rol->dabufp++ = ncnt;
}
if (adc16bit) {
  for (i = 0; i < ADC_NUMSLOTS; i++) {  
    if (usesDACHAPADC(i)==1) {
    rannum = ranny[rannyint];    
    rannyint++;
    if (rannyint> 2000000 -1) rannyint=0;
    setDACHAPADC(i,rannum);
  }
 }
}   
if(getOverSampleCurrentHAPTB()==0) {
  dac16val = dac16val + 5; 
} 
 if ( rampdac12 ) {    
   if (dac12val == 2100 && numberSteps12 < 20) {
     numberSteps12++;
   }
   else{   
     dac12val += 16 ;  
	 numberSteps12=0;
     totalSteps12++;
   }
   if (dac12val >= 8500 ) dac12val = 500 ;
   setDACHAPTB(1,dac12val);  
 }
 if ( rampdac16 ) {   
   dac16val = 100  + 64 *totalSteps16;  
   totalSteps16++;
   setDACHAPTB(2,dac16val);  
 }
  if (0 ) {
    dac16val+=10;
    if (dac16val>=maxdac16) dac16val=32700;
    setDACHAPTB(2,dac16val);
  }
 } 
if(( READOUT_VQWK == 1) ) {
*rol->dabufp++ = 0xff902902 ;
{ 
int id = 0;
 if (firstEvents>1){
   firstEvents--;
 } else if (firstEvents==1){
   logMsg("Reenabling ADC gates at beginning of event %d.",
	  event_no,0,0,0,0,0);
   unset_tir_oport(0x40 );
   firstEvents = 0;
 } else {
   int readcounter_max = 2;
   int readcounter;
   int clearall = 0;
   id=0;
   set_tir_oport(0x10 );
   while (id < nvqwk){
     timer[id] = vqwkWaitForDataReady(id, 0xFF, 200);
     if (timer[id]<0) {
       logMsg("Ev#%d: VQWK %d timed out with timer=%d.",
	      event_no, id, timer[id],0,0,0);
     }
     id++;
   }
   id=0;
   unset_tir_oport(0x10 );
   set_tir_oport(0x20 );
   clearall = 0; 
   while (id < nvqwk) {
     if (vqwkProbeDataReady(id)!=0){
       rol->dabufp+=vqwkReadRegisters(id, 0xff, rol->dabufp);
     } else {
       *(rol->dabufp)++ = 'FAKE';
       *(rol->dabufp)++ = event_no;
       *(rol->dabufp)++ = id;
       *(rol->dabufp)++ = 0;
       *(rol->dabufp)++ = 0;
       *(rol->dabufp)++ = 0;
       rol->dabufp += 7*6;
     }
     if (vqwkProbeDataReady(id)!=0){
       logMsg("Ev#%d: VQWK %d has extra data.  Clear all!",
	      event_no, id,0,0,0,0);
       clearall = 1;
     }
     id++;
   }
   if (clearall == 1){
     id=0;
     logMsg("Ev#%d: Clearing all!", event_no,0,0,0,0,0);
     while (id < nvqwk) {
       vqwkClearAllChannels(id);
       id++;
     }
     clearall = 0;
   }
 }
 unset_tir_oport(0x20 );
 } 
} 
{ 
 unset_tir_oport(0x80 );
}   
 } 
{event_depth__--; *StartOfEvent[event_depth__] = (long) (((char *) (rol->dabufp)) - ((char 
*) StartOfEvent[event_depth__]));	if ((*StartOfEvent[event_depth__] & 1) != 0) { 
(rol->dabufp) = ((long *)((char *) (rol->dabufp))+1); *StartOfEvent[event_depth__] += 1; }; if 
((*StartOfEvent[event_depth__] & 2) !=0) { *StartOfEvent[event_depth__] = *StartOfEvent[event_depth__] + 2; (rol->dabufp) = 
((long *)((short *) (rol->dabufp))+1);; };	*StartOfEvent[event_depth__] = ( 
(*StartOfEvent[event_depth__]) >> 2) - 1;}; ; sysTempeVeatClear(0);
  }   
}  
void partrig_done()
{
  {   
  }   
}  
void __done()
{
poolEmpty = 0;  
  {   
 vmetack(  1  ,  0  );  ;
  }   
}  
static void __status()
{
  {   
  }   
}  
