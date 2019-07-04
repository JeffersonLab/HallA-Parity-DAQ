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
  ntsize(0), updtime(upd), iclear(0)
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

THaStripChart::Init(char *hrs) 
{
  iloop = 0;
  gSystem->Load("libdc.so");
  gSystem->Load("libscaler.so");
  scaler = new THaScaler(hrs);
  if (scaler->Init() == -1) {
    cout << "Error initializing scalers !!"<<endl;
    exit(0);
  }
  if (strstr(hrs,"Left") != NULL) {
    cout << "Setting up Left HRS scalers "<<endl;
    scaler->SetClockLoc(1,29);  // for Left HRS
    scaler->SetClockRate(103800);
  } else {
    cout << "Setting up Right HRS scalers "<<endl;
    scaler->SetClockLoc(1,31);  // for Right HRS
    scaler->SetClockRate(103800);
  }
  // Get an initial reading, then delay.
  if (scaler->LoadDataOnline() == -1) {
     cout << "Failed to obtain data from VME crate.  Stopping"<<endl;
     exit(0);
  }
  Int_t dum = 0;
  for (Int_t i = 0; i < 2000000; i++) dum++;  // delay
  char string_ntup[]="time:bcm_u3:bcm_u10:trig1:trig3:trig5:trig7:CrossSection_T1:CrossSection_T3";
  ntsize = 9;  // this must match the length of string_ntup;
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

   Float_t bcm_u3 = scaler->GetScalerRate(1,0);   // slot, chan
   Float_t bcm_u10 = scaler->GetScalerRate(1,1);
   Float_t t1 = scaler->GetScalerRate(1,30);
   Float_t t3 = scaler->GetScalerRate(0,4);
   Float_t t5 = scaler->GetScalerRate(0,5);
   Float_t t7 = scaler->GetScalerRate(1,29);

   cout << "Chk "<<bcm_u3<<"  "<<bcm_u10<<"  "<<t1<<"  "<<t3<<"  "<<t5<<"  "<<t7<<endl;
   //   scaler->Print();

   Float_t crsec1 = 0;
   Float_t crsec3 = 0;

   if (bcm_u3 > 0) {
     crsec1 = t1/bcm_u3;
     crsec3 = t3/bcm_u3;
   }
   
   farray_ntup[0] = (float)updtime * iloop;
   farray_ntup[1] = bcm_u3;
   farray_ntup[2] = bcm_u10;
   farray_ntup[3] = t1;
   farray_ntup[4] = t3;
   farray_ntup[5] = t5;
   farray_ntup[6] = t7;
   farray_ntup[7] = crsec1;
   farray_ntup[8] = crsec3;

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
  gStyle->SetLabelSize(0.04,"x");
  gStyle->SetLabelSize(0.04,"y");
  gStyle->SetPadBottomMargin(0.16);
  gStyle->SetPadTopMargin(0.1);
  gStyle->SetPadLeftMargin(0.16);
  gStyle->SetPadRightMargin(0.1);
  gStyle->SetTitleH(0.10);
  gStyle->SetTitleW(0.40);
  gStyle->SetTitleOffset(0.4);
  gStyle->SetTitleSize(0.08);

  canv->cd(1);
  ntup->Draw("bcm_u3 : time  ",cutcomm);
  if (drawx) htemp->GetXaxis()->SetTitle("SEC");

  canv->cd(2);
  ntup->Draw("bcm_u10 : time  ",cutcomm);
  if (drawx) htemp->GetXaxis()->SetTitle("SEC");

  canv->cd(3);
  ntup->Draw("trig3 : time  ",cutcomm);
  if (drawx) htemp->GetXaxis()->SetTitle("SEC");

  canv->cd(4);
  ntup->Draw("trig1 : time  ",cutcomm);
  if (drawx) htemp->GetXaxis()->SetTitle("SEC");

  canv->cd(5);
  ntup->Draw("CrossSection_T3 : time  ",cutcomm);
  if (drawx) htemp->GetXaxis()->SetTitle("SEC");

  canv->cd(6);
  ntup->Draw("CrossSection_T1 : time  ",cutcomm);
  if (drawx) htemp->GetXaxis()->SetTitle("SEC");

  canv->Update();
}


void strip(string hrs)
{  
  // Invoke this script with "root .x strip.C"  

  if (hrs != "Left"  && hrs != "Right") {
    cout << "Error: can only run with args 'Left' or 'Right'"<<endl;
    cout << "try again "<<endl;
    exit(0);
  }
  cout << "Starting strip chart for  hrs = "<<hrs<<endl;

  Int_t update_time = 5;  // update time (sec)
  TTimer *timer = new TTimer(update_time*1000); 
  THaStripChart *strip = new THaStripChart(update_time);
  timer->Connect("Timeout()", "THaStripChart", strip, "Update()");
  strip->Init(hrs);
  timer->TurnOn();
}

