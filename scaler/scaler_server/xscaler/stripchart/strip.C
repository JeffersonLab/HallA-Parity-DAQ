//    Strip Chart for PVDIS Scalers.    
//    R. Michaels   Oct 2009
//
//    To show some scaler quantities and ratios of scalers.
//    Run this as follows from the root prompt:
//        .x strip.C
//    and don't forget the dot(.) before the x


#include "strip.h"

THaStripChart::THaStripChart(int upd) :
  scaler(NULL), ntup(NULL), farray_ntup(NULL), canv(NULL),
  ntsize(0), updtime(upd), iclear(0), arm("none")
{
  // constructor
}


THaStripChart::~THaStripChart() 
{
  if (scaler) delete scaler;
  if (ntup) delete ntup;
  if (farray_ntup) delete farray_ntup;
  if (canv) delete canv;
}

THaStripChart::Init(string sarm) 
{
  arm = sarm;
  iloop = 0;
  gSystem->Load("libdc.so");
  gSystem->Load("libscaler.so");
  scaler = new THaScaler(arm.c_str());
  if (scaler->Init() == -1) {
    cout << "Error initializing scalers !!"<<endl;
    exit(0);
  }
  if (arm == "Left") {
    cout << "Setting up Left ARM scalers "<<endl;
    scaler->SetClockLoc(1,29);  // for Left ARM
    scaler->SetClockRate(103800);
  } else {
    cout << "Setting up Right ARM scalers "<<endl;
    scaler->SetClockLoc(1,31);  // for Right ARM
    scaler->SetClockRate(103800);
  }
  // Get an initial reading, then delay.
  if (scaler->LoadDataOnline() == -1) {
     cout << "Failed to obtain data from VME crate.  Stopping"<<endl;
     exit(0);
  }
  Int_t dum = 0;
  for (Int_t i = 0; i < 2000000; i++) dum++;  // delay
  char string_ntup[]="time:bcm_u3:bcm_u10:ewid1:enar1:piwid1:pinar1";
  ntsize = 7;  // this must match the length of string_ntup;
  ntup = new TNtuple("ascal","Scaler Data",string_ntup);
  farray_ntup = new Float_t[ntsize];
  Clear();
  InitPlots();
}

THaStripChart::InitPlots()
{
  char comment[80];
  sprintf(comment,"SCALER STRIP CHART   (Recent time at right, updates each %d sec)",updtime);
  canv = new TCanvas("c1",comment,200,10,1200,600);
  canv->Divide(2,3);
  Update();
}

THaStripChart::Update()
{
   if (!scaler) return;
   //   if (CheckClear()) Clear();
   if (scaler->LoadDataOnline() == -1) {
     cout << "Failed to obtain data from VME crate.  Stopping"<<endl;
     exit(0);
   }
   iloop++;
   UpdateNtuple(); 
   UpdatePlots();
}

Int_t THaStripChart::CheckClear()
{
  FILE *fd;
  char file[]="clear.ctl";
  fd = fopen(file,"r");
  char clearstat[10];
  if (fd == NULL) return 0;
  fgets(clearstat,10,fd);
  iclear = atoi(clearstat);
  strcpy(clearstat,"0");
  fclose(fd);
  fd = fopen(file,"w");
  if (fd == NULL) return iclear;
  fputs(clearstat,fd);
  fclose(fd);
  return iclear;
}

THaStripChart::Clear() 
{
  memset(farray_ntup,0,ntsize);
  ntup->Reset();
  iloop = 0;
}


