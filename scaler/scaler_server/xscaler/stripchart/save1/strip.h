#include <string>
#include <fstream>
#include <iostream>
#include <list>
#include <TMath.h>
#include <TBranch.h>
#include <TGClient.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TROOT.h>
#include <TGImageMap.h>
#include <TKey.h>
#include "TNtuple.h"
#include "TTimer.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"

using namespace std;

class THaScaler;

class THaStripChart  {

public:

  THaStripChart(Int_t upd=5);
  virtual ~THaStripChart();
  void Init();
  void Update();

private:

  void InitPlots();
  void UpdateNtuple();
  void UpdatePlots();
  Int_t CheckClear();
  void Clear();
  TCanvas *canv;
  Float_t *farray_ntup;
  TNtuple *ntup;
  THaScaler *scaler;
  Int_t iloop,updtime,ntsize,iclear;

};
