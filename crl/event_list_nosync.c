/* # 1 "event_list_nosync.c" */
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
{{ ( input_event__ ) = 0; if (( &( rol->input ->list) )->c){ ( &( rol->input ->list) )->c--; ( 
input_event__ ) = ( &( rol->input ->list) )->f; ( &( rol->input ->list) )->f = ( &( rol->input ->list) )->f->n; 
}; if (!( &( rol->input ->list) )->c) { ( &( rol->input ->list) )->l = 0; }} ;} ;       rol->dabufpi = (long *) &input_event__->length;
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
void event_list_nosync__init (rolp)
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
	  strcpy(rol_name__, "ROL2" );
	  rolp->listName = rol_name__;
	  printf("Init - Initializing new rol structures for %s\n",rol_name__);
	  strcpy(name, rolp->listName);
	  strcat(name, ":pool");
	  rolp->pool  = partCreate(name, 128  , 10 ,1);
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
/* # 9 "event_list_nosync.c" 2 */
/* # 1 "/adaqfs/coda/2.5/common/include/EVENT_source.h" 1 */
static int EVENT_handlers,EVENTflag;
static int EVENT_isAsync;
static unsigned long *INPUT;
static void 
eventtenable(int val)
{
}
static void 
eventtdisable(int val)
{
}
static unsigned long 
eventttype()
{
  return(1);
}
static int 
eventttest(){
  int tval=0;
  if ((rol->input->list.c) > 0)
    tval = 1;
  return(tval);
}
/* # 10 "event_list_nosync.c" 2 */
static void __download()
{
    daLogMsg("INFO","Readout list compiled %s", DAYTIME);
   rol->poll = 1;
    *(rol->async_roc) = 0;  
  {   
    daLogMsg("INFO","User Download 2 Executed");
  }   
}       
static void __prestart()
{
{ dispatch_busy = 0; bzero((char *) evMasks, sizeof(evMasks)); bzero((char *) syncTRtns, 
sizeof(syncTRtns)); bzero((char *) ttypeRtns, sizeof(ttypeRtns)); bzero((char *) Tcode, sizeof(Tcode)); 
wrapperGenerator = 0; theEvMask = 0; currEvMask = 0; trigId = 1; poolEmpty = 0; __the_event__ = (DANODE *) 0; 
input_event__ = (DANODE *) 0; } ;     *(rol->nevents) = 0;
  {   
unsigned long jj, adc_id;
    daLogMsg("INFO","Entering User Prestart 2");
    {EVENT_handlers = 0;EVENT_isAsync = 0;EVENTflag = 0;} ;
{ void davetrig ();void davetrig_done (); doneRtns[trigId] = (FUNCPTR) ( davetrig_done ) ; 
trigRtns[trigId] = (FUNCPTR) ( davetrig ) ; syncTRtns[trigId] = (FUNCPTR) eventttest ; Tcode[trigId] = ( 1 ) ; 
ttypeRtns[trigId] = eventttype ; {printf("linking sync EVENT trigger to id %d \n", trigId ); EVENT_handlers = ( 
trigId );EVENT_isAsync = 0;} ;trigId++;} ;     {evMasks[ 1 ] |= (1<<( EVENT_handlers ));} ;
  rol->poll = 1;
    daLogMsg("INFO","User Prestart 2 executed");
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
    daLogMsg("INFO","User End 2 Executed");
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
    daLogMsg("INFO","User Pause 2 Executed");
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
    daLogMsg("INFO","Entering User Go 2");
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
void davetrig(unsigned long EVTYPE,unsigned long EVSOURCE)
{
    long EVENT_LENGTH;
  {   
int ii;
if(input_event__) { EVENT_LENGTH = *rol->dabufpi++ - 1; EVTYPE = 
(((*rol->dabufpi)&0xff0000)>>16); if (__the_event__) __the_event__->nevent = input_event__->nevent; rol->dabufpi++; 
INPUT = rol->dabufpi; } ; { 
 if (rol->dabufp != ((void *) 0) ) {           
   for (ii=-2;ii<EVENT_LENGTH;ii++)   
   if(ii==-1) {
       *rol->dabufp++ = (INPUT[ii])&0xfeffffff;
   } else {
       *rol->dabufp++ = INPUT[ii];
   }
 }else{
   printf("ROL2: ERROR rol->dabufp is NULL -- Event lost\n");
 }
 } 
  }   
}  
void davetrig_done()
{
  {   
  }   
}  
void __done()
{
poolEmpty = 0;  
  {   
  }   
}  
static void __status()
{
  {   
  }   
}  
