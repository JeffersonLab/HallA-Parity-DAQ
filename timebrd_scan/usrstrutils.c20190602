#ifndef _USRSTRUTILS_INCLUDED
#define _USRSTRUTILS_INCLUDED
/*#include <limits.h>*/
#define LONG_MAX 0x7FFFFFFF

/* usrstrutils

   utilities to extract information from configuration string passed
   to ROC in the *.config file in rcDatabase.

   The config line can be of the form

   keyword[=value][,keyword[=value]] ...

   CRL code can use the following three routines to look for keywords and
   the associated values.

   int getflag(char *s) - Return 0 if s not present as a keyword
                                 1 if keyword is present with no value
				 2 if keyword is present with a value
   int getint(char *s) - If keyword present, interpret value as an integer.
                         Value assumed deximal, unless preceeded by 0x for hex
			 Return 0 if keyword not present or has no value.

   char *getstr(char *s) - Return ptr to string value associated with
                           the keyword.  Return null if keyword not present.
			   return null string if keyword has no value.
			   Caller must free the string.
	     
*/
/* Define some common keywords as symbols, so we have just one place to
   change them*/
#define LEFTHRS "lefthrs"
#define RIGHTHRS "righthrs"
#define COUNTHOUSE "counthouse"
#define PVDISLEFT "pvdisleft"
#define PVDISRIGHT "pvdisright"
#define NOCNTHOUSE "nocnthouse"
#define QWKINJ "qwkinj"
#define INJ "inj"
#define EVTYPEFIX "evtypefix"
#define NOSCALER "noscaler"
#define SCALER2 "scaler2"
#define BMWSCAN "bmwscan"
#define TBREADOUT "tbreadout"
#define FLEXIO "flexio"
#define FLEXIO2 "flexio2"
#define QWEAKADC "qweakadc"
#define ADC18BIT "adc18bit"
#define ADC16BIT "adc16bit"
#define MTREAD "mtread"  /* empty readout (for test) */
#define RAMPDAC12 "rampdac12"
#define RAMPDAC16 "rampdac16"
#define TSCONTROL "tscontrol"
#define AUTOLED "autoled"
#define FLAG_FILE "ffile"
#define COMMENT_CHAR ';'

#define VQWK_SAMPLE_PERIOD  "vqwkperiod"
#define VQWK_NUM_BLOCKS     "vqwkblocks"
#define VQWK_SAMP_PER_BLOCK "vqwksamples"
#define VQWK_GATE_DELAY     "vqwkdelay"
#define VQWK_INT_GATE_FREQ  "vqwkgatefreq"
#define VQWK_INTERNAL_MODE  "vqwkinternal"

#define VQWK_DEFAULT_HAPTB_RAMP_DELAY_CH      "vqwk_default_haptb_ramp_delay_CH"
#define VQWK_DEFAULT_HAPTB_RAMP_DELAY_INJ     "vqwk_default_haptb_ramp_delay_INJ"
#define VQWK_DEFAULT_HAPTB_INTEGRATE_TIME_CH  "vqwk_default_haptb_integrate_time_CH"
#define VQWK_DEFAULT_HAPTB_INTEGRATE_TIME_INJ "vqwk_default_haptb_integrate_time_INJ"

#define VQWK_VERBOSE_MODE   "vqwk_verbose"

#ifndef INTERNAL_FLAGS
#define INTERNAL_FLAGS ""
#endif

char *internal_configusrstr=0;
char *file_configusrstr=0;

