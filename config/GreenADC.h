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
#include "../adc16/HAPADC_cf_commands.h"

#define GA_MAXADC 20

#define GAINRADIO 100
#define DACRADIO  200

#define GM_ADC_GET 101
#define GM_ADC_SET 201

#define DACON 1
#define DACOFF 2

#define GAINLO 1
#define GAINHI 2

class GreenADC : public TGCompositeFrame {

  //  RQ_OBJECT()

  //  ClassDef(GreenADC, 0) 

 public:


  GreenADC(Int_t crate_number, const char * crate_name,
	  const TGWindow *p, UInt_t w, UInt_t h);

  ~GreenADC();
  
  void Init(ULong_t);
  Bool_t ProcessMessage(Long_t, Long_t, Long_t);
  Int_t getNumADC();

 private:

  Int_t crateNumber;
  TString *crateName;

  Int_t fNumADC;
  Int_t fLabel[GA_MAXADC];
  Int_t fGain[GA_MAXADC];
  Int_t fNewGain[GA_MAXADC];
  Int_t fDAC[GA_MAXADC];
  Int_t fNewDAC[GA_MAXADC];
  TGRadioButton *fGainRB[2][GA_MAXADC];
  TGRadioButton *fDACRB[2][GA_MAXADC];
  TGLabel *fSetLab[GA_MAXADC];

  Int_t getLabelADC(Int_t);
  Bool_t getValsADC();
  Bool_t getValADC(Int_t);
  Bool_t setValsADC();
  Int_t setValADC(Int_t);

  void DoRadio(Int_t id);
  void ResetRadio();

};
