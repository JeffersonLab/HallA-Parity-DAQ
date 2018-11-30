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
#include "../timebrd/HAPTB_cf_commands.h"

#define GM_HAPTB   1000
#define GM_TB_GET  1001
#define GM_TB_SET  1002

class GreenTB : public TGCompositeFrame {

//    RQ_OBJECT()

//    ClassDef(GreenTB, 0) 

 public:


  GreenTB(Int_t crate_number, const char * crate_name,
	  const TGWindow *p, UInt_t w, UInt_t h);

  ~GreenTB();
  
  void Init(ULong_t);
  Bool_t ProcessMessage(Long_t, Long_t, Long_t);

 private:

  Int_t crateNumber;
  TString *crateName;
  
  Int_t CurrentRD, CurrentIT, CurrentOS;

  // objects for timebrd tabpage
  TGTextEntry  *tentRD, *tentIT, *tentOS;

  Bool_t getValsTB();
  Bool_t setValsTB();
  Int_t setParTB(const int parNum, const int value);

};
