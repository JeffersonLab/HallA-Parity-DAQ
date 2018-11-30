
#include "GreenMonster.h"

ClassImp(GreenMonster)

GreenMonster::GreenMonster(const TGWindow *p, UInt_t w, UInt_t h) : 
TGMainFrame(p, w, h), mainWidth(w), mainHeight(h)
{

  gClient->GetColorByName("darkseagreen",grnback);
  ULong_t white;
  gClient->GetColorByName("white",white);
  if (grnback==white)   gClient->GetColorByName("green",grnback);

  this->SetBackgroundColor(grnback);
  fUseCrate[0]= kTRUE;
  fUseTB[0]=1;
  fUseADC[0]= 0;
  fUseADCX[0]= 1;
  fUsevqwk[0]= 1;
  fUseSCAN[0]=1;
  fCrateNames[0] = new TString("CH");
  fCrateNumbers[0] = Crate_CountingHouse;

  fUseCrate[1]= kFALSE;
  fUseTB[1]=0;
  fUseADC[1]= 0;
  fUseADCX[1]= 0;
  fUsevqwk[1]= 1;
  fUseSCAN[1]=1;
  fCrateNames[1] = new TString("Inj");
  fCrateNumbers[1] = Crate_Injector;

  fUseCrate[2]= kFALSE;
  fUseTB[2]=0;
  fUseADC[2]= 0;
  fUseADCX[2]= 0;
  fUsevqwk[2]= 1;
  fUseSCAN[2]=0;
  fCrateNames[2] = new TString("LftSpec");
  fCrateNumbers[2] = Crate_LeftSpect;

  fUseCrate[3]= kFALSE;
  fUseTB[3]=1;
  fUseADC[3]= 0;
  fUseADCX[3]= 1;
  fUsevqwk[3]= 1;
  fUseSCAN[3]=0;
  fCrateNames[3] = new TString("RtSpec");
  fCrateNumbers[3] = Crate_RightSpect;

  fUseCrate[4]= kFALSE;
  fUseTB[4]=0;
  fUseADC[4]= 0;
  fUseADCX[4]= 0;
  fUsevqwk[4]= 1;
  fUseSCAN[4]=0;
  fCrateNames[4] = new TString("Test");
  fCrateNumbers[4] = Crate_TestCrate;


  if(fUseCrate[0]&&fUseSCAN[0]){ 
    scanCrate=Crate_CountingHouse;  
  }else if(fUseCrate[1]&&fUseSCAN[1]){ 
    scanCrate=Crate_Injector;
  }else if(fUseCrate[2]&&fUseSCAN[2]){ 
    scanCrate=Crate_LeftSpect;
  }else if(fUseCrate[3]&&fUseSCAN[3]){ 
    scanCrate=Crate_RightSpect;
  }


}

GreenMonster::~GreenMonster() {
};

void GreenMonster::Init() {
  GreenMonster::InitGui();
}