THaStripChart::UpdateNtuple()
{
   if (ntsize <= 0 || farray_ntup == NULL) return;

   // Things to plot
   Float_t bcm_u3, bcm_u10;
   Float_t elec_wide1, elec_narrow1;
   Float_t pion_wide1, pion_narrow1;
   
   if (arm == "Left") {
      bcm_u3 = scaler->GetScalerRate(0,0);       // slot, chan
      bcm_u10 = scaler->GetScalerRate(0,24);     // I am NOT claiming these are right (Oct 7, 2009)
      elec_wide1 = scaler->GetScalerRate(1,30);
      elec_narrow1 = scaler->GetScalerRate(1,13);
      pion_wide1 = scaler->GetScalerRate(1,16);
      pion_narrow1 = scaler->GetScalerRate(1,29);
      time = scaler->GetScalerRate(1,29);
   } else {  // Right HRS
      bcm_u3 = scaler->GetScalerRate(0,0);       // slot, chan
      bcm_u10 = scaler->GetScalerRate(0,14);
      elec_wide1 = scaler->GetScalerRate(1,16);
      elec_narrow1 = scaler->GetScalerRate(1,25);
      pion_wide1 = scaler->GetScalerRate(0,5);
      pion_narrow1 = scaler->GetScalerRate(1,22);
      time = scaler->GetScalerRate(1,31);
   }

// to debug:   scaler->Print();

   // farray[] much track with string_ntup[]

   farray_ntup[0] = (float)updtime * iloop;
   farray_ntup[1] = bcm_u3;
   farray_ntup[2] = bcm_u10;
   farray_ntup[3] = elec_wide1;
   farray_ntup[4] = elec_narrow1;
   farray_ntup[5] = pion_wide1;
   farray_ntup[6] = pion_narrow1;

   if (ntup) ntup->Fill(farray_ntup);

}

THaStripChart::UpdatePlots()
{
  if (!ntup || !canv) return;  

  char cutcomm[50];
  // cut on time (e.g. 600 = 10 minutes)
  sprintf(cutcomm,"%d-time<600",updtime*iloop); 
  // to turn on/off the X axis label "SEC"
  Int_t drawx = 1;

  ntup->SetMarkerColor(4);
  ntup->SetMarkerStyle(21);

  gROOT->Reset();
  gStyle->SetOptStat(0);
  gStyle->SetLabelSize(0.06,"x");
  gStyle->SetLabelSize(0.05,"y");
  gStyle->SetPadBottomMargin(0.16);
  gStyle->SetPadTopMargin(0.1);
  gStyle->SetPadLeftMargin(0.16);
  gStyle->SetPadRightMargin(0.1);
  gStyle->SetTitleH(0.10);
  gStyle->SetTitleW(0.40);
  gStyle->SetTitleOffset(0.3);
  gStyle->SetTitleSize(0.07);

  canv->cd(1);
  ntup->Draw("bcm_u3 : time  ",cutcomm);
  if (drawx) htemp->GetXaxis()->SetTitle("SEC");

  canv->cd(2);
  ntup->Draw("bcm_u10 : time  ",cutcomm);
  if (drawx) htemp->GetXaxis()->SetTitle("SEC");

  canv->cd(3);
  ntup->Draw("ewid1 : time  ",cutcomm);
  if (drawx) htemp->GetXaxis()->SetTitle("SEC");

  canv->cd(4);
  ntup->Draw("enar1 : time  ",cutcomm);
  if (drawx) htemp->GetXaxis()->SetTitle("SEC");

  canv->cd(5);
  ntup->Draw("piwid1 : time  ",cutcomm);
  if (drawx) htemp->GetXaxis()->SetTitle("SEC");

  canv->cd(6);
  ntup->Draw("pinar1 : time  ",cutcomm);
  if (drawx) htemp->GetXaxis()->SetTitle("SEC");

  canv->Update();
}


void strip(string arm)
{  
  // Invoke this script with "root .x strip.C"  

  if (arm != "Left"  && arm != "Right") {
    cout << "Error: can only run with args 'Left' or 'Right'"<<endl;
    cout << "try again "<<endl;
    exit(0);
  }
  cout << "Starting strip chart for  arm = "<<arm<<endl;

  Int_t update_time = 5;  // update time (sec)
  TTimer *timer = new TTimer(update_time*1000); 
  THaStripChart *strip = new THaStripChart(update_time);
  timer->Connect("Timeout()", "THaStripChart", strip, "Update()");
  strip->Init(arm);
  timer->TurnOn();
}

