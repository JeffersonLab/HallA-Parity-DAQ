#ifndef THaEtClient_
#define THaEtClient_

//////////////////////////////////////////////////////////////////////
//
//   THaEtClient
//   Data from ET Online System
//
//   THaEtClient contains normal CODA data obtained via
//   the ET (Event Transfer) online system invented
//   by the JLab DAQ group. 
//   This code works locally or remotely and uses the
//   ET system in a particular mode favored by  hall A.
//
//   Robert Michaels (rom@jlab.org)
//
/////////////////////////////////////////////////////////////////////

#include "THaCodaData.h"

#define ET_CHUNK_SIZE 500
#ifndef __CINT__
#include "et.h"
#endif

class TString;

// The ET memory file will have this prefix.  The suffix is $SESSION.
#define ETMEM_PREFIX "/tmp/et_sys_"

// Hall A computers that run CODA/ET
#define ADAQ1 "129.57.192.102"
#define ADAQ2 "129.57.192.103"
#define ADAQ3 "129.57.192.104"


class THaEtClient : public THaCodaData 
{

public:

    THaEtClient(int mode=1);      // By default, gets data from ADAQ1
// find data on 'computer'.  e.g. computer="129.57.192.102"
    THaEtClient(const char* computer, int mode=1);  
    THaEtClient(const char* computer, const char* session, int mode=1);  
    int codaOpen(const char* computer, int mode=1);
    int codaOpen(const char* computer, const char* session, int mode=1);
    int codaClose();
    ~THaEtClient();
    int *getEvBuffer()         // Gets next event buffer after codaRead()
      { return evbuffer; }
    int codaRead();            // codaRead() must be called once per event
    virtual bool isOpen() const;

private:

    THaEtClient(const THaEtClient &fn);
    THaEtClient& operator=(const THaEtClient &fn);
    int CHUNK;
    int DEBUG; 
    int FAST; 
    int SMALL_TIMEOUT; 
    int BIG_TIMEOUT; 
    int nread, nused, timeout;
#ifndef __CINT__
    et_sys_id id;
    et_statconfig sconfig;
    et_stat_id my_stat;
    et_att_id my_att;
    et_openconfig openconfig;
#endif
    char *daqhost,*session,*etfile;
    int waitflag,didclose,notopened,firstread;
    void initflags();
    int init(const char* computer="aparSta");

#ifndef STANDALONE
    ClassDef(THaEtClient,0)   // ET client connection for online data
#endif

};

#endif