void GreenMonster::InitGui() {

  Int_t killServerCommand[MAXCRATES] = 
    {KILL_SERVER_1, KILL_SERVER_2, 
     KILL_SERVER_3, KILL_SERVER_4, 
     KILL_SERVER_5}; 


  TGCompositeFrame *tf;
  TGTextButton *bt;
  TGLayoutHints   *fLayout;

  TGTab *fTab = new TGTab(this, 0, 0);
  fTab->SetBackgroundColor(grnback);

  TGLayoutHints *framout = new TGLayoutHints(kLHintsTop | kLHintsExpandX | 
					     kLHintsExpandY, 5, 5, 5, 5);


  tf  = new TGHorizontalFrame(this,mainWidth-10,mainHeight-10,kFixedWidth);
  tf->SetBackgroundColor(grnback);

  // place quit button on bottom of page
  TGLayoutHints *fLbottom = new TGLayoutHints(kLHintsBottom | kLHintsRight, 
					      2, 2, 4, 2);


  // create quit button
  TGCompositeFrame* sizebtn = new TGCompositeFrame(tf,150,100,kFixedWidth);
  sizebtn->SetBackgroundColor(grnback);
  tf->AddFrame(sizebtn,fLbottom);
  bt = new TGTextButton(sizebtn,"&QUIT",".qqqqqqqqq");
  bt->SetBackgroundColor(grnback);
  sizebtn->AddFrame(bt,new TGLayoutHints(kLHintsBottom|kLHintsExpandX,5,10,4,2));

  TGPictureButton* mike = 
    new TGPictureButton(tf,gClient->GetPicture("/adaqfs/home/apar/devices/config/gm.xpm"));
  mike->SetBackgroundColor(grnback);
  mike->SetToolTipText("grrr...");
  tf->AddFrame(mike,new TGLayoutHints(kLHintsBottom|kLHintsLeft,5,10,4,2));

  bt->Resize(100,bt->GetDefaultHeight());

  TGLayoutHints *fL_Bot_Right = new TGLayoutHints(kLHintsBottom | 
						  kLHintsRight, 2, 2, 5, 1);
  this->AddFrame(tf,fL_Bot_Right);

  //
  // create first page, Timeboard
  //


  tf = fTab->AddTab("TimeBoard");
  TGTabElement *tabel = fTab->GetTabTab(0);
  tabel->ChangeBackground(grnback);
  tf->SetLayoutManager(new TGMatrixLayout(tf, 2, 2, 10));
  tf->SetBackgroundColor(grnback);


  TGLayoutHints *addTBLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX | 
						 kLHintsExpandY, 5, 5, 5, 5);

  for (Int_t i=0; i<MAXCRATES; i++) {
    if (fUseCrate[i]&&fUseTB[i]) {
      fTimeBoard[i] = new GreenTB(fCrateNumbers[i],fCrateNames[i]->Data(),
				  tf, 350, 200);
      fTimeBoard[i]->Init(grnback);
      tf->AddFrame(fTimeBoard[i],addTBLayout);
    }
  }  


  //
  // create next page, vqwk adcs
  //
  Int_t vqwkTAB=0;
  for(Int_t i=0; i<MAXCRATES;i++){
    vqwkTAB=vqwkTAB+fUsevqwk[i];
  }
  if(vqwkTAB>0){
    tf = fTab->AddTab("VQWK ADCs");
    tabel = fTab->GetTabTab(1);
    tabel->ChangeBackground(grnback);
    tf->SetLayoutManager(new TGMatrixLayout(tf, 2, 2, 10));
    tf->SetBackgroundColor(grnback);


    TGLayoutHints *addVQWKLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX | 
						     kLHintsExpandY, 5, 5, 5, 5);

    for (Int_t i=0; i<MAXCRATES; i++) {
      if (fUseCrate[i]&&fUsevqwk[i]) {
	fVQWK[i] = new GreenVQWK(fCrateNumbers[i],fCrateNames[i]->Data(),
				 tf, 350, 200);

	fVQWK[i]->Init(grnback);
	tf->AddFrame(fVQWK[i],addVQWKLayout);
      }
    }  
  }


  //
  // create next page, ADCs
  //
  char buff[15];
  for (Int_t i=0; i<MAXCRATES; i++) {
    if (fUseCrate[i]&&fUseADC[i]) {
      sprintf(buff,"ADC16s, %s",fCrateNames[i]->Data());
      tf = fTab->AddTab(buff);
      tabel = fTab->GetTabTab(fTab->GetNumberOfTabs()-1);  // get tab index
      tabel->ChangeBackground(grnback);
      tf->SetBackgroundColor(grnback);

      fADC[i] = new GreenADC(fCrateNumbers[i],fCrateNames[i]->Data(),
			     tf, 350, 200);
      fADC[i]->Init(grnback);
      tf->AddFrame(fADC[i],framout);
    } 
  }  

  //
  // create next page, ADCs
  //
  for (Int_t i=0; i<MAXCRATES; i++) {
    if (fUseCrate[i]&&fUseADCX[i]) {
      sprintf(buff,"ADC18s, %s",fCrateNames[i]->Data());
      tf = fTab->AddTab(buff);
      tabel = fTab->GetTabTab(fTab->GetNumberOfTabs()-1);  // get tab index
      tabel->ChangeBackground(grnback);
      tf->SetBackgroundColor(grnback);

      fADC18[i] = new GreenADC18(fCrateNumbers[i],fCrateNames[i]->Data(),
				 tf, 350, 200);
      fADC18[i]->Init(grnback);
      tf->AddFrame(fADC18[i],framout);
    } 
  }  



  //
  // create BMW page
  //
  tf = fTab->AddTab("BMW");
  fBMW_TABID = fTab->GetNumberOfTabs()-1;  // set tab index for later use
  tf->SetBackgroundColor(grnback);
  tf->SetLayoutManager(new TGHorizontalLayout(tf));
  TGCompositeFrame *tcfbmw = new TGGroupFrame(tf, "Beam Modulation", 
					      kHorizontalFrame);
  tcfbmw->SetBackgroundColor(grnback);
  tf->AddFrame(tcfbmw,framout);
  tabel = fTab->GetTabTab(fBMW_TABID);
  tabel->ChangeBackground(grnback);



  //
  // create server page
  //
 
  tf = fTab->AddTab("VXWorks Server");
  tabel = fTab->GetTabTab(fTab->GetNumberOfTabs()-1);  // get tab index
  tabel->ChangeBackground(grnback);
  tf->SetBackgroundColor(grnback);
  tf->SetLayoutManager(new TGHorizontalLayout(tf));
  TGLayoutHints *noex = new TGLayoutHints(kLHintsCenterX | 
					  kLHintsCenterY, 5, 5, 5, 5);
  TGCompositeFrame *tcf4a = new TGCompositeFrame(tf, 300, 400, 
						 kVerticalFrame|kFixedWidth);
  tcf4a->SetBackgroundColor(grnback);
  tf->AddFrame(tcf4a,noex);

  // create SCAN page


  tf = fTab->AddTab("ScanUtil");
  fSCN_TABID = fTab->GetNumberOfTabs()-1;  // set tab index for later use
  tabel = fTab->GetTabTab(fSCN_TABID);  // get tab index
  tabel->ChangeBackground(grnback);
  tf->SetBackgroundColor(grnback);
  tf->SetLayoutManager(new TGHorizontalLayout(tf)); 
  TGCompositeFrame *tcfSCN = new TGGroupFrame(tf, "SCAN UTILITY", 
					      kHorizontalFrame);
  tcfSCN->SetBackgroundColor(grnback);
  tf->AddFrame(tcfSCN,framout);
  tcfSCN->SetLayoutManager(new TGVerticalLayout(tcfSCN));

  TGCompositeFrame *sfrm0 = new TGHorizontalFrame(tcfSCN,300,200);
  sfrm0->SetBackgroundColor(grnback);
  TGLayoutHints *sL_0 = new TGLayoutHints(kLHintsTop, 2, 2, 5, 1);
  tcfSCN->AddFrame(sfrm0,sL_0);
  sfrm0->SetLayoutManager(new TGMatrixLayout(sfrm0, 0, 2, 10));

  fStateRBtSCN[0] = new TGRadioButton(sfrm0,"  CLEAN ",SCN_RADIO_CLN);
  fStateRBtSCN[1] = new TGRadioButton(sfrm0,"NOT CLEAN  ",SCN_RADIO_NOT);
  for (Int_t ib = 0; ib<2; ib++) {
    fStateRBtSCN[ib]->SetBackgroundColor(grnback);
    fStateRBtSCN[ib]->Associate(this);
    sfrm0->AddFrame(fStateRBtSCN[ib]);
  }

  TGLayoutHints *addSCANLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX | 
						   kLHintsExpandY, 5, 5, 5, 5);

  //----------------------------------------------------------------------
  // add buttons to SCN page
  TGCompositeFrame *tcfSCN1;
  if(scanCrate==fCrateNumbers[0]){
    tcfSCN1 = new TGGroupFrame(tcfSCN, "CH",kVerticalFrame);
  }else if(scanCrate==fCrateNumbers[1]){
    tcfSCN1 = new TGGroupFrame(tcfSCN, "Inj",kVerticalFrame);
  }
  if(tcfSCN == 0) {
    cout << "ERROR: tcfSCN1 pointer zero.  Cannot continue; would segfault "<<endl;
    exit(0);
  }

  
  tcfSCN1->SetBackgroundColor(grnback);
  tcfSCN->AddFrame(tcfSCN1,addSCANLayout);

  TGCompositeFrame *sfrm1 = new TGHorizontalFrame(tcfSCN1,300,200);
  sfrm1->SetBackgroundColor(grnback);
  TGLayoutHints *sL_1 = new TGLayoutHints(kLHintsTop, 2, 2, 5, 1);
  tcfSCN1->AddFrame(sfrm1,sL_1);
  sfrm1->SetLayoutManager(new TGMatrixLayout(sfrm1, 0, 2, 10));
  sprintf(buff, "0");
  TGTextBuffer *stbuf = new TGTextBuffer(6);
  stbuf->AddText(0,buff);
  TGLabel * stlab= new TGLabel(sfrm1,new TGHotString("Set Point 1"));
  sfrm1->AddFrame(stlab);
  stlab->SetBackgroundColor(grnback);
  tentSetPnt1SCN = new TGTextEntry(sfrm1, stbuf, 221);
  tentSetPnt1SCN->Resize(60, tentSetPnt1SCN->GetDefaultHeight());
  tentSetPnt1SCN->SetFont("-adobe-courier-bold-r-*-*-14-*-*-*-*-*-iso8859-1");
  sfrm1->AddFrame(tentSetPnt1SCN);
  sprintf(buff, "0");
  TGTextBuffer *stbuf2 = new TGTextBuffer(6);
  stbuf2->AddText(0,buff);
  TGLabel * stlab2= new TGLabel(sfrm1,new TGHotString("Set Point 2"));
  sfrm1->AddFrame(stlab2);
  stlab2->SetBackgroundColor(grnback);
  tentSetPnt2SCN = new TGTextEntry(sfrm1, stbuf2, 221);
  tentSetPnt2SCN->Resize(60, tentSetPnt2SCN->GetDefaultHeight());
  tentSetPnt2SCN->SetFont("-adobe-courier-bold-r-*-*-14-*-*-*-*-*-iso8859-1");
  sfrm1->AddFrame(tentSetPnt2SCN);

  sprintf(buff, "0");
  TGTextBuffer *stbuf3 = new TGTextBuffer(6);
  stbuf3->AddText(0,buff);
  TGLabel * stlab3= new TGLabel(sfrm1,new TGHotString("Set Point 3"));
  sfrm1->AddFrame(stlab3);
  stlab3->SetBackgroundColor(grnback);
  tentSetPnt3SCN = new TGTextEntry(sfrm1, stbuf3, 221);
  tentSetPnt3SCN->Resize(60, tentSetPnt3SCN->GetDefaultHeight());
  tentSetPnt3SCN->SetFont("-adobe-courier-bold-r-*-*-14-*-*-*-*-*-iso8859-1");
  sfrm1->AddFrame(tentSetPnt3SCN);

  sprintf(buff, "0");
  TGTextBuffer *stbuf4 = new TGTextBuffer(6);
  stbuf4->AddText(0,buff);
  TGLabel * stlab4= new TGLabel(sfrm1,new TGHotString("Set Point 4"));
  sfrm1->AddFrame(stlab4);
  stlab4->SetBackgroundColor(grnback);
  tentSetPnt4SCN = new TGTextEntry(sfrm1, stbuf4, 221);
  tentSetPnt4SCN->Resize(60, tentSetPnt4SCN->GetDefaultHeight());
  tentSetPnt4SCN->SetFont("-adobe-courier-bold-r-*-*-14-*-*-*-*-*-iso8859-1");
  sfrm1->AddFrame(tentSetPnt4SCN);

  tcfSCN1->Resize(tcfSCN1->GetDefaultSize());
  sfrm1->Resize(sfrm1->GetDefaultSize());

  TGCompositeFrame *sfrm2 = new TGHorizontalFrame(tcfSCN,300,200);
  sfrm2->SetBackgroundColor(grnback);
  TGLayoutHints *sL_2 = new TGLayoutHints(kLHintsTop, 2, 2, 5, 1);
  tcfSCN->AddFrame(sfrm2,sL_2);
  sfrm2->SetLayoutManager(new TGMatrixLayout(sfrm2, 1, 0, 10));

  sfrm2->AddFrame(checkStatusBtSCN = 
		  new TGTextButton(sfrm2,"Check Status",GM_SCN_CHECK));
  checkStatusBtSCN->Associate(this);
  checkStatusBtSCN->SetBackgroundColor(grnback);

  sfrm2->AddFrame(setValueBtSCN = 
		  new TGTextButton(sfrm2,"Set Values",GM_SCN_SET));
  setValueBtSCN->Associate(this);
  setValueBtSCN->SetBackgroundColor(grnback);

  sfrm2->Resize(sfrm2->GetDefaultSize());
 
  tcfSCN->Resize(tcfSCN->GetDefaultSize());
 
  SCNCheckStatus(); // this call is actually an important initialization!
  SCNCheckValues(); // this call is actually an important initialization!



  //----------------------------------------------------------------------
  // add buttons to BMW page
  // tcfbmw has a matrix layout with 2 columns: put label and buttons
  // side by side...

  tcfbmw->SetLayoutManager(new TGVerticalLayout(tcfbmw));

  TGCompositeFrame *tcfbmw1 = 
    new TGGroupFrame(tcfbmw, "Beam Modulation Script",kHorizontalFrame);
  tcfbmw1->SetBackgroundColor(grnback);
  tcfbmw->AddFrame(tcfbmw1,framout);

  TGCompositeFrame *tcfbmw_t = 
    new TGGroupFrame(tcfbmw, "Beam Modulation- TEST",kVerticalFrame);
  tcfbmw_t->SetBackgroundColor(grnback);
  tcfbmw->AddFrame(tcfbmw_t,framout);


  TGMatrixLayout *fMatLay = new TGMatrixLayout(tcfbmw1, 2, 3, 10, 10);
  tcfbmw1->SetLayoutManager(fMatLay);

  switchLabelBMW = new TGLabel(tcfbmw1,"Kill switch is OFF");
  switchLabelBMW->SetBackgroundColor(grnback);
  tcfbmw1->AddFrame(switchLabelBMW);
  tcfbmw1->AddFrame(checkStatusBtBMW = new TGTextButton(tcfbmw1,"Check Status",
							GM_BMW_CHECK));
  checkStatusBtBMW->Associate(this);
  checkStatusBtBMW->SetBackgroundColor(grnback);

  // tcfbmw1->AddFrame(statusLabelBMW = 
  // 		    new TGLabel(tcfbmw1,"Beam Modulation is OFF"));
  tcfbmw1->AddFrame(statusLabelBMW = 
 		    new TGLabel(tcfbmw1,"*************"));
  statusLabelBMW->SetBackgroundColor(grnback);
  changeStatusBtBMW = new TGTextButton(tcfbmw1,"Start Beam Modulation",
  				       GM_BMW_CHANGE);
  changeStatusBtBMW->Associate(this);
  changeStatusBtBMW->SetBackgroundColor(grnback);
  tcfbmw1->AddFrame(changeStatusBtBMW);

  //  activeLabelBMW = new TGLabel(tcfbmw1,"Beam Modulation script is INACTIVE");
  activeLabelBMW = new TGLabel(tcfbmw1,"Beam Modulation script is ***");
  activeLabelBMW->SetBackgroundColor(grnback);
  tcfbmw1->AddFrame(activeLabelBMW);

  TGCompositeFrame *tmpfrmB = new TGHorizontalFrame(tcfbmw_t,500,100);
  tmpfrmB->SetBackgroundColor(grnback);
  tcfbmw_t->AddFrame(tmpfrmB);
  tmpfrmB->SetLayoutManager(new TGMatrixLayout(tmpfrmB, 1, 0, 10));

  tmpfrmB->AddFrame(enableTestBtBMW = 
		    new TGTextButton(tmpfrmB,"Enable BMW Test",GM_BMW_TEST_ENABLE));
  enableTestBtBMW->Associate(this);
  enableTestBtBMW->SetBackgroundColor(grnback);
  tmpfrmB->AddFrame(setKillBtBMW = 
		    new TGTextButton(tmpfrmB,"Toggle Kill Switch",GM_BMW_SET_KILL));
  setKillBtBMW->Associate(this);
  setKillBtBMW->SetBackgroundColor(grnback);

  //    tmpfrmB->AddFrame(setValueBtBMW = 
  //  	       new TGTextButton(tmpfrmB,"Apply Value",GM_BMW_TEST_SET_VALUE));
  //    setValueBtBMW->Associate(this);
  //    setValueBtBMW->SetBackgroundColor(grnback);

  TGCompositeFrame *tmpfrm = new TGHorizontalFrame(tcfbmw_t,500,100);
  tmpfrm->SetLayoutManager(new TGMatrixLayout(tmpfrm, 0, 4));
  tmpfrm->SetBackgroundColor(grnback);
  tcfbmw_t->AddFrame(tmpfrm);
  fTestObjRBtBMW[0] = new TGRadioButton(tmpfrm,"MHF1C01H ",BMW_OBJRADIO1);
  fTestObjRBtBMW[1] = new TGRadioButton(tmpfrm,"MHF1C02H ",BMW_OBJRADIO2);
  fTestObjRBtBMW[2] = new TGRadioButton(tmpfrm,"MHF1C03V ",BMW_OBJRADIO3);
  fTestObjRBtBMW[3] = new TGRadioButton(tmpfrm,"MHF1C08H ",BMW_OBJRADIO4);
  fTestObjRBtBMW[4] = new TGRadioButton(tmpfrm,"MHF1C08V ",BMW_OBJRADIO5);
  fTestObjRBtBMW[5] = new TGRadioButton(tmpfrm,"MHF1C10H ",BMW_OBJRADIO6);
  fTestObjRBtBMW[6] = new TGRadioButton(tmpfrm,"MHF1C10V ",BMW_OBJRADIO7);
  fTestObjRBtBMW[7] = new TGRadioButton(tmpfrm,"SL Zone 20",BMW_OBJRADIO8);
  for (Int_t ib = 0; ib<8; ib++) {
    fTestObjRBtBMW[ib]->SetBackgroundColor(grnback);
    fTestObjRBtBMW[ib]->Associate(this);
    tmpfrm->AddFrame(fTestObjRBtBMW[ib]);
  }
  chosenObjBMW = 0;
  fTestObjRBtBMW[0]->SetState(kButtonDown);

  TGCompositeFrame *tmpfrmv = new TGHorizontalFrame(tcfbmw_t,500,100);
  tmpfrmv->SetBackgroundColor(grnback);
  tcfbmw_t->AddFrame(tmpfrmv);

  //  tmpfrmv->SetLayoutManager(new TGMatrixLayout(tmpfrm, 0, 2));
  sprintf(buff, "0");
  TGTextBuffer *tbuf = new TGTextBuffer(6);
  tbuf->AddText(0,buff);
  tentSetPntBMW = new TGTextEntry(tmpfrmv, tbuf, 221);
  tentSetPntBMW->Resize(60, tentSetPntBMW->GetDefaultHeight());
  tentSetPntBMW->SetFont("-adobe-courier-bold-r-*-*-14-*-*-*-*-*-iso8859-1");
  tmpfrmv->AddFrame(tentSetPntBMW);
  TGLabel * lab= new TGLabel(tmpfrmv,new TGHotString("Set Point (mA or keV)"));
  tmpfrmv->AddFrame(lab);
  lab->SetBackgroundColor(grnback);

  BMWCheckStatus();
  BMWActiveProbe();

  //----------------------------------------------------------------------
  // add buttons to Control page
  fLayout = new TGLayoutHints(kLHintsCenterX | kLHintsCenterY | 
			      kLHintsExpandX,2,2,5,5);
  for (Int_t i = 0; i<MAXCRATES; i++) {
    if (fUseCrate[i]) {
      TString tmpstr = "Kill VXWorks Server, ";
      tmpstr += fCrateNames[i]->Data();
      tcf4a->AddFrame(bt = new TGTextButton(tcf4a,tmpstr.Data(),
					    killServerCommand[i]),
		      fLayout);
      bt->SetBackgroundColor(grnback);
      bt->Associate(this);
      bt->SetToolTipText("Kill the Socket Server running on this crate");
    }
  } 

  //  //----------------------------------------------------------------------
  //  // Add top composite frame to the main frame (this)
  AddFrame(fTab,new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,0,0,0,0));
  MapSubwindows();
  Layout();
  SetWindowName("Configuration Utility");
  SetIconName("GM");
  MapWindow();
}


