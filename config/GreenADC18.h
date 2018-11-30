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
#include "TGComboBox.h"
#include "math.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h>


using namespace std;

#include "../cfSock/cfSock_types.h"
#include "../adc18/ADC18_cf_commands.h"

#define GA_MAXADC 20

#define DACRADIO18  100 //

#define GM_ADC_GET 101
#define GM_ADC_SET 201

#define DACTRI   0
#define DACSAW   1
#define DACCONST 2
#define DACOFF18   3 //

using namespace std;

class GreenADC18 : public TGCompositeFrame {

  //  RQ_OBJECT()

  //  ClassDef(GreenADC18, 0) 

 public:


  GreenADC18(Int_t crate_number, const char * crate_name,
	   const TGWindow *p, UInt_t w, UInt_t h);

  ~GreenADC18();
  
  void Init(ULong_t);
  Bool_t ProcessMessage(Long_t, Long_t, Long_t);
  Int_t getNumADC();

 private:

  Int_t crateNumber;
  TString *crateName;

  Int_t fNumADC;

  Int_t fLabel[GA_MAXADC];

  Int_t fIntGain[GA_MAXADC];
  Int_t fNewIntGain[GA_MAXADC];

  Int_t fConvGain[GA_MAXADC];
  Int_t fNewConvGain[GA_MAXADC];
  
  Int_t fDAC[GA_MAXADC];
  Int_t fNewDAC[GA_MAXADC];

  Int_t fSample[GA_MAXADC];

  Int_t fPedDac[4][GA_MAXADC];
  
  TGLabel *fSetLab[GA_MAXADC];
  TGRadioButton *fDACRB[4][GA_MAXADC];
  
  TGTextEntry  *intBox[GA_MAXADC], *convBox[GA_MAXADC];
  
  vector <TGComboBox*> sampleBox;
  
  Int_t getLabelADC(Int_t);


  Bool_t getValADC(Int_t);
  Bool_t getValsADC();

  Bool_t setValADC(Int_t);
  Bool_t setValsADC();

  void CalPed(Int_t);

  void DoRadio(Int_t id);
  void ResetRadio(Int_t);

};
