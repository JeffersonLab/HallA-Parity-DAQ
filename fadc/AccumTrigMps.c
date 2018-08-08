/* # 1 "AccumTrigMps.c" */
/* # 1 "/adaqfs/coda/2.5/common/include/rol.h" 1 */
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
/* # 1 "/adaqfs/coda/2.5/common/include/libpart.h" 1 */
extern char	*malloc();
/* # 1 "/adaqfs/coda/2.5/common/include/mempart.h" 1 */
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
/* # 62 "/adaqfs/coda/2.5/common/include/mempart.h" */
/* # 79 "/adaqfs/coda/2.5/common/include/mempart.h" */
/* # 106 "/adaqfs/coda/2.5/common/include/mempart.h" */
extern void partFreeAll();  
extern ROL_MEM_ID partCreate (char *name, int size, int c, int incr);
/* # 18 "/adaqfs/coda/2.5/common/include/libpart.h" 2 */
/* # 67 "/adaqfs/coda/2.5/common/include/rol.h" 2 */
/* # 1 "/adaqfs/coda/2.5/common/include/rolInt.h" 1 */
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
/* # 70 "/adaqfs/coda/2.5/common/include/rol.h" 2 */
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
/* # 1 "/adaqfs/coda/2.5/common/include/BankTools.h" 1 */
static int EVENT_type;
long *StartOfEvent[32 ],event_depth__, *StartOfUEvent;
/* # 78 "/adaqfs/coda/2.5/common/include/BankTools.h" */
/* # 95 "/adaqfs/coda/2.5/common/include/BankTools.h" */
/* # 107 "/adaqfs/coda/2.5/common/include/BankTools.h" */
/* # 120 "/adaqfs/coda/2.5/common/include/BankTools.h" */
/* # 152 "/adaqfs/coda/2.5/common/include/BankTools.h" */
/* # 186 "/adaqfs/coda/2.5/common/include/BankTools.h" */
/* # 198 "/adaqfs/coda/2.5/common/include/BankTools.h" */
/* # 217 "/adaqfs/coda/2.5/common/include/BankTools.h" */
/* # 92 "/adaqfs/coda/2.5/common/include/rol.h" 2 */
/* # 1 "/adaqfs/coda/2.5/common/include/trigger_dispatch.h" 1 */
static unsigned char dispatch_busy; 
static int intLockKey,trigId;
static int poolEmpty;
static unsigned long theEvMask, currEvMask, currType, evMasks[16 ];
static VOIDFUNCPTR wrapperGenerator;
static FUNCPTR trigRtns[32 ], syncTRtns[32 ], doneRtns[32 ], ttypeRtns[32 ];
static unsigned long Tcode[32 ];
static DANODE *__the_event__, *input_event__, *__user_event__;
/* # 50 "/adaqfs/coda/2.5/common/include/trigger_dispatch.h" */
/* # 141 "/adaqfs/coda/2.5/common/include/trigger_dispatch.h" */
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
	 if (wrapperGenerator) {event_depth__--; *StartOfEvent[event_depth__] = (long) (((char 
*) (rol->dabufp)) - ((char *) StartOfEvent[event_depth__]));	if 
((*StartOfEvent[event_depth__] & 1) != 0) { (rol->dabufp) = ((long *)((char *) (rol->dabufp))+1); 
*StartOfEvent[event_depth__] += 1; }; if ((*StartOfEvent[event_depth__] & 2) !=0) { *StartOfEvent[event_depth__] = 
*StartOfEvent[event_depth__] + 2; (rol->dabufp) = ((long *)((short *) (rol->dabufp))+1);; 
};	*StartOfEvent[event_depth__] = ( (*StartOfEvent[event_depth__]) >> 2) - 1;}; ;	 	 rol->dabufp = ((void *) 0) ; if (__the_event__) { if (rol->output) { {if(! ( 
&(rol->output->list) )->c ){( &(rol->output->list) )->f = ( &(rol->output->list) )->l = ( __the_event__ );( 
__the_event__ )->p = 0;} else {( __the_event__ )->p = ( &(rol->output->list) )->l;( &(rol->output->list) 
)->l->n = ( __the_event__ );( &(rol->output->list) )->l = ( __the_event__ );} ( __the_event__ )->n = 
0;( &(rol->output->list) )->c++;	if(( &(rol->output->list) )->add_cmd != ((void *) 0) ) (*(( 
&(rol->output->list) )->add_cmd)) (( &(rol->output->list) )); } ; } else { { if (( __the_event__ )->part == 0) { free( 
__the_event__ ); __the_event__ = 0; } else { {if(! ( & __the_event__ ->part->list )->c ){( & __the_event__ 
->part->list )->f = ( & __the_event__ ->part->list )->l = ( __the_event__ );( __the_event__ )->p = 0;} else 
{( __the_event__ )->p = ( & __the_event__ ->part->list )->l;( & __the_event__ ->part->list 
)->l->n = ( __the_event__ );( & __the_event__ ->part->list )->l = ( __the_event__ );} ( __the_event__ 
)->n = 0;( & __the_event__ ->part->list )->c++;	if(( & __the_event__ ->part->list )->add_cmd != 
((void *) 0) ) (*(( & __the_event__ ->part->list )->add_cmd)) (( & __the_event__ ->part->list )); } ; 
} if( __the_event__ ->part->free_cmd != ((void *) 0) ) { (*( __the_event__ ->part->free_cmd)) 
( __the_event__ ->part->clientData); } } ;	} __the_event__ = (DANODE *) 0; } 
if(input_event__) { { if (( input_event__ )->part == 0) { free( input_event__ ); input_event__ = 0; } else { {if(! ( & 
input_event__ ->part->list )->c ){( & input_event__ ->part->list )->f = ( & input_event__ ->part->list 
)->l = ( input_event__ );( input_event__ )->p = 0;} else {( input_event__ )->p = ( & input_event__ 
->part->list )->l;( & input_event__ ->part->list )->l->n = ( input_event__ );( & input_event__ 
->part->list )->l = ( input_event__ );} ( input_event__ )->n = 0;( & input_event__ ->part->list 
)->c++;	if(( & input_event__ ->part->list )->add_cmd != ((void *) 0) ) (*(( & input_event__ ->part->list 
)->add_cmd)) (( & input_event__ ->part->list )); } ; } if( input_event__ ->part->free_cmd != ((void *) 0) ) { 
(*( input_event__ ->part->free_cmd)) ( input_event__ ->part->clientData); } } ; 
input_event__ = (DANODE *) 0; } {int ix; if (currEvMask) {	for (ix=0; ix < trigId; ix++) {	if (currEvMask & 
(1<<ix)) (*doneRtns[ix])(); } if (rol->pool->list.c) { currEvMask = 0; __done(); rol->doDone = 0; } 
else { poolEmpty = 1; rol->doDone = 1; } } } ; 	}
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
	{ if (( theNode )->part == 0) { free( theNode ); theNode = 0; } else { {if(! ( & theNode 
->part->list )->c ){( & theNode ->part->list )->f = ( & theNode ->part->list )->l = ( theNode );( theNode )->p 
= 0;} else {( theNode )->p = ( & theNode ->part->list )->l;( & theNode ->part->list )->l->n = ( 
theNode );( & theNode ->part->list )->l = ( theNode );} ( theNode )->n = 0;( & theNode ->part->list 
)->c++;	if(( & theNode ->part->list )->add_cmd != ((void *) 0) ) (*(( & theNode ->part->list )->add_cmd)) 
(( & theNode ->part->list )); } ; } if( theNode ->part->free_cmd != ((void *) 0) ) { (*( theNode 
->part->free_cmd)) ( theNode ->part->clientData); } } ;       if (theEvMask) {
	theEvMask = theEvMask & ~(1<<theSource);
      } 
      if (!theEvMask) {
	rol->dabufp = ((void *) 0) ; if (__the_event__) { if (rol->output) { {if(! ( 
&(rol->output->list) )->c ){( &(rol->output->list) )->f = ( &(rol->output->list) )->l = ( __the_event__ );( 
__the_event__ )->p = 0;} else {( __the_event__ )->p = ( &(rol->output->list) )->l;( &(rol->output->list) 
)->l->n = ( __the_event__ );( &(rol->output->list) )->l = ( __the_event__ );} ( __the_event__ )->n = 
0;( &(rol->output->list) )->c++;	if(( &(rol->output->list) )->add_cmd != ((void *) 0) ) (*(( 
&(rol->output->list) )->add_cmd)) (( &(rol->output->list) )); } ; } else { { if (( __the_event__ )->part == 0) { free( 
__the_event__ ); __the_event__ = 0; } else { {if(! ( & __the_event__ ->part->list )->c ){( & __the_event__ 
->part->list )->f = ( & __the_event__ ->part->list )->l = ( __the_event__ );( __the_event__ )->p = 0;} else 
{( __the_event__ )->p = ( & __the_event__ ->part->list )->l;( & __the_event__ ->part->list 
)->l->n = ( __the_event__ );( & __the_event__ ->part->list )->l = ( __the_event__ );} ( __the_event__ 
)->n = 0;( & __the_event__ ->part->list )->c++;	if(( & __the_event__ ->part->list )->add_cmd != 
((void *) 0) ) (*(( & __the_event__ ->part->list )->add_cmd)) (( & __the_event__ ->part->list )); } ; 
} if( __the_event__ ->part->free_cmd != ((void *) 0) ) { (*( __the_event__ ->part->free_cmd)) 
( __the_event__ ->part->clientData); } } ;	} __the_event__ = (DANODE *) 0; } 
if(input_event__) { { if (( input_event__ )->part == 0) { free( input_event__ ); input_event__ = 0; } else { {if(! ( & 
input_event__ ->part->list )->c ){( & input_event__ ->part->list )->f = ( & input_event__ ->part->list 
)->l = ( input_event__ );( input_event__ )->p = 0;} else {( input_event__ )->p = ( & input_event__ 
->part->list )->l;( & input_event__ ->part->list )->l->n = ( input_event__ );( & input_event__ 
->part->list )->l = ( input_event__ );} ( input_event__ )->n = 0;( & input_event__ ->part->list 
)->c++;	if(( & input_event__ ->part->list )->add_cmd != ((void *) 0) ) (*(( & input_event__ ->part->list 
)->add_cmd)) (( & input_event__ ->part->list )); } ; } if( input_event__ ->part->free_cmd != ((void *) 0) ) { 
(*( input_event__ ->part->free_cmd)) ( input_event__ ->part->clientData); } } ; 
input_event__ = (DANODE *) 0; } {int ix; if (currEvMask) {	for (ix=0; ix < trigId; ix++) {	if (currEvMask & 
(1<<ix)) (*doneRtns[ix])(); } if (rol->pool->list.c) { currEvMask = 0; __done(); rol->doDone = 0; } 
else { poolEmpty = 1; rol->doDone = 1; } } } ;       }
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
theNode ) = ( &( rol->dispatch ->list) )->f; ( &( rol->dispatch ->list) )->f = ( &( rol->dispatch 
->list) )->f->n; }; if (!( &( rol->dispatch ->list) )->c) { ( &( rol->dispatch ->list) )->l = 0; }} ;} ;     theNode->source = theSource;
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
theNode ) = ( &( rol->dispatch ->list) )->f; ( &( rol->dispatch ->list) )->f = ( &( rol->dispatch 
->list) )->f->n; }; if (!( &( rol->dispatch ->list) )->c) { ( &( rol->dispatch ->list) )->l = 0; }} ;} ; 	    theNode->source = theSource; 
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
/* # 99 "/adaqfs/coda/2.5/common/include/rol.h" 2 */
static char rol_name__[40];
static char temp_string__[132];
static void __poll()
{
    {cdopolldispatch();} ;
}
void AccumTrigMps__init (rolp)
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
	  rolp->pool  = partCreate(name, 9216  , 500 ,1);
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
/* # 6 "AccumTrigMps.c" 2 */
/* # 1 "/adaqfs/coda/2.5/common/include/VME_source.h" 1 */
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
struct vme_dpm {
    long dpm_mem[32768];
    unsigned short dpm_addr;
    unsigned short dpm_mode;
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
struct vme_dpm *dpm;
struct vme_dpm *dpml;
struct vme_scal *vscal[32];
struct vme_scal *vlscal[32];
volatile unsigned long *tsmem;
unsigned long ts_memory[4096];
long *vme2_ir;
/* # 1 "/adaqfs/coda/2.5/common/include/vme_triglib.h" 1 */
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
    if(ts) {
      ts->intVec = 0xec ;
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
 if(code == 2 )
   sysIntEnable(3 );
  intEnable(11);          
 if(code == 1 )
   tir[1]->tir_csr = 0x7;
 if(code == 2 )
   ts->csr2 |= 0x0000;
  intUnlock(lock_key);
/* # 187 "/adaqfs/coda/2.5/common/include/vme_triglib.h" */
}
static inline void 
vmetdisable(int code,unsigned int intMask)
{
 if(code == 1 )
   tir[1]->tir_csr = 0x80;
 if(code == 2 )
   ts->csr2 &= ~(0x1800);
}
static inline void 
vmetack(int code, unsigned int intMask)
{
 if(code == 1 )
   tir[1]->tir_dat = 0x8000;
 if(code == 2 )
   ts->lrocBufStatus = 0x100;
}
static inline unsigned long 
vmettype(int code)
{
 unsigned long tt;
/* # 227 "/adaqfs/coda/2.5/common/include/vme_triglib.h" */
  tt=1;
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
   lval = ts->lrocBufStatus;
   if( (lval != 0xffffffff) && ((lval&0xf) != 0) ) {
     return (1);
   } else {
     return (0);
   }
 }
  return(0);
}
/* # 136 "/adaqfs/coda/2.5/common/include/VME_source.h" 2 */
void VME_int_handler()
{
  theIntHandler(VME_handlers);                    
}
/* # 7 "AccumTrigMps.c" 2 */
/* # 1 "scaler1.h" 1 */
struct lecroy_1151_scaler {
    unsigned short reset;
    unsigned short blank1[7];
    unsigned short bim[8];
    unsigned short blank2[16];
    unsigned long preset[16];
    unsigned long scaler[16];
    unsigned short blank3[29];
    unsigned short id[3];
  };