Bool_t GreenMonster::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
  int junk;
  // Process events generated by the buttons in the frame
  switch (GET_MSG(msg)) {
  case kC_COMMAND:
    switch (GET_SUBMSG(msg)) {
    case kCM_BUTTON:
      switch (parm1) 
	{
	case 1:
	  {
	    int command = 10,par_1 =1,par_2 =2, par_3 =3, command_type = 10;
	    char *msgReq =(char *)"One";
	    char *reply =(char *)"Y";

	    //junk = cfSockCommand(command,par_1,par_2,reply,msgReq);
	    struct greenRequest gRequest;
	    strcpy(gRequest.message,msgReq);
	    gRequest.reply = reply;
	    gRequest.command_type = command_type;
	    gRequest.command = command;
	    gRequest.par1 = par_1;
	    gRequest.par2 = par_2;
	    gRequest.par3 = par_3;
	    junk = GreenSockCommand(Crate_CountingHouse,&gRequest);

	    printf ("cfSockCommand returned :  %d \n",junk);
	    command_type = gRequest.command_type;
	    command = gRequest.command;
	    par_1 = gRequest.par1;
	    par_2 = gRequest.par2;
	    par_3 = gRequest.par3;
	    if(*reply=='Y' || *reply=='y') {
	      printf ("GM: MESSAGE FROM SERVER:\n%s\n", msgReq);
	      printf("Server reply command: %d \n",command);
	      printf("Server reply param_1: %d \n",par_1);
	      printf("Server reply param_2: %d \n",par_2);
	      printf("Server reply param_3: %d \n",par_3);
	    }

	  }
	  break;
	case 2:
	  {
	    TExec *ex = new TExec("ex",".q");
	    ex->Exec();
	  }
	  break;
	case KILL_SERVER_1:
	case KILL_SERVER_2:
	case KILL_SERVER_3:
	case KILL_SERVER_4:
	case KILL_SERVER_5:
	  {
	    int command = 10,par_1 =1,par_2 =2, par_3 =3, command_type = 10;
	    char *msgReq =(char *)"Q";
	    char *reply =(char *)"N";
	    junk = cfSockCommand(fCrateNumbers[parm1-KILL_SERVER_1],
				 command_type,command,par_1,par_2,par_3,
				 reply,msgReq);
	    //	    printf ("cfSockCommand returned :  %d \n",junk);
	    msgReq = (char *)"Q";
	    reply = (char *)"N";
	    junk = cfSockCommand(fCrateNumbers[parm1-KILL_SERVER_1],
				 command_type,command,par_1,par_2,par_3,
				 reply,msgReq);
	    break;
	  }
	case GM_BMW_CHANGE:
	  {
	    printf("changing status of BMW \n");
	    BMWChangeStatus();
	    break;
	  }
	case GM_BMW_CHECK:	
	  {
	    printf("checking status of BMW \n");
	    BMWCheckStatus();
	    BMWActiveProbe();
	    break;
	  }
	case GM_BMW_SET_KILL:
	  {
	    BMWSetKill();
	    break;
	  }
	case GM_BMW_TEST_ENABLE:
	  {
	    BMWStartTest();
	    break;
	  }
	case GM_SCN_CHECK:	
	  {
	    //	      printf("checking status of SCN \n");
	    SCNCheckStatus();
	    SCNCheckValues();
	    break;
	  }
	case GM_SCN_SET:	
	  {
	    //	      printf("setting SCN values\n");
	    SCNSetValues();
	    SCNCheckStatus();
	    break;
	  }
	default:
	  printf("text button id %ld pressed\n", parm1);
	  printf("text button context %ld \n", parm2);
	  break;
	}
      break;
    case kCM_RADIOBUTTON: {
      switch (parm1) 
	{
	case BMW_OBJRADIO1:
	case BMW_OBJRADIO2:
	case BMW_OBJRADIO3:
	case BMW_OBJRADIO4:
	case BMW_OBJRADIO5:
	case BMW_OBJRADIO6:
	case BMW_OBJRADIO7:
	case BMW_OBJRADIO8:
	  {
	    //	      cout << "radio button pushed " << parm1 << endl;
	    BMWDoRadio(parm1);
	    break;
	  }
	case SCN_RADIO_CLN:
	case SCN_RADIO_NOT:
	  {
	    // cout << "SCN radio button pushed " << parm1 << endl;
	    SCNDoRadio(parm1);
	    break;
	  }
	default:
	  {
	    cout << "unknown radio button pushed " << parm1 << endl;
	  }
	}
    }
    case kCM_TAB: {
      if (parm1==fBMW_TABID) {
	// check status of BMW every time the tab is chosen
	//	  printf("clicked bmw tab \n");
	BMWCheckStatus();
	BMWActiveProbe();	  
      }	  
      break;
    }
    default:
      break;
    }
  default:
    break;
  }
  return kTRUE;
}

