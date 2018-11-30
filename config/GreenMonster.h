#define MAXCRATES 5

#include <iostream>
//CAP edit 11/02/2015

#include "TGClient.h"
#include "Rtypes.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TObjectTable.h"
#include "TGCanvas.h"
#include "TPad.h"
#include "TPaveText.h"
#include "TGTab.h"
#include "TGIcon.h"
#include <TExec.h>
#include "TGFrame.h"
#include "TGMenu.h"
#include "TSystem.h"
#include "TGLayout.h"
#include "TGLabel.h"
#include "TGButton.h"
#include "TGTextBuffer.h"
#include "TGTextEntry.h"
#include "TGToolTip.h"
#include "TFile.h"
#include "TTimer.h"
#include "TColor.h"
#include "TQObject.h"
#include "TRootHelpDialog.h"
#include "RQ_OBJECT.h"

#include "GreenTB.h"
#include "GreenADC.h" 
#include "GreenADC18.h"
#include "GreenVQWK.h" //Caryn add
#include "../cfSock/cfSock_types.h"
#include "../bmw/bmw_cf_commands.h"
#include "../scan/SCAN_cf_commands.h"
#include "../qweak/VQWK_cf_commands.h" //Caryn add
//#include "GMSock.h"

#define GM_LOCAL     201

#define GM_BMW_CHANGE 2001
#define GM_BMW_CHECK  2002
#define GM_BMW_TEST_ENABLE 2005
#define GM_BMW_TEST_SET_VALUE 2006
#define GM_BMW_SET_KILL 2007
// the relative value of these button id's labels each modulation object
#define BMW_OBJRADIO1 2101
#define BMW_OBJRADIO2 2102
#define BMW_OBJRADIO3 2103
#define BMW_OBJRADIO4 2104
#define BMW_OBJRADIO5 2105
#define BMW_OBJRADIO6 2106
#define BMW_OBJRADIO7 2107
#define BMW_OBJRADIO8 2108

#define KILL_SERVER_1 5001
#define KILL_SERVER_2 5002
#define KILL_SERVER_3 5003
#define KILL_SERVER_4 5004
#define KILL_SERVER_5 5005


#define GM_SCN_CHECK  6002
#define GM_SCN_SET    6003
#define SCN_RADIO_CLN 6101
#define SCN_RADIO_NOT 6102
#define SCN_RADIO_CLN_BT 0
#define SCN_RADIO_NOT_BT 1
#define SCN_INT_CLN 1
#define SCN_INT_NOT 0

class GreenMonster : public TGMainFrame {
  RQ_OBJECT()

  ClassDef(GreenMonster, 0) 

public:
  GreenMonster(const TGWindow *p, UInt_t w, UInt_t h);
  ~GreenMonster();

  Bool_t ProcessMessage( Long_t msg, Long_t parm1, Long_t parm2);
  void BMWDelayCheck();
  void BMWTestStep();

  void Init();

private:
  Int_t mainWidth;
  Int_t mainHeight;
  //  const static ULong_t grnback = 0x8fbc8f;  // darkseagreen
  ULong_t grnback;

  Bool_t fUseCrate[MAXCRATES];
  Int_t fUseTB[MAXCRATES];
  Int_t fUseADC[MAXCRATES];
  Int_t fUseADCX[MAXCRATES];
  Int_t fUsevqwk[MAXCRATES];
  Int_t fUseSCAN[MAXCRATES];
  TString* fCrateNames[MAXCRATES];
  Int_t fCrateNumbers[MAXCRATES];
  Int_t scanCrate;

  GreenTB* fTimeBoard[MAXCRATES];
  GreenVQWK* fVQWK[MAXCRATES]; //Caryn add
  GreenADC* fADC[MAXCRATES];
  GreenADC18* fADC18[MAXCRATES];

  // objects for bmw tabpage
  Int_t fBMW_TABID;
  TGTextButton *changeStatusBtBMW;
  TGTextButton *checkStatusBtBMW;
  TGTextButton *enableTestBtBMW;
  TGTextButton *setKillBtBMW;
  TGTextButton *setValueBtBMW;

  TGLabel *switchLabelBMW;
  TGLabel *activeLabelBMW;
  TGLabel *statusLabelBMW;
  TGRadioButton* fTestObjRBtBMW[8];
  TGTextEntry  *tentSetPntBMW;
  Int_t chosenObjBMW;
  Int_t testSetpointBMW;

  TTimer* _ctimer;
  void BMWDoRadio(Int_t);
  Bool_t BMWCheckStatus();
  void BMWChangeStatus();
  void BMWActiveProbe();
  void BMWCheckActiveFlag();
  
  void BMWSetKill();
  void BMWStartTest();

  // objects for scan tabpage
  Int_t fSCN_TABID;
  TGTextButton *setValueBtSCN;
  TGTextButton *checkStatusBtSCN;

  Int_t statusSCN;
  Int_t setpoint1SCN;
  Int_t setpoint2SCN;
  Int_t setpoint3SCN;
  Int_t setpoint4SCN;
  Int_t setpoint5SCN;
  TGRadioButton* fStateRBtSCN[2];
  TGTextEntry  *tentSetPnt1SCN;
  TGTextEntry  *tentSetPnt2SCN;
  TGTextEntry  *tentSetPnt3SCN;
  TGTextEntry  *tentSetPnt4SCN;
  TGTextEntry  *tentSetPnt5SCN;
  TGTextEntry  *tentSetPnt6SCN;
  TGTextEntry  *tentSetPnt7SCN;
  TGTextEntry  *tentSetPnt8SCN;

  void SCNDoRadio(Int_t);
  void SCNUpdateStatus(Int_t);
  Bool_t SCNCheckStatus();
  void SCNSetValues();
  void SCNSetValue(Int_t,Int_t);
  void SCNCheckValues();
  void SCNSetStatus(Int_t);
  

  //
  void InitGui();

};