struct str_7200_scaler {
  unsigned long CSR             ;  
  unsigned long IntVerR         ;  
  unsigned long empty0          ;  
  unsigned long disCntR         ;  
  unsigned long empty1          ;  
  unsigned long clrFIFOR        ;  
  unsigned long wrToFIFOR       ;  
  unsigned long empty2          ;  
  unsigned long clrR            ;  
  unsigned long clkShadowR      ;  
  unsigned long gEnCntR         ;  
  unsigned long gDisCntR        ;  
  unsigned long bclrCntR        ;  
  unsigned long bclkShadowR     ;  
  unsigned long bgEnCntR        ;  
  unsigned long bgDisCntR       ;  
  unsigned long grClrR[4]       ;  
  unsigned long empty3[4]       ;  
  unsigned long resetR          ;  
  unsigned long empty4          ;  
  unsigned long testclkR        ;  
  unsigned long empty5[5]       ;  
  unsigned long readFIFOR       ;  
  unsigned long empty6[31]      ;  
  unsigned long clrCntR[32]     ;  
  unsigned long clrOverR[32]    ;  
  unsigned long readShR[32]     ;  
  unsigned long readCntR[32]    ;  
  unsigned long readClrCntR[32] ;  
  unsigned long grOverR[4]      ;  
 };