void GreenMonster::BMWDoRadio(Int_t id) {    
  fTestObjRBtBMW[chosenObjBMW]->SetState(kButtonUp);
  chosenObjBMW = id - BMW_OBJRADIO1;
  fTestObjRBtBMW[chosenObjBMW]->SetState(kButtonDown);
  //  cout << "New BMW test object chosen : " << chosenObjBMW << endl;
  return;
}

void GreenMonster::BMWStartTest() {    
  struct greenRequest gRequest;
  int command, command_type, par1, par2, par3;
  char *msgReq = (char *)"BMW Start Test";
  char *reply = (char *)"Y";

  //
  command_type = COMMAND_BMW;    gRequest.command_type = command_type;
  command = BMW_TEST_START;      gRequest.command = command;
  par1 = 0;                      gRequest.par1 = par1;
  par2 = 0;                      gRequest.par2 = par2;
  par3 = 0;                      gRequest.par3 = par3;
  strcpy(gRequest.message,msgReq);   gRequest.reply = reply;
  if (GreenSockCommand(Crate_CountingHouse,&gRequest) == SOCK_OK) {
    command = gRequest.command;
  } else {
    printf("BMWStartTest::ERROR accessing socket!\n"); 
    return;
  }

  // do first test step
  BMWTestStep();

}

