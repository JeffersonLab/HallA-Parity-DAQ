//--------------------------------------------------------
//  tscalhist_main.C
//
//  Test of scaler class, for reading scaler history (end-of-run)
//  files and obtaining data.
//  This version also fills an ntuple
//  Hint: if you have two scaler history files (for L and R arm),
//  try to concatenate them 'cat file1 file2 > file3' and analyze
//  the resulting concatenated file3.
// 
//  R. Michaels, updated Mar 2005
//--------------------------------------------------------

#include <iostream>
#include <string>
#include "THaScaler.h"

#ifndef __CINT__
#include "TROOT.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TNtuple.h"
#include "TRandom.h"
#endif

using namespace std;

int main(int argc, char* argv[]) {

   const Double_t calib_u3  = 4114;  // calibrations (an example)
   const Double_t calib_d10 = 12728;
   const Double_t off_u3    = 167.1;
   const Double_t off_d10   = 199.5;

// Initialize root and output
   TROOT scalana("scalroot","Hall A scaler analysis");
   TFile hfile("scalerhist.root","RECREATE","Scaler history in Hall A");

// Define the ntuple here
//                0   1    2    3   4   5   6   7   8   9  10    11
   char cdata[]="run:time:u3:d10:ct1:ct2:ct3:ct4:ct5:ct6:ct7:ctacc";
   int nlen = strlen(cdata);
   char *string_ntup = new char[nlen+1];
   strcpy(string_ntup,cdata);
   TNtuple *ntup = new TNtuple("ascal","Scaler History Data",string_ntup);

   Float_t farray_ntup[12];       // Note, dimension is same as size of string_ntup 

   cout << "Enter bank 'Right' or 'Left' (spectrometer) ->" << endl;
   string bank;  cin >> bank;
   cout << "enter [runlo, runhi] = interval of runs to summarize ->" << endl;
   int runlo; cout << "runlo: " << endl; cin >> runlo; 
   int runhi; cout << "runhi: " << endl; cin >> runhi;

   THaScaler scaler(bank.c_str());
   if (scaler.Init() == -1) {    // Init for default time ("now").
      cout << "Error initializing scaler "<<endl;  return 1;
   }

   int status;

   for (int run = runlo; run < runhi; run++) {

     status = scaler.LoadDataHistoryFile(run);  // load data from default history file
     if ( status != 0 ) continue;

     for (int i = 0; i < 12; i++) farray_ntup[i] = 0;

     cout << "\nScalers for run = "<<run<<endl<<flush;
     cout << "Time    ---  Beam Current (uA)  ----    |   --- Triggers ---  "<<endl;
     cout << "(min)         u3           d10";
     cout << "        T1(Right)      T3(Left)       T5(coinc)"<<endl;
     double time_sec = scaler.GetPulser("clock")/1024;
     double curr_u3  = (scaler.GetBcm("bcm_u3")/time_sec - off_u3)/calib_u3;
     double curr_d10 = (scaler.GetBcm("bcm_d10")/time_sec - off_d10)/calib_d10;
     printf("%7.2f     %7.2f     %7.2f       %d       %d         %d\n",
	    time_sec/60, curr_u3, curr_d10, scaler.GetTrig(1), scaler.GetTrig(3),
            scaler.GetTrig(5)); 
     cout << "            T6               T7       Tot accepted"<<endl;
     printf("             %d        %d      %d\n",
          scaler.GetTrig(6), scaler.GetTrig(7),scaler.GetNormData(0,12));

     farray_ntup[0] = run;
     farray_ntup[1] = time_sec;
     farray_ntup[2] = curr_u3;
     farray_ntup[3] = curr_d10;
     for (int itrig = 1; itrig <= 7; itrig++) farray_ntup[3+itrig]=scaler.GetTrig(itrig);
     farray_ntup[11] = scaler.GetNormData(0,12);

     ntup->Fill(farray_ntup);


   }

   hfile.Write();
   hfile.Close();

   return 0;
}











