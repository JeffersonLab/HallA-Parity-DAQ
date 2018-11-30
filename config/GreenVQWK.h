#include <iostream>
#include <stdlib.h>
#include "TGClient.h"
#include "Rtypes.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TObjectTable.h"
#include "RQ_OBJECT.h"
#include "TQObject.h"
#include <TExec.h>
#include "TGFrame.h"
#include "TGLabel.h"
#include "TGTextBuffer.h"
#include "TGTextEntry.h"
#include "TGButton.h"

#include "../cfSock/cfSock_types.h"
#include "GMSock.h"
#include "../qweak/VQWK_cf_commands.h"

#define GM_VQWK   7000
#define GM_VQWK_GET  7001
#define GM_VQWK_SET  7002

class GreenVQWK : public TGCompositeFrame {

//    RQ_OBJECT()

//    ClassDef(GreenVQWK, 0) 

 public:


  GreenVQWK(Int_t crate_number, const char * crate_name,
	  const TGWindow *p, UInt_t w, UInt_t h);

  ~GreenVQWK();
  
  void Init(ULong_t);
  Bool_t ProcessMessage(Long_t, Long_t, Long_t);

 private:

  Int_t crateNumber;
  TString *crateName;
  
  Int_t CurrentSPB, CurrentGD, CurrentNB;

  // objects for timebrd tabpage
  TGTextEntry  *tentSPB, *tentGD, *tentNB;

  Bool_t getValsVQWK();
  Bool_t setValsVQWK(const int index);
  Int_t setParVQWK(const int parNum, const int value,const int index);

};