void GreenMonster::BMWTestStep() {    
  int value;
  Bool_t kill_switch;

  struct greenRequest gRequest;
  int command, command_type, par1, par2, par3;
  char *msgReq = (char *)"BW Test Set Data";
  char *reply = (char *)"Y";

  //
  // get value from tent
  //
  value = atoi(tentSetPntBMW->GetText());
  cout << " writing new set point " << value << " to " << chosenObjBMW << endl;
  //
  // send set message for obj, value
  //
  command_type = COMMAND_BMW;    gRequest.command_type = command_type;
  command = BMW_TEST_SET_DATA;   gRequest.command = command;
  par1 = chosenObjBMW;           gRequest.par1 = par1;
  par2 = value;                  gRequest.par2 = par2;
  par3 = 0;                      gRequest.par3 = par3;
  strcpy(gRequest.message,msgReq);   gRequest.reply = reply;

  if (GreenSockCommand(Crate_CountingHouse,&gRequest) == SOCK_OK) {
    command = gRequest.command;
    par1 = gRequest.par1;
    par2 = gRequest.par2;
    // par1 is kill switch!
    if (par1 != 0) {
      kill_switch = kTRUE;
    } else {
      kill_switch = kFALSE;
    }
  } else {
    printf("ERROR accessing socket!\n"); 
    return;
  }
  
  //    //
  //    // check for kill switch,
  //    //
  //    command_type = COMMAND_BMW;    gRequest.command_type = command_type;
  //    command = BMW_GET_STATUS;      gRequest.command = command;
  //    par1 = 0;                      gRequest.par1 = par1;
  //    par2 = 0;                      gRequest.par2 = par2;
  //    strcpy(gRequest.message,msgReq);   gRequest.reply = reply;
  //    if (GreenSockCommand(Crate_CountingHouse,&gRequest) == SOCK_OK) {
  //      command = gRequest.command;
  //      par1 = gRequest.par1;
  //      par2 = gRequest.par2;
  //      if (par2 != 0) {
  //        kill_switch = kTRUE;
  //      } else { 
  //        kill_switch = kFALSE;
  //      }
  //    } else {
  //      printf("ERROR accessing socket!\n"); 
  //      return;
  //    }

  //
  //if none, set timer to self
  //
  if (!kill_switch) {    
    TTimer* ctimer = new TTimer(4000,kTRUE);
    TQObject::Connect(ctimer, "Timeout()", "GreenMonster", this, 
		      "BMWTestStep()");
    ctimer->Start(4000, kTRUE);
  }

}