/* For internal use. Returns ptr to keyword and ptr to value */
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
  end = strchr(val,',');	/* value string ends at next keyword */
  if(end)
    slen = end - val;
  else				/* No more keywords, value is rest of string */
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
  if(!sval) return(0);		/* Just return zero if no value string */
  retval = strtol(sval,0,0);
  if(retval == LONG_MAX && (sval[1]=='x' || sval[1]=='X')) {/* Probably hex */
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
    if(pos) {			/* Make sure it is really the keyword */
      /* Make sure it is an isolated keyword */
      if((pos != constr && pos[-1] != ',') ||
          (pos[slen] != '=' && pos[slen] != ',' && pos[slen] != '\0')) {
        pos += 1;	continue;
      } else break;		/* It's good */
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
  /* Regexp would be nice
     Look for string s in file_configusrstr, config.usrString, and then
     internal_configusrstr, using the first file it is found in.

     s must occur after a ",", or
     at the start of config.usrString.  s must be followed by "," or "=" or
     null.  (No spaces are allowed in config strings.)
     */
  getflagpos_instring(file_configusrstr,s,pos_ret,val_ret);
  if(*pos_ret) return;
  getflagpos_instring(rol->usrString,s,pos_ret,val_ret);
  if(*pos_ret) return;
  getflagpos_instring(internal_configusrstr,s,pos_ret,val_ret);
  return;
}

void init_strings()
     /* Load/reload config line from user flag file. */
{
  char *ffile_name;
  int fd;
  char s[256], *flag_line;

  if(!internal_configusrstr) {	/* Internal flags not loaded */
    internal_configusrstr = (char *) malloc(strlen(INTERNAL_FLAGS)+1);
    strcpy(internal_configusrstr,INTERNAL_FLAGS);
  }
/*  daLogMsg("Internal Config: %s\n",internal_configusrstr); */
/*  daLogMsg("rcDatabase Conf: %s\n",rol->usrString);  */

  ffile_name = getstr(FLAG_FILE);
/* check that filename exists */
  fd = fopen(ffile_name,"r");
  if(!fd) {
/*    printf("Failed to open usr flag file %s\n",ffile_name); */
    free(ffile_name);
    if(file_configusrstr) free(file_configusrstr); /* Remove old line */
    file_configusrstr = (char *) malloc(1);
    file_configusrstr[0] = '\0';
  } else {
    /* Read till an uncommented line is found */
    flag_line = 0;
    while(fgets(s,255,fd)){
      char *arg;
      arg = strchr(s,COMMENT_CHAR);
      if(arg) *arg = '\0'; /* Blow away comments */
      arg = s;			/* Skip whitespace */
      while(*arg && isspace(*arg)){
	arg++;
      }
      if(*arg) {
	flag_line = arg;
	break;
      }
    }
    if(file_configusrstr) free(file_configusrstr); /* Remove old line */
    if(flag_line) {		/* We have a config usrstr */
      file_configusrstr = (char *) malloc(strlen(flag_line)+1);
      strcpy(file_configusrstr,flag_line);
    } else {
      file_configusrstr = (char *) malloc(1);
      file_configusrstr[0] = '\0';
    }
    fclose(fd);
    free(ffile_name);
  }
/*  daLogMsg("Run time Config: %s\n",file_configusrstr); */
}


void load_strings_from_file(char *input_name)
     /* Load/reload config line from user flag file. */
{
  char *ffile_name;
  int fd;
  char s[256], *flag_line;

  if(!internal_configusrstr) {	/* Internal flags not loaded */
    internal_configusrstr = (char *) malloc(strlen(INTERNAL_FLAGS)+1);
    strcpy(internal_configusrstr,INTERNAL_FLAGS);
  }
/*  daLogMsg("Internal Config: %s\n",internal_configusrstr); */
/*  daLogMsg("rcDatabase Conf: %s\n",rol->usrString);  */

  ffile_name = (char *) malloc(strlen(input_name)+1);
  strcpy(ffile_name,input_name);
/* check that filename exists */
  fd = fopen(ffile_name,"r");
  if(!fd) {
    printf("Failed to open usr flag file %s\n",ffile_name); 
    free(ffile_name);
    if(file_configusrstr) free(file_configusrstr); /* Remove old line */
    file_configusrstr = (char *) malloc(1);
    file_configusrstr[0] = '\0';
  } else {
    /* Read till an uncommented line is found */
    flag_line = 0;
    while(fgets(s,255,fd)){
      char *arg;
      arg = strchr(s,COMMENT_CHAR);
      if(arg) *arg = '\0'; /* Blow away comments */
      arg = s;			/* Skip whitespace */
      while(*arg && isspace(*arg)){
        arg++;
      }
      if(*arg) {
        flag_line = arg;
        break;
      }
    }
    if(file_configusrstr) free(file_configusrstr); /* Remove old line */
    if(flag_line) {		/* We have a config usrstr */
      file_configusrstr = (char *) malloc(strlen(flag_line)+1);
      strcpy(file_configusrstr,flag_line);
    } else {
      file_configusrstr = (char *) malloc(1);
      file_configusrstr[0] = '\0';
    }
    fclose(fd);
    free(ffile_name);
  }
  daLogMsg("Run time Config: %s\n",file_configusrstr); 
}
      

/*********************************************************
* Just do the same as above, but now also print 
* all of the original file into a new copy of it
* and:
*      if line containing flag != auto-gen
*        Comment it out, copy it, change its desired
*        parameter, and put it in the auto-gen line at EOF
*      if the file doesn't exist to begin with
*        Make a new one and put this as auto-gen line at EOF
*        and print at top of file a brief description
**********************************************************/
  
void edit_strings_in_file(char *input_name, char *edit_name, char* ffile_name)
     /* Update confline from user flag file. */
{
  char * ffile_name_write = "/tmp/tmp_usr_str_utils_print_file.txt";
  int fd;
  char s[256], *flag_line;
  bool newFile = false;
  bool autoGen = false;

  if(!internal_configusrstr) {	/* Internal flags not loaded */
    internal_configusrstr = (char *) malloc(strlen(INTERNAL_FLAGS)+1);
    strcpy(internal_configusrstr,INTERNAL_FLAGS);
  }
/*  daLogMsg("Internal Config: %s\n",internal_configusrstr); */
/*  daLogMsg("rcDatabase Conf: %s\n",rol->usrString);  */

  ffile_name = (char *) malloc(strlen(input_name)+1);
  strcpy(ffile_name,input_name);
/* check that filename exists */
  fd = fopen(ffile_name,"r");
  fd_write = fopen(ffile_name_write,"w");
  if(!fd) {
    newFile = true;
    printf("Failed to open usr flag file %s\n",ffile_name); 
    free(ffile_name);
    if(file_configusrstr) free(file_configusrstr); /* Remove old line */
    file_configusrstr = (char *) malloc(1);
    file_configusrstr[0] = '\0';
    fd_write<<";This is a CRL readable configuration file for the Hall A parity DAQ\n";
    fd_write<<";Each new line with a colon at the beginning is a comment and not read, whitespace is ignored\n";
    fd_write<<";The flags are parsed by usrstrutils.c in CRLs and GreenMonster for configuring the DAQ\n";
    fd_write<<";Examples of flags in use are haptb_ramp_delay_CH, vqwk_num_samples, etc.\n";
    fd_write<<";Each flag should be specified as a CSV, = sign pair: flag_name=flag_value,flag_n2=flag_val2, etc.\n";
    fd_write<<"AUTO_GENERATED_CONTENT=true,"<<input_name<<","<<input_value<<"\n";
  } 
  else {
    /* Read till an uncommented line is found */
    char *pos,*val;
    print_line = 0;
    flag_line = 0;
    while(fgets(s,255,fd)){
      char *arg;
      arg = strchr(s,COMMENT_CHAR);
      if(arg) {
        *arg = '\0'; /* Blow away comments in input_name search */
        fd_write<<s; /* Preserve comments in file printing */
      }

      else {
        getstr(s,"AUTO_GENERATED_CONTENT",&pos,&val)
        if (val == "true"){ /* FIXME need to verify method to get string in CSV */
          autoGen = true; /* Don't print this line to file yet - needs to be updated */
        }
        else {
          fd_write<<";"<<s; /* Preserve the uncommented line, but comment it out if not auto-gen EOF*/
        }
      }
      arg = s;			/* Skip whitespace */
      while(*arg && isspace(*arg)){
        arg++;
      }
      if(*arg) {
        flag_line = arg;
        /*break;*/
      }
      print_line = arg; /* Just save the whole line, after pruning white space */
    }
    if(print_line && autoGen) { /* Then we found the string we wanted in the old file and should edit it in EOF auto-gen */
      if (input_name in print_line){
        char * new_print_line;
        editflag_instring(input_name,input_value,print_line,new_print_line); /* FIXME need method to edit just the entry & strip \n off*/
        fd_write<<new_print_line<<"\n";
      }
      else {
        fd_write<<print_line<<","<<input_name<<"="<<input_value<<"\n";
      }
    }
    if(print_line && !autoGen) { /* Then we found the string we wanted in the old file and should make a new EOF auto-gen */
      if (input_name in print_line){
        char * new_print_line;
        editflag_instring(input_name,input_value,print_line,new_print_line); /* FIXME need method to edit just the entry & strip \n off*/
        fd_write<<"AUTO_GENERATED_CONTENT=true,"<<new_print_line<<"\n";
      }
      else {
        fd_write<<"AUTO_GENERATED_CONTENT=true,"<<print_line<<","<<input_name<<"="<<input_value<<"\n";
    }
    if (!print_line && autoGen) { /* Then we didn't find it and should add it in EOF auto-gen */
      fd_write<<input_name<<"="<<value_name<<"\n";
    }
    if (!print_line && !autoGen) { /* Then we didn't find it and there was no autoGen line, so make one */
      fd_write<<"AUTO_GENERATED_CONTENT=true,"<<input_name<<"="<<value_name<<"\n";
    }
    if(file_configusrstr) free(file_configusrstr); /* Remove old line */
    if(flag_line) {		/* We have a config usrstr */
      file_configusrstr = (char *) malloc(strlen(flag_line)+1);
      strcpy(file_configusrstr,flag_line);
    } else {
      file_configusrstr = (char *) malloc(1);
      file_configusrstr[0] = '\0';
    }
    fclose(fd);
    free(ffile_name);
  }
  daLogMsg("Run time Config: %s\n",file_configusrstr); 
}

void editflag_instring(char *name_edit, char *val_edit, char *s,char **val_ret)
{
  int slen,nlen,vlen,old_vlen; /* The trick is to find the old value and find its position and length */
  char *pos,*val;

  slen=strlen(s); /* The previous version of this method assumed that s is a sub-string, not the whole line */
  nlen=strlen(name_edit);
  vlen=strlen(val_edit);
  pos = name_edit;
  while(pos) {
    pos = strstr(s,pos);
    if(pos) {			/* Make sure it is really the keyword */
      /* Make sure it is an isolated keyword */
      if((pos != name_edit && pos[-1] != ',') ||
          (pos[nlen] != '=' && pos[nlen] != ',' && pos[nlen] != '\0')) {
        pos += 1;	continue;
      } else break;		/* It's good */
    }
  }
  /* Loop over s and find the  */
  /* for each entry in s < begining of name_edit=val_edit save that value to val_ret
   * for each entry in s >= beginning of name_edit=val_edit and < it + vlen save val_edit[entry] to val_ret
   * for each entry in s >= beginning of name_edit=val_edit + vlen save the original entry that was at each entry beginning at name_edit=val_edit + slen and going to nlen */
  /* Find the contents of the file to the right of this old one */
  char* old_pos,old_val,rest_val;
  getflagpos_instring(s,name_edit,old_pos,old_val);
  char* old_flag_value_str;
  old_flag_value_str = getstr(name_edit);
  rest_val=strstr(old_pos,old_val); /* strstr is safe here because old_pos has already sliced off front of file 
                                     * Need to get length of old_val and subtract it off front of rest_val */
  if (rest_val){
    if (rest_val[strlen(old_flag_value_str)] == ',' ) { /* Then we have more file to go */
      *rest_val = rest_val + strlen(old_flag_value_str) + 1;
    }
  }
  int len_initial = strlen(s)-strlen(pos)
  int len_val_ret = len_initial + vlen + strlen(rest_val);
  for (int i = 0 ; i<len_val_ret ; i++) {
    if (i < len_initial) {
      val_ret[i] = s[i];
    }
    else if (i < len_initial+vlen) {
      val_ret[i] = val_edit[i-len_intial];
    }
    else {
      val_ret[i] = rest_val[i-len_initial-vlen];
    }
  }
  return;
}
  
      
#endif /* _USRSTRUTILS_INCLUDED */