struct caen_v560_scaler {
  unsigned short empty1[2];  
  unsigned short IntVectR ;  
  unsigned short IntLevR  ;  
  unsigned short EnIntR   ;  
  unsigned short DisIntR  ;  
  unsigned short ClrIntR  ;  
  unsigned short RequestR ;  
  unsigned long  CntR[16] ;  
  unsigned short ClrR     ;  
  unsigned short setVETOR ;  
  unsigned short clrVETOR ;  
  unsigned short incCntR  ;  
  unsigned short StatusR  ;  
  unsigned short empty2[80];  
  unsigned short FixCodeR ;  
  unsigned short ModTypeR ;  
  unsigned short VersionR ;  
 };
static struct lecroy_1151_scaler *slecroy1151[10 ];
static struct str_7200_scaler *sstr7200[10 ];
static struct caen_v560_scaler *scaen560[10 ];
/* # 23 "AccumTrigMps.c" 2 */
unsigned long laddr;
extern int bigendian_out;
unsigned long event_counter = 0;
unsigned long current_mode = 0;
unsigned long starting_mps = 0;
unsigned long current_mps = 0;
static void __download()
{
    daLogMsg("INFO","Readout list compiled %s", DAYTIME);
    *(rol->async_roc) = 0;  
  {   
unsigned long res;
unsigned long laddr;
unsigned short temp;
  bigendian_out = 0;
  tirInit(0x0ed0 );
  initHAPTB();
  setTimeHAPTB(200,12000);
  sis3320SetupMode(0 , 2);
  sis3320SetMaxNumEvents(0 , 1 );
  sis3320SetMaxSampleLength(0 , 512 );
  sis3320StopDelay(0,0 );
  sis3320SetGain(0,-1,1);
  sis3320SetDac(0,45000);
  sis3320SetThresh(0, 0, 3700, 3200);
  sis3320SetN5N6(0, 0, 10, 100, 10, 100);
{ 
 if (1 ) {
   res = sysBusToLocalAdrs(0x39, 0xceb500 , &laddr);
   if (res != 0) {
      printf("ERROR: cannot address scaler at 0x%x \n",0xceb500 );
      scaen560[0] = 0;
   } else {
      scaen560[0] = (struct caen_v560_scaler *)laddr;
   }
   if (scaen560[0] != 0) {   
        printf("Clearing scalers in download \n ");
        scaen560[0]->ClrR=0;
        temp = scaen560[0]->clrVETOR;
   }
 }
 } 
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
    { VME_handlers =0;VME_isAsync = 0;VMEflag = 0;} ;
{ void usrtrig ();void usrtrig_done (); doneRtns[trigId] = (FUNCPTR) ( usrtrig_done ) ; 
trigRtns[trigId] = (FUNCPTR) ( usrtrig ) ; Tcode[trigId] = ( 1 ) ; ttypeRtns[trigId] = vmettype ; {printf("Linking async VME trigger to id %d 
\n", trigId ); VME_handlers = ( trigId );VME_isAsync = 1;vmetriglink( 1 ,VME_int_handler);} 
;trigId++;} ;     {evMasks[ 1 ] |= (1<<( VME_handlers ));} ;
    daLogMsg("INFO","User Prestart Executed");
  }   
if (__the_event__) rol->dabufp = ((void *) 0) ; if (__the_event__) { if (rol->output) { {if(! ( 
&(rol->output->list) )->c ){( &(rol->output->list) )->f = ( &(rol->output->list) )->l = ( __the_event__ );( 
__the_event__ )->p = 0;} else {( __the_event__ )->p = ( &(rol->output->list) )->l;( &(rol->output->list) 
)->l->n = ( __the_event__ );( &(rol->output->list) )->l = ( __the_event__ );} ( __the_event__ )->n = 
0;( &(rol->output->list) )->c++;	if(( &(rol->output->list) )->add_cmd != ((void *) 0) ) (*(( 
&(rol->output->list) )->add_cmd)) (( &(rol->output->list) )); } ; } else { { if (( __the_event__ )->part == 0) { free( 
__the_event__ ); __the_event__ = 0; } else { {if(! ( & __the_event__ ->part->list )->c ){( & __the_event__ 
->part->list )->f = ( & __the_event__ ->part->list )->l = ( __the_event__ );( __the_event__ )->p = 0;} else 
{( __the_event__ )->p = ( & __the_event__ ->part->list )->l;( & __the_event__ ->part->list 
)->l->n = ( __the_event__ );( & __the_event__ ->part->list )->l = ( __the_event__ );} ( __the_event__ 
)->n = 0;( & __the_event__ ->part->list )->c++;	if(( & __the_event__ ->part->list )->add_cmd != 
((void *) 0) ) (*(( & __the_event__ ->part->list )->add_cmd)) (( & __the_event__ ->part->list )); } ; 
} if( __the_event__ ->part->free_cmd != ((void *) 0) ) { (*( __the_event__ ->part->free_cmd)) 
( __the_event__ ->part->clientData); } } ;	} __the_event__ = (DANODE *) 0; } 
if(input_event__) { { if (( input_event__ )->part == 0) { free( input_event__ ); input_event__ = 0; } else { {if(! ( & 
input_event__ ->part->list )->c ){( & input_event__ ->part->list )->f = ( & input_event__ ->part->list 
)->l = ( input_event__ );( input_event__ )->p = 0;} else {( input_event__ )->p = ( & input_event__ 
->part->list )->l;( & input_event__ ->part->list )->l->n = ( input_event__ );( & input_event__ 
->part->list )->l = ( input_event__ );} ( input_event__ )->n = 0;( & input_event__ ->part->list 
)->c++;	if(( & input_event__ ->part->list )->add_cmd != ((void *) 0) ) (*(( & input_event__ ->part->list 
)->add_cmd)) (( & input_event__ ->part->list )); } ; } if( input_event__ ->part->free_cmd != ((void *) 0) ) { 
(*( input_event__ ->part->free_cmd)) ( input_event__ ->part->clientData); } } ; 
input_event__ = (DANODE *) 0; } {int ix; if (currEvMask) {	for (ix=0; ix < trigId; ix++) {	if (currEvMask & 
(1<<ix)) (*doneRtns[ix])(); } if (rol->pool->list.c) { currEvMask = 0; __done(); rol->doDone = 0; } 
else { poolEmpty = 1; rol->doDone = 1; } } } ;     *(rol->nevents) = 0;
    rol->recNb = 0;
}       
static void __end()
{
  {   
   vmetdisable(  1  ,  0  );  ;
    daLogMsg("INFO","User End Executed");
  }   
if (__the_event__) rol->dabufp = ((void *) 0) ; if (__the_event__) { if (rol->output) { {if(! ( 
&(rol->output->list) )->c ){( &(rol->output->list) )->f = ( &(rol->output->list) )->l = ( __the_event__ );( 
__the_event__ )->p = 0;} else {( __the_event__ )->p = ( &(rol->output->list) )->l;( &(rol->output->list) 
)->l->n = ( __the_event__ );( &(rol->output->list) )->l = ( __the_event__ );} ( __the_event__ )->n = 
0;( &(rol->output->list) )->c++;	if(( &(rol->output->list) )->add_cmd != ((void *) 0) ) (*(( 
&(rol->output->list) )->add_cmd)) (( &(rol->output->list) )); } ; } else { { if (( __the_event__ )->part == 0) { free( 
__the_event__ ); __the_event__ = 0; } else { {if(! ( & __the_event__ ->part->list )->c ){( & __the_event__ 
->part->list )->f = ( & __the_event__ ->part->list )->l = ( __the_event__ );( __the_event__ )->p = 0;} else 
{( __the_event__ )->p = ( & __the_event__ ->part->list )->l;( & __the_event__ ->part->list 
)->l->n = ( __the_event__ );( & __the_event__ ->part->list )->l = ( __the_event__ );} ( __the_event__ 
)->n = 0;( & __the_event__ ->part->list )->c++;	if(( & __the_event__ ->part->list )->add_cmd != 
((void *) 0) ) (*(( & __the_event__ ->part->list )->add_cmd)) (( & __the_event__ ->part->list )); } ; 
} if( __the_event__ ->part->free_cmd != ((void *) 0) ) { (*( __the_event__ ->part->free_cmd)) 
( __the_event__ ->part->clientData); } } ;	} __the_event__ = (DANODE *) 0; } 
if(input_event__) { { if (( input_event__ )->part == 0) { free( input_event__ ); input_event__ = 0; } else { {if(! ( & 
input_event__ ->part->list )->c ){( & input_event__ ->part->list )->f = ( & input_event__ ->part->list 
)->l = ( input_event__ );( input_event__ )->p = 0;} else {( input_event__ )->p = ( & input_event__ 
->part->list )->l;( & input_event__ ->part->list )->l->n = ( input_event__ );( & input_event__ 
->part->list )->l = ( input_event__ );} ( input_event__ )->n = 0;( & input_event__ ->part->list 
)->c++;	if(( & input_event__ ->part->list )->add_cmd != ((void *) 0) ) (*(( & input_event__ ->part->list 
)->add_cmd)) (( & input_event__ ->part->list )); } ; } if( input_event__ ->part->free_cmd != ((void *) 0) ) { 
(*( input_event__ ->part->free_cmd)) ( input_event__ ->part->clientData); } } ; 
input_event__ = (DANODE *) 0; } {int ix; if (currEvMask) {	for (ix=0; ix < trigId; ix++) {	if (currEvMask & 
(1<<ix)) (*doneRtns[ix])(); } if (rol->pool->list.c) { currEvMask = 0; __done(); rol->doDone = 0; } 
else { poolEmpty = 1; rol->doDone = 1; } } } ; }  
static void __pause()
{
  {   
   vmetdisable(  1  ,  0  );  ;
    daLogMsg("INFO","User Pause Executed");
  }   
if (__the_event__) rol->dabufp = ((void *) 0) ; if (__the_event__) { if (rol->output) { {if(! ( 
&(rol->output->list) )->c ){( &(rol->output->list) )->f = ( &(rol->output->list) )->l = ( __the_event__ );( 
__the_event__ )->p = 0;} else {( __the_event__ )->p = ( &(rol->output->list) )->l;( &(rol->output->list) 
)->l->n = ( __the_event__ );( &(rol->output->list) )->l = ( __the_event__ );} ( __the_event__ )->n = 
0;( &(rol->output->list) )->c++;	if(( &(rol->output->list) )->add_cmd != ((void *) 0) ) (*(( 
&(rol->output->list) )->add_cmd)) (( &(rol->output->list) )); } ; } else { { if (( __the_event__ )->part == 0) { free( 
__the_event__ ); __the_event__ = 0; } else { {if(! ( & __the_event__ ->part->list )->c ){( & __the_event__ 
->part->list )->f = ( & __the_event__ ->part->list )->l = ( __the_event__ );( __the_event__ )->p = 0;} else 
{( __the_event__ )->p = ( & __the_event__ ->part->list )->l;( & __the_event__ ->part->list 
)->l->n = ( __the_event__ );( & __the_event__ ->part->list )->l = ( __the_event__ );} ( __the_event__ 
)->n = 0;( & __the_event__ ->part->list )->c++;	if(( & __the_event__ ->part->list )->add_cmd != 
((void *) 0) ) (*(( & __the_event__ ->part->list )->add_cmd)) (( & __the_event__ ->part->list )); } ; 
} if( __the_event__ ->part->free_cmd != ((void *) 0) ) { (*( __the_event__ ->part->free_cmd)) 
( __the_event__ ->part->clientData); } } ;	} __the_event__ = (DANODE *) 0; } 
if(input_event__) { { if (( input_event__ )->part == 0) { free( input_event__ ); input_event__ = 0; } else { {if(! ( & 
input_event__ ->part->list )->c ){( & input_event__ ->part->list )->f = ( & input_event__ ->part->list 
)->l = ( input_event__ );( input_event__ )->p = 0;} else {( input_event__ )->p = ( & input_event__ 
->part->list )->l;( & input_event__ ->part->list )->l->n = ( input_event__ );( & input_event__ 
->part->list )->l = ( input_event__ );} ( input_event__ )->n = 0;( & input_event__ ->part->list 
)->c++;	if(( & input_event__ ->part->list )->add_cmd != ((void *) 0) ) (*(( & input_event__ ->part->list 
)->add_cmd)) (( & input_event__ ->part->list )); } ; } if( input_event__ ->part->free_cmd != ((void *) 0) ) { 
(*( input_event__ ->part->free_cmd)) ( input_event__ ->part->clientData); } } ; 
input_event__ = (DANODE *) 0; } {int ix; if (currEvMask) {	for (ix=0; ix < trigId; ix++) {	if (currEvMask & 
(1<<ix)) (*doneRtns[ix])(); } if (rol->pool->list.c) { currEvMask = 0; __done(); rol->doDone = 0; } 
else { poolEmpty = 1; rol->doDone = 1; } } } ; }  
static void __go()
{
  {   
  unsigned short temp;
    daLogMsg("INFO","Entering User Go");
  event_counter= 0;
  current_mode = 0;
  starting_mps = 0;
  current_mps = 0;
{ 
  if( current_mode  == 0){      
     tir[1]->tir_oport=0xffff;
     printf("Output bits: %x /n", tir[1]->tir_oport);
     sis3320StopDelay(0,0 );
     sis3320Disable(0 );
     sis3320Enable(0 );
  } else {              
     tir[1]->tir_oport=0;
     printf("Output bits: %x /n", tir[1]->tir_oport);
     sis3320StopDelay(0,300 );
     sis3320Disable(0 );
     sis3320Enable(0 );
     sis3320StartSampling(0 );   
  }
  if (1 ) {
     if (scaen560[0] != 0) {   
        printf("Clearing scalers in go \n");
         scaen560[0]->ClrR=0;
         temp = scaen560[0]->clrVETOR;
     }
  }
 } 
  vmetenable(  1  ,  0  );  ;
  }   
if (__the_event__) rol->dabufp = ((void *) 0) ; if (__the_event__) { if (rol->output) { {if(! ( 
&(rol->output->list) )->c ){( &(rol->output->list) )->f = ( &(rol->output->list) )->l = ( __the_event__ );( 
__the_event__ )->p = 0;} else {( __the_event__ )->p = ( &(rol->output->list) )->l;( &(rol->output->list) 
)->l->n = ( __the_event__ );( &(rol->output->list) )->l = ( __the_event__ );} ( __the_event__ )->n = 
0;( &(rol->output->list) )->c++;	if(( &(rol->output->list) )->add_cmd != ((void *) 0) ) (*(( 
&(rol->output->list) )->add_cmd)) (( &(rol->output->list) )); } ; } else { { if (( __the_event__ )->part == 0) { free( 
__the_event__ ); __the_event__ = 0; } else { {if(! ( & __the_event__ ->part->list )->c ){( & __the_event__ 
->part->list )->f = ( & __the_event__ ->part->list )->l = ( __the_event__ );( __the_event__ )->p = 0;} else 
{( __the_event__ )->p = ( & __the_event__ ->part->list )->l;( & __the_event__ ->part->list 
)->l->n = ( __the_event__ );( & __the_event__ ->part->list )->l = ( __the_event__ );} ( __the_event__ 
)->n = 0;( & __the_event__ ->part->list )->c++;	if(( & __the_event__ ->part->list )->add_cmd != 
((void *) 0) ) (*(( & __the_event__ ->part->list )->add_cmd)) (( & __the_event__ ->part->list )); } ; 
} if( __the_event__ ->part->free_cmd != ((void *) 0) ) { (*( __the_event__ ->part->free_cmd)) 
( __the_event__ ->part->clientData); } } ;	} __the_event__ = (DANODE *) 0; } 
if(input_event__) { { if (( input_event__ )->part == 0) { free( input_event__ ); input_event__ = 0; } else { {if(! ( & 
input_event__ ->part->list )->c ){( & input_event__ ->part->list )->f = ( & input_event__ ->part->list 
)->l = ( input_event__ );( input_event__ )->p = 0;} else {( input_event__ )->p = ( & input_event__ 
->part->list )->l;( & input_event__ ->part->list )->l->n = ( input_event__ );( & input_event__ 
->part->list )->l = ( input_event__ );} ( input_event__ )->n = 0;( & input_event__ ->part->list 
)->c++;	if(( & input_event__ ->part->list )->add_cmd != ((void *) 0) ) (*(( & input_event__ ->part->list 
)->add_cmd)) (( & input_event__ ->part->list )); } ; } if( input_event__ ->part->free_cmd != ((void *) 0) ) { 
(*( input_event__ ->part->free_cmd)) ( input_event__ ->part->clientData); } } ; 
input_event__ = (DANODE *) 0; } {int ix; if (currEvMask) {	for (ix=0; ix < trigId; ix++) {	if (currEvMask & 
(1<<ix)) (*doneRtns[ix])(); } if (rol->pool->list.c) { currEvMask = 0; __done(); rol->doDone = 0; } 
else { poolEmpty = 1; rol->doDone = 1; } } } ; }
void usrtrig(unsigned long EVTYPE,unsigned long EVSOURCE)
{
    long EVENT_LENGTH;
  {   
unsigned long stat, csr, jj;
 volatile unsigned short tir_dat;
rol->dabufp = (long *) 0;
{	{if(__the_event__ == (DANODE *) 0 && rol->dabufp == ((void *) 0) ) { {{ ( __the_event__ ) = 0; if 
(( &( rol->pool ->list) )->c){ ( &( rol->pool ->list) )->c--; ( __the_event__ ) = ( &( rol->pool 
->list) )->f; ( &( rol->pool ->list) )->f = ( &( rol->pool ->list) )->f->n; }; if (!( &( rol->pool 
->list) )->c) { ( &( rol->pool ->list) )->l = 0; }} ;} ; if(__the_event__ == (DANODE *) 0) { logMsg ("TRIG ERROR: no pool buffer 
available\n"); return; } rol->dabufp = (long *) &__the_event__->length; if (input_event__) { 
__the_event__->nevent = input_event__->nevent; } else { __the_event__->nevent = *(rol->nevents); } } } ; 
StartOfEvent[event_depth__++] = (rol->dabufp); if(input_event__) {	*(++(rol->dabufp)) = (( EVTYPE ) << 16) | (( 0x10 ) << 8) | 
(0xff & (input_event__->nevent));	} else {	*(++(rol->dabufp)) = (syncFlag<<24) | (( EVTYPE ) << 
16) | (( 0x10 ) << 8) | (0xff & *(rol->nevents));	}	((rol->dabufp))++;} ; {	long *StartOfBank; StartOfBank = (rol->dabufp); *(++(rol->dabufp)) = ((( 0x0212 ) << 16) | 
( 0x01 ) << 8) | ( 0 );	((rol->dabufp))++; ; { 
 int nwrds=0;
 int i;
 int index;
 int buf_pointer;
 int read_pointer;
 int buf_len;
 int sub_page;
 int sub_index;
 int sub_length;
 int* mem_loc;
 stat = sis3320Finished(0 );
 stat = 1;
 if(stat>>0 && current_mode == 1 ){
    sis3320Disable(0);
    *rol->dabufp++= 0 ;
    *rol->dabufp++=0x000020002;    
    buf_pointer=sis3320GetBufLength(0 ); 
    *rol->dabufp++=buf_pointer;   
    *rol->dabufp++= 512 ;           
    *rol->dabufp++=sis3320GetActualNumEvents(0 );
    sis3320SetupMapping(0 , 0 , 0, 512 ,
        &sub_page, &sub_index, &sub_length, &mem_loc);
    read_pointer = buf_pointer / 2;
    for (i=0;i<sub_length/2;i++){
        *rol->dabufp++=mem_loc[read_pointer+sub_index];
        read_pointer++;
        if(read_pointer>= 512 /2) read_pointer=0;
    }
} else if(current_mode == 0) {
        *rol->dabufp++ = 0xaaaaaaaa;
} else {
     *rol->dabufp++ = 0xda0000da;
logMsg("ERROR: FADC Samples not ready sis3320Status = 0x%x\n",stat,0,0,0,0,0); }
 } 
*StartOfBank = (long) (((char *) (rol->dabufp)) - ((char *) StartOfBank));	if 
((*StartOfBank & 1) != 0) { (rol->dabufp) = ((long *)((char *) (rol->dabufp))+1); *StartOfBank += 1; }; if 
((*StartOfBank & 2) !=0) { *StartOfBank = *StartOfBank + 2; (rol->dabufp) = ((long *)((short *) 
(rol->dabufp))+1);; };	*StartOfBank = ( (*StartOfBank) >> 2) - 1;}; ; {	long *StartOfBank; StartOfBank = (rol->dabufp); *(++(rol->dabufp)) = ((( 0x0211 ) << 16) | 
( 0x01 ) << 8) | ( 0 );	((rol->dabufp))++; ; { 
 int i;
 if(stat>0 && current_mode == 0) {
   *rol->dabufp++ = 0 ;
   for (i=0; i<6; i++){
      *rol->dabufp++ = sis3320GetNumAcc(0, 0 >>1, 0 %2, i);
      *rol->dabufp++ = sis3320GetAccum(0,0 >>1,0 %2,i,1);
      *rol->dabufp++ = sis3320GetAccum(0,0 >>1,0 %2,i,0);
   }
   *rol->dabufp++ = sis3320GetDac(0, 0 );
   *rol->dabufp++ = sis3320GetThresh(0,0 ,1);
   *rol->dabufp++ = sis3320GetThresh(0,0 ,2);
   *rol->dabufp++ = sis3320GetN5N6(0,0 );
} else if(current_mode == 1) {
        *rol->dabufp++ = 0xaaaaaaaa;
} else {
     *rol->dabufp++ = 0xda0000da;
logMsg("ERROR: FADC Accum not ready sis3320Status = 0x%x\n",stat,0,0,0,0,0); }
 } 
*StartOfBank = (long) (((char *) (rol->dabufp)) - ((char *) StartOfBank));	if 
((*StartOfBank & 1) != 0) { (rol->dabufp) = ((long *)((char *) (rol->dabufp))+1); *StartOfBank += 1; }; if 
((*StartOfBank & 2) !=0) { *StartOfBank = *StartOfBank + 2; (rol->dabufp) = ((long *)((short *) 
(rol->dabufp))+1);; };	*StartOfBank = ( (*StartOfBank) >> 2) - 1;}; ; {	long *StartOfBank; StartOfBank = (rol->dabufp); *(++(rol->dabufp)) = ((( 0x0210 ) << 16) | 
( 0x01 ) << 8) | ( 0 );	((rol->dabufp))++; ; { 
   *rol->dabufp++ = getRampDelayHAPTB();
   *rol->dabufp++ = getIntTimeHAPTB();
   tir_dat = tir[1]->tir_dat; 
   *rol->dabufp++ = (tir_dat & 0xfff);
   if (1  && scaen560[0] != 0) {
      *rol->dabufp++ = 0xcae56000;   
      for (jj = 0; jj<16; jj++) *rol->dabufp++ = scaen560[0]->CntR[jj];
      current_mps = scaen560[0]->CntR[5];  
   }
  event_counter++;
  if( current_mode ==0 && event_counter>= 3600  ){
     current_mode=1;
     event_counter=0;
     starting_mps=current_mps;   
     tir[1]->tir_oport=0;
     sis3320StopDelay(0,300 );
     logMsg("Switching to Triggered Mode");
  }else if (current_mode ==1 && (current_mps-starting_mps)>= 1000 ){
     current_mode=0;
     event_counter=0;
     starting_mps= current_mps;
     tir[1]->tir_oport=0xffff;
     sis3320StopDelay(0,0 );
     logMsg("Switching to Accum Mode");
  }
  if( current_mode  == 0){    
     sis3320Disable(0 );
     sis3320Enable(0 );
  } else {
     sis3320Disable(0 );   
     sis3320Enable(0 );
     sis3320StartSampling(0 );   
  }
 } 
*StartOfBank = (long) (((char *) (rol->dabufp)) - ((char *) StartOfBank));	if 
((*StartOfBank & 1) != 0) { (rol->dabufp) = ((long *)((char *) (rol->dabufp))+1); *StartOfBank += 1; }; if 
((*StartOfBank & 2) !=0) { *StartOfBank = *StartOfBank + 2; (rol->dabufp) = ((long *)((short *) 
(rol->dabufp))+1);; };	*StartOfBank = ( (*StartOfBank) >> 2) - 1;}; ; {event_depth__--; *StartOfEvent[event_depth__] = (long) (((char *) (rol->dabufp)) - 
((char *) StartOfEvent[event_depth__]));	if ((*StartOfEvent[event_depth__] & 1) != 0) { 
(rol->dabufp) = ((long *)((char *) (rol->dabufp))+1); *StartOfEvent[event_depth__] += 1; }; if 
((*StartOfEvent[event_depth__] & 2) !=0) { *StartOfEvent[event_depth__] = *StartOfEvent[event_depth__] + 2; 
(rol->dabufp) = ((long *)((short *) (rol->dabufp))+1);; };	*StartOfEvent[event_depth__] = ( 
(*StartOfEvent[event_depth__]) >> 2) - 1;}; ;   }   
}  
void usrtrig_done()
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