void GreenMonster::BMWChangeStatus() {
  struct greenRequest gRequest;
  int command, par1, par2, par3, command_type;
  char *msgReq = (char *)"BMW Status Change";
  char *reply = (char *)"Y";
  // get bmw status
  Bool_t bmw_running = BMWCheckStatus();

  command_type = COMMAND_BMW;   gRequest.command_type = command_type;

  printf("BMWChangeStatus():: bmw_running = %d\n", bmw_running);

  if (bmw_running) {
    // kill bmw
    command = BMW_KILL;          gRequest.command = command;
  } else {
    // start bmw
    command = BMW_START;          gRequest.command = command;
  }
  printf("BMWChangeStatus():: Setting command to %d\n", command);
  printf("changing status of bmw client\n");
  par1 = 0;                     gRequest.par1 = par1;
  par2 = 0;                     gRequest.par2 = par2;
  par3 = 0;                     gRequest.par3 = par3;

  strcpy(gRequest.message,msgReq);   gRequest.reply = reply;
  if (GreenSockCommand(Crate_CountingHouse,&gRequest) == SOCK_OK) {
    printf("bmw status change call is complete\n");
  } else {
    printf("ERROR accessing socket!\n");
    return;
  }
  // check bmw status
  BMWCheckStatus();
  printf("BMWChangeStatus():: BMW_START = %d\n", BMW_START);
  printf("BMWChangeStatus():: command = %d\n", command);
  printf("BMWChangeStatus():: Exiting...\n");
}


void GreenMonster::BMWSetKill() {
  struct greenRequest gRequest;
  int command, par1, par2, par3, command_type;
  char *msgReq = (char *)"BMW status check";
  char *reply = (char *)"Y";
  Bool_t bmw_running,kill_switch;

  // get bmw status
  //  printf("BMW Status =");
  command_type = COMMAND_BMW;    gRequest.command_type = command_type;
  command = BMW_GET_STATUS;      gRequest.command = command;
  par1 = 0;                      gRequest.par1 = par1;
  par2 = 0;                      gRequest.par2 = par2;
  par3 = 0;                      gRequest.par3 = par3;
  strcpy(gRequest.message,msgReq);   gRequest.reply = reply;
  if (GreenSockCommand(Crate_CountingHouse,&gRequest) == SOCK_OK) {
    command = gRequest.command;
    par1 = gRequest.par1;
    par2 = gRequest.par2;
    if (par1 != 0) {
      bmw_running = kTRUE;
    } else { 
      bmw_running = kFALSE;
    }
    if (par2 != 0) {
      kill_switch = kTRUE;
    } else { 
      kill_switch = kFALSE;
    }
  } else {
    printf("ERROR accessing socket!\n"); 
    return;
  }

  if (!kill_switch) {
    command_type = COMMAND_BMW;   gRequest.command_type = command_type;
    char *msgReq = (char *)"BMW set kill";
    command = BMW_KILL;          gRequest.command = command;
    printf("setting kill switch on bmw\n");
    par1 = 0;                     gRequest.par1 = par1;
    par2 = 0;                     gRequest.par2 = par2;
    par3 = 0;                      gRequest.par3 = par3;
    
    strcpy(gRequest.message,msgReq);   gRequest.reply = reply;
    if (GreenSockCommand(Crate_CountingHouse,&gRequest) == SOCK_OK) {
      printf("bmw kill switch call is complete\n");
    } else {
      printf("ERROR accessing socket!\n");
      return;
    }
  } else {
    command_type = COMMAND_BMW;   gRequest.command_type = command_type;
    char *msgReq = (char *)"BMW lift kill";
    command = BMW_UNKILL;          gRequest.command = command;
    printf("lifting kill switch on bmw\n");
    par1 = 0;                     gRequest.par1 = par1;
    par2 = 0;                     gRequest.par2 = par2;
    par3 = 0;                      gRequest.par3 = par3;
    
    strcpy(gRequest.message,msgReq);   gRequest.reply = reply;
    if (GreenSockCommand(Crate_CountingHouse,&gRequest) == SOCK_OK) {
      printf("bmw un-kill switch call is complete\n");
    } else {
      printf("ERROR accessing socket!\n");
      return;
    }
  }

  BMWCheckStatus();  // check bmw status
}

Bool_t GreenMonster::BMWCheckStatus() {
  // get bmw status
  struct greenRequest gRequest;
  int command, par1, par2, par3, command_type;
  char *msgReq = (char *)"BMW status check";
  char *reply = (char *)"Y";
  Bool_t bmw_running,kill_switch;
  
  //  printf("BMWCheckStatus():: \n");
  command_type = COMMAND_BMW;    gRequest.command_type = command_type;
  command = BMW_GET_STATUS;      gRequest.command = command;
  par1 = 0;                      gRequest.par1 = par1;
  par2 = 0;                      gRequest.par2 = par2;
  par3 = 0;                      gRequest.par3 = par3;
  strcpy(gRequest.message,msgReq);   gRequest.reply = reply;
  if (GreenSockCommand(Crate_CountingHouse,&gRequest) == SOCK_OK) {
    command = gRequest.command;
    par1 = gRequest.par1;
    par2 = gRequest.par2;

    printf("BMWCheckStatus()::\n  command = %d, \t par1 = %d, \t par2 = %d\n", command, par1, par2);

    if (par1 != 0) {
      bmw_running = kTRUE;
      printf("BMWCheckStatus():: bmw_running = %d\n", bmw_running);
    } else { 
      bmw_running = kFALSE;
      printf("BMWCheckStatus():: bmw_running = %d\n", bmw_running);
    }
    if (par2 != 0) {
      kill_switch = kTRUE;
    } else { 
      kill_switch = kFALSE;
    }
  } else {
    printf("ERROR accessing socket!\n");
    return kFALSE;
  }

  //  printf("% d  %d\n",par1,par2);

  if (kill_switch) {
    switchLabelBMW->SetText("Kill Switch is ON");
  } else {
    switchLabelBMW->SetText("Kill Switch is OFF");
  }
  if (bmw_running) {
    statusLabelBMW->SetText("Beam Modulation is ON");
    changeStatusBtBMW->SetText("Set Kill Switch");
    return kTRUE;
  } else {
    //    statusLabelBMW->SetText("Beam Modulation is OFF");
    statusLabelBMW->SetText("******************");
    changeStatusBtBMW->SetText("Start Beam Modulation");
    return kFALSE;
  }
  MapSubwindows();
}

