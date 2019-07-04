/* # 1 "injector_ts.c" */
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
void injector_ts__init (rolp)
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
	  strcpy(rol_name__, "G0INJ" );
	  rolp->listName = rol_name__;
	  printf("Init - Initializing new rol structures for %s\n",rol_name__);
	  strcpy(name, rolp->listName);
	  strcat(name, ":pool");
	  rolp->pool  = partCreate(name, 4000  , 50000 ,1);
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
/* # 7 "injector_ts.c" 2 */
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
/* # 8 "injector_ts.c" 2 */
unsigned long READOUT_BEAM_SCALERS;
unsigned long READOUT_VQWK;
unsigned long firstEvents;
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
/* # 38 "injector_ts.c" 2 */
/* # 1 "g0inj.h" 1 */
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
  if(retval == 0x7FFFFFFF  && (sval[1]=='x' || sval[1]=='X'))  
     sscanf(sval,"%x",&retval);
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
internal_configusrstr = (char *) malloc(strlen("ffile=/adaqfs/home/apar/devices/crl/injector/g0inj.flags,vqwk_verbose=1" 
)+1); strcpy(internal_configusrstr,"ffile=/adaqfs/home/apar/devices/crl/injector/g0inj.flags,vqwk_verbose=1" 
);   }
  daLogMsg("INFO","Internal Config: %s",internal_configusrstr);
  daLogMsg("INFO","rcDatabase Conf: %s",rol->usrString);
  ffile_name = getstr("ffile" );
  fd = fopen(ffile_name,"r");
  if(!fd) {
    printf("Failed to open usr flag file %s\n",ffile_name);
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
  daLogMsg("INFO","Run time Config: %s",file_configusrstr);
}
/* # 21 "g0inj.h" 2 */
/* # 1 "go_info.c" 1 */
/* # 1 "evmacro.h" 1 */
/* # 1 "go_info.c" 2 */
int goflag;
void send_go_info_event()
{
  int len,slen;
  unsigned long *sptr;
  unsigned long *lenptr;
{	{if(__user_event__ == (DANODE *) 0) { {{ ( __user_event__ ) = 0; if (( &( rol->pool ->list) 
)->c){ ( &( rol->pool ->list) )->c--; ( __user_event__ ) = ( &( rol->pool ->list) )->f; ( &( rol->pool 
->list) )->f = ( &( rol->pool ->list) )->f->n; }; if (!( &( rol->pool ->list) )->c) { ( &( rol->pool ->list) 
)->l = 0; }} ;} ; if(__user_event__ == (DANODE *) 0) { logMsg ("TRIG ERROR: no pool buffer 
available\n"); return; } rol->dabufp = (long *) &__user_event__->length; __user_event__->nevent = -1; } } ; 
StartOfUEvent = (rol->dabufp); *(++(rol->dabufp)) = ((( 0x85 ) << 16) | ( 0x10 ) << 8) | (0xff & rol->pid 
);	((rol->dabufp))++;} ;   sptr = (unsigned long *) internal_configusrstr;
  slen = strlen((char *) sptr);
  len = (slen+3)/4;
  *(rol->dabufp)++ = len+1;
*(rol->dabufp)++ = (unsigned long)(((( 0 )&0xffff)<<16)|(( slen )&0xff) | ((( 3 ) & 0xff)<<8)); 
;   while(len--) *(rol->dabufp)++ = *sptr++;
  sptr = (unsigned long *) rol->usrString;
  slen = strlen((char *) sptr);
  len = (slen+3)/4;
  *(rol->dabufp)++ = len+1;
*(rol->dabufp)++ = (unsigned long)(((( 1 )&0xffff)<<16)|(( slen )&0xff) | ((( 3 ) & 0xff)<<8)); 
;   while(len--) *(rol->dabufp)++ = *sptr++;
  sptr = (unsigned long *) file_configusrstr;
  slen = strlen((char *) sptr);
  len = (slen+3)/4;
  *(rol->dabufp)++ = len+1;
*(rol->dabufp)++ = (unsigned long)(((( 2 )&0xffff)<<16)|(( slen )&0xff) | ((( 3 ) & 0xff)<<8)); 
;   while(len--) *(rol->dabufp)++ = *sptr++;
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
->part->free_cmd)) ( __user_event__ ->part->clientData); } } ;	} __user_event__ = (DANODE *) 0; }; ; }
/* # 22 "g0inj.h" 2 */
/* # 1 "myevtypes.h" 1 */
/* # 24 "g0inj.h" 2 */
typedef unsigned int UINT32;
extern int getDataScan(int);
extern int getCleanScan();
extern int setDataScan(int,int);
extern int setCleanScan(int);
/* # 39 "injector_ts.c" 2 */
int VQWK_DATA_READY_TIME = 30;
unsigned long laddr;
extern int bigendian_out;
static int buffered;
int id = 0;
int verbose_vqwk_errors = 0;
UINT32 addr_beam     = 0x38d000;
UINT32 addrinc_beam  = 0x724000;
UINT32 nscalers_beam = 1;
UINT32 addr_vqwk;
UINT32 addrinc_vqwk = 0x10000;
UINT32 nvqwk;
UINT32 baseaddr_vqwk = 0x800000;
UINT32 addr_misc     = 0xC00000;
UINT32 addrinc_misc  = 0x0;
UINT32 nscalers_misc = 1;
UINT32 channel_mask_misc = 0x0001030f;
int creg = 0;
unsigned long imask = 0x0;
unsigned long rmask = 0xffffffff;
int latch =0;
extern int bigendian_out;
int local_mps_counter;
int local_qrt_counter;
int lastqrt_mps_counter;
static void __download()
{
    daLogMsg("INFO","Readout list compiled %s", DAYTIME);
    *(rol->async_roc) = 0;  
  {   
unsigned long res;
  bigendian_out = 0;
  tirInit(0x0ed0 );
    daLogMsg("INFO","Reading flags from file");
  init_strings();
    daLogMsg("INFO","Users Download Executed");
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
    daLogMsg("INFO","Reading flags from file");
  init_strings();
    { VME_handlers =0;VME_isAsync = 0;VMEflag = 0;} ;
{ void vmetrig ();void vmetrig_done (); doneRtns[trigId] = (FUNCPTR) ( vmetrig_done ) ; 
trigRtns[trigId] = (FUNCPTR) ( vmetrig ) ; Tcode[trigId] = ( 1 ) ; ttypeRtns[trigId] = vmettype ; {printf("Linking async VME trigger to id %d 
\n", trigId ); VME_handlers = ( trigId );VME_isAsync = 1;vmetriglink( 1 ,VME_int_handler);} 
;trigId++;} ;     {evMasks[ 1 ] |= (1<<( VME_handlers ));} ;
    daLogMsg("INFO","Begin setting up the JLab FlexIO module.");
{ 
  flexioInit(0x0ee0);          
  flexioEnableInputLatch(2);   
  flexioPrintStatus();
 } 
{ 
  nscalers_beam = 0;
  nscalers_beam = getint("nscaler" );
  if (nscalers_beam==0){
    READOUT_BEAM_SCALERS = 0;
  } else {
    READOUT_BEAM_SCALERS = 1 ;
  }
 } 
if(( READOUT_BEAM_SCALERS == 1) ) {
    daLogMsg("INFO","Begin setting up the beam scalers.");
{ 
    if (addr_beam == 0) {
       daLogMsg("ERROR","INVALID Base Address");
    } else {
daLogMsg("INFO","Initializing %d scalers starting at addr_beam 
0x%.8x",nscalers_beam,addr_beam);        sis3801InitializeMany(addr_beam, addrinc_beam , nscalers_beam);
    }
 } 
    daLogMsg("INFO","Beam scalers initialized and cleared");
} 
{ 
 nvqwk = 12;
  if (nvqwk==0){
    READOUT_VQWK = 0;
  } else {
    READOUT_VQWK = 1 ;
  }
 } 
if(( READOUT_VQWK == 1) ) {
    daLogMsg("INFO","Begin setting up the VQWK modules.");
daLogMsg("INFO","Send a disable signal to the VQWK ADC gates during prestart"); { 
  firstEvents = 2;
  set_tir_oport(0x40 );
 } 
    addr_vqwk = baseaddr_vqwk;
daLogMsg("INFO","Set up %d ADCs beginning with ladd 0x%.8x with address steps of 
0x%.8x",nvqwk,addr_vqwk, addrinc_vqwk); { 
    int id=0;
    vqwkInit(addr_vqwk, addrinc_vqwk, nvqwk);
   while(id<nvqwk){
     vqwkSetSamplePeriod(id,getint("vqwkperiod" ));
     vqwkSetNumberOfBlocks(id,getint("vqwkblocks" ));
     vqwkSetSamplesPerBlock(id,getint("vqwksamples" ));
     vqwkSetGateDelay(id,getint("vqwkdelay" ));
     id++;
   } 
    rol->dabufp = (long *)0;
{	{if(__user_event__ == (DANODE *) 0) { {{ ( __user_event__ ) = 0; if (( &( rol->pool ->list) 
)->c){ ( &( rol->pool ->list) )->c--; ( __user_event__ ) = ( &( rol->pool ->list) )->f; ( &( rol->pool 
->list) )->f = ( &( rol->pool ->list) )->f->n; }; if (!( &( rol->pool ->list) )->c) { ( &( rol->pool ->list) 
)->l = 0; }} ;} ; if(__user_event__ == (DANODE *) 0) { logMsg ("TRIG ERROR: no pool buffer 
available\n"); return; } rol->dabufp = (long *) &__user_event__->length; __user_event__->nevent = -1; } } ; 
StartOfUEvent = (rol->dabufp); *(++(rol->dabufp)) = ((( (unsigned int)(0x90 +( rol->pid )) ) << 16) | ( 0x01 ) << 
8) | (0xff & 0 );	((rol->dabufp))++;} ;     if(rol->dabufp) {
      int id=0;
      while (id < nvqwk) {
        rol->dabufp += vqwkDumpStatus(id, rol->dabufp);
        id++;
      }
    } else {
      daLogMsg("WARN","No buffer, ROC configuration event not written");
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
    daLogMsg("INFO","Begin setting up the miscelaneous scalers.");
{ 
  if (addr_misc == 0) {
       daLogMsg("ERROR","INVALID Base Address");
  } else {
daLogMsg("INFO","Initializing %d scalers starting at addr_misc 
0x%.8x",nscalers_misc,addr_misc);        initSTR7200(addr_misc, addrinc_misc , nscalers_misc);
       ClrSTR7200();
  }
 } 
    daLogMsg("INFO","Miscelaneous scalers initialized and cleared");
  local_mps_counter   = 0;
  local_qrt_counter   = 0;
  lastqrt_mps_counter = 0;
    daLogMsg("INFO","Prestart Executed");
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
   vmetdisable(  1  ,  0  );  ;
    daLogMsg("INFO","Sending disable to scalers during end");
{ 
  sis3801DisableMany();
 } 
{ 
  id = 0;
  while (id < nscalers_misc){
    disableSTR7200(id);
    id++;
  }
 } 
    daLogMsg("INFO","End Executed");
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
    daLogMsg("INFO","Pause Executed");
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
    daLogMsg("INFO","Entering Go");
    daLogMsg("INFO","Sending enable to the scalers during go");
{ 
  sis3801EnableMany();
 } 
{ 
  id = 0;
  while (id < nscalers_misc){
    countDisableSTR7200(id,~(channel_mask_misc));
    daLogMsg("INFO", "Sending a disable to the STR7200 mask pattern 0x%.8x",
             ~(channel_mask_misc));
    enableSTR7200(id);
    id++;
  }
 } 
   vmetenable(  1  ,  0  );  ;
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
void vmetrig(unsigned long EVTYPE,unsigned long EVSOURCE)
{
    long EVENT_LENGTH;
  {   
unsigned long event_ty, event_no, sync_or_unbuffered_event;
unsigned long do_beam_readout;
unsigned long i, value, ichan;
  UINT32 index,  header, real_num_of_words, num_of_words;
  UINT32 input_reg, old_out_reg, output_reg, tmp_reg, old_reg;
  UINT32 eventscalers[32];
  int timeout, tirdata, happexbit;
  int clearall;
  int id = 0;
  UINT32 qwk_warnings[3];
  int    timer[21];
  num_of_words=32;
  set_tir_oport(0x80 );
{ 
  id=0;
  while (id < 3){
    qwk_warnings[id++] = 0;
  }
  id=0;
  clearall = 0;
  if (firstEvents==0){
    unset_tir_oport(0x40 );
  } else if (firstEvents>1){
    firstEvents--;
  } else if (firstEvents==1){
    if (verbose_vqwk_errors==1){
      daLogMsgI("INFO","Reenabling ADC gates at beginning of event %d.",
                  event_no);
    }
    qwk_warnings[0] = '  ON';
    unset_tir_oport(0x40 );
    firstEvents = 0;
  }
  if (clearall == 1){
    id=0;
    if (verbose_vqwk_errors==1){
      daLogMsgI("WARN","Ev#%d: Clearing all!", event_no);
    }
    qwk_warnings[2] = 1;
    set_tir_oport(0x40 );
    while (id < nvqwk) {
      vqwkClearAllChannels(id);
      id++;
    }
    clearall = 0;
  }
 } 
  event_ty = EVTYPE;
  event_no = *rol->nevents;
{ 
  input_reg  = flexioReadData(2);
  output_reg = flexioReadData(1);
  if (0  == 1) {
    old_reg = input_reg;
    input_reg = (old_reg & (~ 0x2)) | (( (~(old_reg & 0x1))<<1) & 0x2);
  }
  id = 0;
  while (id < nscalers_misc){
    real_num_of_words = ReadSTR7200(id,&eventscalers);
    id++;
  }
 } 
  rol->dabufp = (long *)0;
{	{if(__the_event__ == (DANODE *) 0 && rol->dabufp == ((void *) 0) ) { {{ ( __the_event__ ) = 0; if (( 
&( rol->pool ->list) )->c){ ( &( rol->pool ->list) )->c--; ( __the_event__ ) = ( &( rol->pool 
->list) )->f; ( &( rol->pool ->list) )->f = ( &( rol->pool ->list) )->f->n; }; if (!( &( rol->pool ->list) 
)->c) { ( &( rol->pool ->list) )->l = 0; }} ;} ; if(__the_event__ == (DANODE *) 0) { logMsg ("TRIG ERROR: no pool buffer 
available\n"); return; } rol->dabufp = (long *) &__the_event__->length; if (input_event__) { 
__the_event__->nevent = input_event__->nevent; } else { __the_event__->nevent = *(rol->nevents); } } } ; 
StartOfEvent[event_depth__++] = (rol->dabufp); if(input_event__) {	*(++(rol->dabufp)) = (( EVTYPE ) << 16) | (( 0x10 ) << 8) | 
(0xff & (input_event__->nevent));	} else {	*(++(rol->dabufp)) = (syncFlag<<24) | (( EVTYPE ) << 16) 
| (( 0x10 ) << 8) | (0xff & *(rol->nevents));	}	((rol->dabufp))++;} ; {	long *StartOfBank; StartOfBank = (rol->dabufp); *(++(rol->dabufp)) = ((( 0x01 ) << 16) | ( 
0x01 ) << 8) | ( 0 );	((rol->dabufp))++; ;   *rol->dabufp++ = 0xffb0b444;
if(( READOUT_BEAM_SCALERS == 1) ) {
      *rol->dabufp++ = 0x3101 ;
{ 
        *rol->dabufp++ = getCleanSCAN();
        *rol->dabufp++ = getDataSCAN(1);  
        *rol->dabufp++ = getDataSCAN(2);  
        *rol->dabufp++ = 0;
        id=0;
        index  = 0;
        header = 0x0;
        while (id < nscalers_beam)
        {
          *(rol->dabufp)++ = header;
          real_num_of_words = sis3801ReadOneModule(id,num_of_words, rol->dabufp);
          rol->dabufp--;
          header = (index<<16) + real_num_of_words;
          *(rol->dabufp)++ = header;
          rol->dabufp += real_num_of_words;
          index += real_num_of_words;
          id++;
        }
          if((unsigned long) (rol->dabufp)&0x7) {
          *(rol->dabufp)++ = 0xf0f0f0f0;
        }
 } 
} 
*StartOfBank = (long) (((char *) (rol->dabufp)) - ((char *) StartOfBank));	if ((*StartOfBank 
& 1) != 0) { (rol->dabufp) = ((long *)((char *) (rol->dabufp))+1); *StartOfBank += 1; }; if 
((*StartOfBank & 2) !=0) { *StartOfBank = *StartOfBank + 2; (rol->dabufp) = ((long *)((short *) 
(rol->dabufp))+1);; };	*StartOfBank = ( (*StartOfBank) >> 2) - 1;}; ; {	long *StartOfBank; StartOfBank = (rol->dabufp); *(++(rol->dabufp)) = ((( 0x02 ) << 16) | ( 
0x01 ) << 8) | ( 0 );	((rol->dabufp))++; ; if(( READOUT_VQWK == 1) ) {
      *rol->dabufp++ = 0xff902902 ;
{ 
        id=0;
        while (id < 21){
          timer[id++] = 0;
        }
        id=0;
        clearall = 0;
        set_tir_oport(0x10 );
        while (id < nvqwk){
          timer[id] = vqwkWaitForDataReady(id, 0xFF, VQWK_DATA_READY_TIME);
          if (timer[id]<0 && verbose_vqwk_errors==1) {
            daLogMsgI("WARN","Ev#%d: VQWK %d timed out with timer=%d.",
                      event_no, id, timer[id]);
          }
          id++;
        }
        id=0;
        unset_tir_oport(0x10 );
        set_tir_oport(0x20 );
        while (id < nvqwk) {
	  if (1==1){
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
            if (verbose_vqwk_errors==1){
              daLogMsgI("WARN","Ev#%d: VQWK %d has extra data.  Clear all!",
                        event_no, id);
            }
            qwk_warnings[1] += (1<<id);
            clearall = 1;
          }
          id++;
        }
        if (clearall == 1){
          id=0;
          if (verbose_vqwk_errors==1){
            daLogMsgI("WARN","Ev#%d: Clearing all!", event_no);
          }
          qwk_warnings[2] = 1;
	  firstEvents = 2;
          set_tir_oport(0x40 );
          while (id < nvqwk) {
            vqwkClearAllChannels(id);
            id++;
          }
          clearall = 0;
        }
        unset_tir_oport(0x20 );
 } 
      *rol->dabufp++ = 0x3112 ;
{ 
        int id = 0;
        *rol->dabufp++ = 'WARN';
        *rol->dabufp++ = qwk_warnings[0];
        *rol->dabufp++ = qwk_warnings[1];
        *rol->dabufp++ = qwk_warnings[2];
        rol->dabufp += 4;
        *rol->dabufp++ = 'TIME';
        while (id < 21) {
          *rol->dabufp++ = timer[id];
          id++;
        }
        rol->dabufp += 2;
 } 
} 
*StartOfBank = (long) (((char *) (rol->dabufp)) - ((char *) StartOfBank));	if ((*StartOfBank 
& 1) != 0) { (rol->dabufp) = ((long *)((char *) (rol->dabufp))+1); *StartOfBank += 1; }; if 
((*StartOfBank & 2) !=0) { *StartOfBank = *StartOfBank + 2; (rol->dabufp) = ((long *)((short *) 
(rol->dabufp))+1);; };	*StartOfBank = ( (*StartOfBank) >> 2) - 1;}; ; {	long *StartOfBank; StartOfBank = (rol->dabufp); *(++(rol->dabufp)) = ((( 0x03 ) << 16) | ( 
0x01 ) << 8) | ( 0 );	((rol->dabufp))++; ;     *rol->dabufp++ = 0x3103 ;
{ 
      int id=0;
      UINT32 qrtbit = (input_reg & 0x4);
      *(rol->dabufp)++ = input_reg;
      *(rol->dabufp)++ = output_reg;
      *(rol->dabufp)++ = eventscalers[0];   
      *(rol->dabufp)++ = eventscalers[1];   
      *(rol->dabufp)++ = eventscalers[8];   
      *(rol->dabufp)++ = eventscalers[16];  
      *(rol->dabufp)++ = eventscalers[2];   
      *(rol->dabufp)++ = 0;
      *rol->dabufp++ = getCleanSCAN();
      *rol->dabufp++ = getDataSCAN(1);
      *rol->dabufp++ = getDataSCAN(2);
      *rol->dabufp++ = 0;
      *rol->dabufp++ = ' ADC';
      id=0;
      while (id < nvqwk) {
        *rol->dabufp++ = timer[id];
        id++;
      }
 } 
*StartOfBank = (long) (((char *) (rol->dabufp)) - ((char *) StartOfBank));	if ((*StartOfBank 
& 1) != 0) { (rol->dabufp) = ((long *)((char *) (rol->dabufp))+1); *StartOfBank += 1; }; if 
((*StartOfBank & 2) !=0) { *StartOfBank = *StartOfBank + 2; (rol->dabufp) = ((long *)((short *) 
(rol->dabufp))+1);; };	*StartOfBank = ( (*StartOfBank) >> 2) - 1;}; ; {	long *StartOfBank; StartOfBank = (rol->dabufp); *(++(rol->dabufp)) = ((( 0x04 ) << 16) | ( 
0x01 ) << 8) | ( 0 );	((rol->dabufp))++; ; { 
  *rol->dabufp++ = 0xfffbd000;     
  *rol->dabufp++ = getDataHAPTB();
  *rol->dabufp++ = getRampDelayHAPTB();
  *rol->dabufp++ = getIntTimeHAPTB();
  *rol->dabufp++ = (getOverSampleCurrentHAPTB() << 8) + getOverSampleHAPTB();
  *rol->dabufp++ = getDACHAPTB(2);  
  *rol->dabufp++ = getDACHAPTB(1);  
  *rol->dabufp++ = 0;
  *rol->dabufp++ = 0;
 } 
*StartOfBank = (long) (((char *) (rol->dabufp)) - ((char *) StartOfBank));	if ((*StartOfBank 
& 1) != 0) { (rol->dabufp) = ((long *)((char *) (rol->dabufp))+1); *StartOfBank += 1; }; if 
((*StartOfBank & 2) !=0) { *StartOfBank = *StartOfBank + 2; (rol->dabufp) = ((long *)((short *) 
(rol->dabufp))+1);; };	*StartOfBank = ( (*StartOfBank) >> 2) - 1;}; ; {event_depth__--; *StartOfEvent[event_depth__] = (long) (((char *) (rol->dabufp)) - ((char 
*) StartOfEvent[event_depth__]));	if ((*StartOfEvent[event_depth__] & 1) != 0) { 
(rol->dabufp) = ((long *)((char *) (rol->dabufp))+1); *StartOfEvent[event_depth__] += 1; }; if 
((*StartOfEvent[event_depth__] & 2) !=0) { *StartOfEvent[event_depth__] = *StartOfEvent[event_depth__] + 2; (rol->dabufp) = 
((long *)((short *) (rol->dabufp))+1);; };	*StartOfEvent[event_depth__] = ( 
(*StartOfEvent[event_depth__]) >> 2) - 1;}; ;   }   
}  
void vmetrig_done()
{
  {   
  unset_tir_oport(0x80 );
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