void GreenMonster::BMWDelayCheck() {
  BMWCheckActiveFlag();
}

void GreenMonster::BMWActiveProbe() {
  // get bmw status
  BMWCheckActiveFlag();
  // recheck after some delay
  //    in order to use timer, or any other QT relying tool, 
  //    one must generate a dictionary using rootcint... 
  TTimer* ctimer = new TTimer(4000,kTRUE);
  TQObject::Connect(ctimer, "Timeout()", "GreenMonster", this, 
		    "BMWDelayCheck()");
  ctimer->Start(1000, kTRUE);
}

void GreenMonster::BMWCheckActiveFlag() {
  // get bmw status
  struct greenRequest gRequest;
  int command, par1, par2, par3, command_type;
  char *msgReq = (char *)"BMW status check";
  char *reply = (char *)"Y";
  Bool_t active=kFALSE;
  
  //  printf("BMWCheckActiveFlag has been called\n");
  command_type = COMMAND_BMW;    gRequest.command_type = command_type;
  command = BMW_CHECK_ALIVE;      gRequest.command = command;
  par1 = 0;                      gRequest.par1 = par1;
  par2 = 0;                      gRequest.par2 = par2;
  par3 = 0;                      gRequest.par3 = par3;
  strcpy(gRequest.message,msgReq);   gRequest.reply = reply;
  if (GreenSockCommand(Crate_CountingHouse,&gRequest) == SOCK_OK) {
    par2 = gRequest.par2;
    if (par2 != 0) {
      active = kTRUE;
    } else { 
      active = kFALSE;
    }
  } else {
    printf("ERROR accessing socket!\n"); 
  }

  if (active) {
    //    activeLabelBMW->SetText("Beam Modulation script is ACTIVE");
    activeLabelBMW->SetText("Beam Modulation script is *******");
  } else {
    //    activeLabelBMW->SetText("Beam Modulation script is INACTIVE");
    activeLabelBMW->SetText("Beam Modulation script is *******");
  }
  MapSubwindows();
}

void GreenMonster::SCNDoRadio(Int_t id) {    
  
  switch (id) 
    {
    case SCN_RADIO_CLN:
      {
	if (statusSCN!=SCN_INT_CLN) {
	  SCNSetValues();  // first set data value to match input value
	  statusSCN=SCN_INT_CLN;
	  SCNSetStatus(statusSCN);
	} 
	break;
      }
    case SCN_RADIO_NOT:
      {
	if (statusSCN!=SCN_INT_NOT) {
	  statusSCN=SCN_INT_NOT;
	  SCNSetStatus(statusSCN);
	}
	break;
      }
    default:
      cout << "ERROR: Unrecognized SCAN status Button ID" << endl;
    }
  return;
}


void GreenMonster::SCNUpdateStatus(Int_t id) {    
  
  switch (id) 
    {
    case SCN_INT_CLN:
      {
	fStateRBtSCN[SCN_RADIO_NOT_BT]->SetState(kButtonUp);
	fStateRBtSCN[SCN_RADIO_CLN_BT]->SetState(kButtonDown);
	statusSCN=id;
	break;
      }
    case SCN_INT_NOT:
      {
	fStateRBtSCN[SCN_RADIO_CLN_BT]->SetState(kButtonUp);
	fStateRBtSCN[SCN_RADIO_NOT_BT]->SetState(kButtonDown);
	statusSCN=id;
	break;
      }
    default:
      cout << "ERROR: Unrecognized SCAN status flag" << endl;
    }
  return;
}


Bool_t GreenMonster::SCNCheckStatus() {
  struct greenRequest gRequest;
  int command, par1, par2, par3, command_type;
  char *msgReq = (char *)"SCN status check";
  char *reply = (char *)"Y";
  Int_t iclean;
  
  //  printf("SCN Status =");
  command_type = COMMAND_SCAN;   gRequest.command_type = command_type;
  command = SCAN_GET_STATUS;     gRequest.command = command;
  par1 = 0;                      gRequest.par1 = par1;
  par2 = 0;                      gRequest.par2 = par2;
  par3 = 0;                      gRequest.par3 = par3;
  strcpy(gRequest.message,msgReq);   gRequest.reply = reply;
  //if (GreenSockCommand(Crate_RightSpect,&gRequest) == SOCK_OK) {
  
  // if (GreenSockCommand(Crate_CountingHouse,&gRequest) == SOCK_OK) {
  //   if (GreenSockCommand(Crate_Injector,&gRequest) == SOCK_OK) {
 
  if (GreenSockCommand(scanCrate,&gRequest) == SOCK_OK) {
    command = gRequest.command;
    par1 = gRequest.par1;
    par2 = gRequest.par2;
    iclean=par1;
    if (iclean != SCN_INT_CLN && iclean != SCN_INT_NOT) {
      printf("UNKNOWN REPLY FOR SCN STATUS: %d  ",iclean);
      return kFALSE;
    }
  } else {
    printf("ERROR accessing socket!\n");
    return kFALSE;
  }
  
  //  printf("% d  %d\n",par1,par2);
  
  SCNUpdateStatus(iclean);
  return kTRUE;
}

void GreenMonster::SCNSetStatus(Int_t status) {
  struct greenRequest gRequest;
  int command, par1, par2, par3, command_type;
  char *msgReq = (char *)"SCN Status Change";
  char *reply = (char *)"Y";

  command_type = COMMAND_SCAN;  gRequest.command_type = command_type;
  command = SCAN_SET_STATUS;    gRequest.command = command;
  par1 = status;                gRequest.par1 = par1;
  par2 = 0;                     gRequest.par2 = par2;
  par3 = 0;                     gRequest.par3 = par3;

  cout << "Setting SCN status: " << par1 << endl;
  strcpy(gRequest.message,msgReq);   gRequest.reply = reply;
  //if (GreenSockCommand(Crate_RightSpect,&gRequest) == SOCK_OK) {
  //if (GreenSockCommand(Crate_CountingHouse,&gRequest) == SOCK_OK) {
  //  if (GreenSockCommand(Crate_Injector,&gRequest)==SOCK_OK) {
  if (GreenSockCommand(scanCrate,&gRequest) == SOCK_OK) {
    printf("SCAN status change call is complete\n");
  } else {
    printf("ERROR accessing socket!\n");
    return;
  }
  // check scan status
  SCNCheckStatus();
}


void GreenMonster::SCNSetValues() {
  int value;
  
  //
  // get value from tent
  //
  value = atoi(tentSetPnt1SCN->GetText());
  SCNSetValue(1,value);
  setpoint1SCN=value;

  value = atoi(tentSetPnt2SCN->GetText());
  SCNSetValue(2,value);
  setpoint2SCN=value;
 
  value = atoi(tentSetPnt3SCN->GetText());
  SCNSetValue(3,value);
  setpoint3SCN=value;

  value = atoi(tentSetPnt4SCN->GetText());
  SCNSetValue(4,value);
  setpoint4SCN=value;

  return;
}

void GreenMonster::SCNSetValue(Int_t which, Int_t value) {
  struct greenRequest gRequest;
  int command, par1, par2, par3, command_type;
  char *msgReq = (char *)"Set SCN Data Value";
  char *reply = (char *)"Y";
  
  cout << " writing new SCAN set point " << value 
       << " to data" << which <<endl;
  //
  // send set message for obj, value
  //
  command_type = COMMAND_SCAN;   gRequest.command_type = command_type;
  command = SCAN_SET_DATA;       gRequest.command = command;
  par1 = which;                  gRequest.par1 = par1;
  par2 = value;                  gRequest.par2 = par2;
  par3 = 0;                      gRequest.par3 = par3;
  strcpy(gRequest.message,msgReq);   gRequest.reply = reply;

  //if (GreenSockCommand(Crate_RightSpect,&gRequest) == SOCK_OK) {
  // if (GreenSockCommand(Crate_CountingHouse,&gRequest) == SOCK_OK) {
  // if (GreenSockCommand(Crate_Injector,&gRequest) == SOCK_OK) {
  if (GreenSockCommand(scanCrate,&gRequest) == SOCK_OK) {
    command = gRequest.command;
    par1 = gRequest.par1;
    par2 = gRequest.par2;
  } else {
    printf("ERROR accessing socket!\n"); 
    return;
  }
  return;
}

void GreenMonster::SCNCheckValues() {
  struct greenRequest gRequest;
  int command, par1, par2, par3, command_type;
  char *msgReq = (char *)"Check SCN Data Value";
  char *reply = (char *)"Y";
  char buff[10];
  int value;

  //
  command_type = COMMAND_SCAN;   gRequest.command_type = command_type;
  command = SCAN_GET_DATA;       gRequest.command = command;
  par1 = 1;                      gRequest.par1 = par1;
  par2 = 0;                      gRequest.par2 = par2;
  par3 = 0;                      gRequest.par3 = par3;
  strcpy(gRequest.message,msgReq);   gRequest.reply = reply;

  //if (GreenSockCommand(Crate_RightSpect,&gRequest) == SOCK_OK) {
  // if (GreenSockCommand(Crate_CountingHouse,&gRequest) == SOCK_OK) {
  //  if (GreenSockCommand(Crate_Injector,&gRequest)==SOCK_OK) {
  if (GreenSockCommand(scanCrate,&gRequest) == SOCK_OK) {
    command = gRequest.command;
    par1 = gRequest.par1;
    par2 = gRequest.par2;
    value = par2;
  } else {
    printf("ERROR accessing socket!\n"); 
    return;
  }

  //
  // get value from tent
  //
  setpoint1SCN=value;
  sprintf(buff, "%i", setpoint1SCN);
  tentSetPnt1SCN->SetText(buff);

  // repeat for second data word

  command_type = COMMAND_SCAN;   gRequest.command_type = command_type;
  command = SCAN_GET_DATA;       gRequest.command = command;
  par1 = 2;                      gRequest.par1 = par1;
  par2 = 0;                      gRequest.par2 = par2;
  par3 = 0;                      gRequest.par3 = par3;
  strcpy(gRequest.message,msgReq);   gRequest.reply = reply;

  //if (GreenSockCommand(Crate_RightSpect,&gRequest) == SOCK_OK) {
  // if (GreenSockCommand(Crate_CountingHouse,&gRequest) == SOCK_OK) {
  //  if (GreenSockCommand(Crate_Injector,&gRequest)==SOCK_OK) {
  if (GreenSockCommand(scanCrate,&gRequest) == SOCK_OK) {
    command = gRequest.command;
    par1 = gRequest.par1;
    par2 = gRequest.par2;
    value = par2;
  } else {
    printf("ERROR accessing socket!\n"); 
    return;
  }

  //
  // get value from tent
  //
  setpoint2SCN=value;
  sprintf(buff, "%i", setpoint2SCN);
  tentSetPnt2SCN->SetText(buff);

  // repeat for third data word

  command_type = COMMAND_SCAN;   gRequest.command_type = command_type;
  command = SCAN_GET_DATA;       gRequest.command = command;
  par1 = 3;                      gRequest.par1 = par1;
  par2 = 0;                      gRequest.par2 = par2;
  par3 = 0;                      gRequest.par3 = par3;
  strcpy(gRequest.message,msgReq);   gRequest.reply = reply;

  //if (GreenSockCommand(Crate_RightSpect,&gRequest) == SOCK_OK) {
  // if (GreenSockCommand(Crate_CountingHouse,&gRequest) == SOCK_OK) {
  //  if (GreenSockCommand(Crate_Injector,&gRequest)==SOCK_OK) {
  if (GreenSockCommand(scanCrate,&gRequest) == SOCK_OK) {
    command = gRequest.command;
    par1 = gRequest.par1;
    par2 = gRequest.par2;
    value = par2;
  } else {
    printf("ERROR accessing socket!\n"); 
    return;
  }

  //
  // get value from tent
  //
  setpoint3SCN=value;
  sprintf(buff, "%i", setpoint3SCN);
  tentSetPnt3SCN->SetText(buff);

  // repeat for fourth data word

  command_type = COMMAND_SCAN;   gRequest.command_type = command_type;
  command = SCAN_GET_DATA;       gRequest.command = command;
  par1 = 4;                      gRequest.par1 = par1;
  par2 = 0;                      gRequest.par2 = par2;
  par3 = 0;                      gRequest.par3 = par3;
  strcpy(gRequest.message,msgReq);   gRequest.reply = reply;

  //if (GreenSockCommand(Crate_RightSpect,&gRequest) == SOCK_OK) {
  // if (GreenSockCommand(Crate_CountingHouse,&gRequest) == SOCK_OK) {
  //  if (GreenSockCommand(Crate_Injector,&gRequest)==SOCK_OK) {
  if (GreenSockCommand(scanCrate,&gRequest) == SOCK_OK) {
    command = gRequest.command;
    par1 = gRequest.par1;
    par2 = gRequest.par2;
    value = par2;
  } else {
    printf("ERROR accessing socket!\n"); 
    return;
  }

  //
  // get value from tent
  //
  setpoint4SCN=value;
  sprintf(buff, "%i", setpoint4SCN);
  tentSetPnt4SCN->SetText(buff);


  return;
}

