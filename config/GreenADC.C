#include "GreenADC.h"
#include "GMSock.h"

//ClassImp(GreenADC)

GreenADC::GreenADC(Int_t crate_number, const char * crate_name,
		 const TGWindow *p, UInt_t w, UInt_t h) :
  TGCompositeFrame(p, w, h), crateNumber(crate_number)
{
  crateName = new TString(crate_name);
}

GreenADC::~GreenADC() {
}


void GreenADC::Init(ULong_t backgrnd) {

  TGLayoutHints *framout = new TGLayoutHints(kLHintsTop | kLHintsExpandX | 
					     kLHintsExpandY, 5, 5, 5, 5);

  SetBackgroundColor(backgrnd);
  TGCompositeFrame *tcf1 = new TGGroupFrame(this, crateName->Data(), 
					     kVerticalFrame);
  tcf1->SetBackgroundColor(backgrnd);
  AddFrame(tcf1,framout);
  TGCompositeFrame *tcf1a = new TGHorizontalFrame(this,200,200);
  tcf1a->SetBackgroundColor(backgrnd);
  TGCompositeFrame *tcf1b = new TGHorizontalFrame(this,200,200);
  tcf1b->SetBackgroundColor(backgrnd);
  TGLayoutHints *fL_Top = new TGLayoutHints(kLHintsTop, 2, 2, 5, 1);
  tcf1->AddFrame(tcf1a,fL_Top);
  tcf1->AddFrame(tcf1b,fL_Top);
  tcf1a->SetLayoutManager(new TGMatrixLayout(tcf1a, 0, 4, 10));
  tcf1b->SetLayoutManager(new TGMatrixLayout(tcf1b, 1, 0, 10));

  //
  // get initial info on ADCs in the crate
  //
  // get the number of ADCs

  fNumADC=getNumADC();

  if (fNumADC==-1) return;

  // column heads:
  TGLabel *lab;
  char buff[20];
  //   ADC labels, 1st column
  sprintf(buff, "ADC number");
  tcf1a->AddFrame(lab = new TGLabel(tcf1a, new TGString(buff)));
  lab->SetBackgroundColor(backgrnd);
  //   Gain settings, 2nd column
  sprintf(buff, "Gain (Lo/Hi)");
  tcf1a->AddFrame(lab = new TGLabel(tcf1a, new TGString(buff)));
  lab->SetBackgroundColor(backgrnd);
  //   DacNoise, 3rd column
  sprintf(buff, "DacNoise(On/Off)");
  tcf1a->AddFrame(lab = new TGLabel(tcf1a, new TGString(buff)));
  lab->SetBackgroundColor(backgrnd);
  //   current settings, 4th
  sprintf(buff, "Readback");
  tcf1a->AddFrame(lab = new TGLabel(tcf1a, new TGString(buff)));
  lab->SetBackgroundColor(backgrnd);

  for (Int_t i=0; i<fNumADC; i++) {

    //  Get label for i-th ADC
    //    fLabel[i]=i;  // use default label for now
    fLabel[i]=getLabelADC(i);
    char buff[30];

    // add ADC labels, 1st column
    TGLabel * lab;
    sprintf(buff, "ADC %d", fLabel[i]);
    lab = new TGLabel(tcf1a, new TGString(buff));
    lab->SetBackgroundColor(backgrnd);
    tcf1a->AddFrame(lab);
    
    // add gain radio buttons, second column
    TGCompositeFrame *tmpfrm = new TGHorizontalFrame(tcf1a,100,100);
    tmpfrm->SetBackgroundColor(backgrnd);
    tcf1a->AddFrame(tmpfrm);
    fGainRB[0][i] = new TGRadioButton(tmpfrm,"Lo",GAINRADIO+i*2+1);
    fGainRB[0][i]->SetBackgroundColor(backgrnd);
    fGainRB[0][i]->Associate(this);
    //    fGainRB[0][i]->Connect("Pressed()", "GreenADC", this, "DoRadio()");
    tmpfrm->AddFrame(fGainRB[0][i]);
    fGainRB[1][i] = new TGRadioButton(tmpfrm,"Hi",GAINRADIO+i*2+2);
    fGainRB[1][i]->SetBackgroundColor(backgrnd);
    fGainRB[1][i]->Associate(this);
    //    fGainRB[1][i]->Connect("Pressed()", "GreenADC", this, "DoRadio()");
    tmpfrm->AddFrame(fGainRB[1][i]);

    // add dacnoise radio buttons, third column
    tmpfrm = new TGHorizontalFrame(tcf1a,100,100);
    tmpfrm->SetBackgroundColor(backgrnd);
    tcf1a->AddFrame(tmpfrm);
    fDACRB[0][i] = new TGRadioButton(tmpfrm,"On",DACRADIO+i*2+1);
    fDACRB[0][i]->SetBackgroundColor(backgrnd);
    fDACRB[0][i]->Associate(this);
    //    fDACRB[0][i]->Connect("Pressed()", "GreenADC", this, "DoRadio()");
    tmpfrm->AddFrame(fDACRB[0][i]);
    fDACRB[1][i] = new TGRadioButton(tmpfrm,"Off",DACRADIO+i*2+2);
    fDACRB[1][i]->SetBackgroundColor(backgrnd);
    fDACRB[1][i]->Associate(this);
    //    fDACRB[1][i]->Connect("Pressed()", "GreenADC", this, "DoRadio()");
    tmpfrm->AddFrame(fDACRB[1][i]);

    // add settings label, fourth column
    fSetLab[i] = new TGLabel(tcf1a, new TGString("ADC Not Initialized     "));
    fSetLab[i]->SetBackgroundColor(backgrnd);
    tcf1a->AddFrame(fSetLab[i]);

    // get current setting info
    getValADC(i);
    fNewGain[i]=fGain[i];
    fNewDAC[i]=fDAC[i];

  }
  ResetRadio();



  tcf1->Resize(tcf1->GetDefaultSize());
  tcf1a->Resize(tcf1a->GetDefaultSize());

  TGTextButton *bt;

  tcf1b->AddFrame(bt = new TGTextButton(tcf1b,"Get Settings",GM_ADC_GET));
  bt->Associate(this);
  bt->Resize(90, bt->GetDefaultHeight());
  bt->SetBackgroundColor(backgrnd);
  bt->SetToolTipText("Display updated information for all ADCs in this crate");
  tcf1b->AddFrame(bt = new TGTextButton(tcf1b,"Apply Settings",GM_ADC_SET));
  bt->SetBackgroundColor(backgrnd);
  bt->Associate(this);
  bt->Resize(90, bt->GetDefaultHeight());
  bt->SetToolTipText("Apply settings to all ADCs in this crate");
//    tcf1b->AddFrame(bt = new TGTextButton(tcf1b,"Cancel",23));
//    bt->SetBackgroundColor(backgrnd);
//    bt->Associate(this);
//    bt->Resize(90, bt->GetDefaultHeight());

  tcf1b->Resize(tcf1b->GetDefaultSize());

  getValsADC();
}


Bool_t GreenADC::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
  // Process events generated by the buttons in the frame
    switch (GET_MSG(msg)) {
    case kC_COMMAND:
      switch (GET_SUBMSG(msg)) {
      case kCM_BUTTON:
	switch (parm1) 
	  {
	  case GM_ADC_GET:
	    {
	      getValsADC();
	      //	      printf("Get Values button pressed\n");
	      break;
	    }
	  case GM_ADC_SET:
	    {
	      setValsADC();
	      getValsADC();
	      //	      printf("Set Values button pressed\n");
	      break;
	    }
	  default:
	    printf("text button id %ld pressed\n", parm1);
	    break;
	  }
	break;
      case kCM_RADIOBUTTON:
	// std::cout << "Radio button pushed " << parm1 << std::endl;
	DoRadio(parm1);
	break;
      default:
	break;
      }
    default:
      break;
    }
  return kTRUE;
}

void GreenADC::ResetRadio() 
{
  for (Int_t i=0; i<fNumADC; i++) {
  // align settings on radio buttons with values of fGain and fDAC
    if (fGain[i]==GAINLO) {
      fGainRB[0][i]->SetState(kButtonDown);
      fGainRB[1][i]->SetState(kButtonUp);
    } else if (fGain[i]==GAINHI) {
      fGainRB[0][i]->SetState(kButtonUp);
      fGainRB[1][i]->SetState(kButtonDown);
    } else {
      fGainRB[0][i]->SetState(kButtonUp);
      fGainRB[1][i]->SetState(kButtonUp);
    }
    if (fDAC[i]==DACON) {
      fDACRB[0][i]->SetState(kButtonDown);
      fDACRB[1][i]->SetState(kButtonUp);
    } else if (fDAC[i]==DACOFF) {
      fDACRB[0][i]->SetState(kButtonUp);
      fDACRB[1][i]->SetState(kButtonDown);
    } else {
      fDACRB[0][i]->SetState(kButtonUp);
      fDACRB[1][i]->SetState(kButtonUp);
    }      
  }
}

void GreenADC::DoRadio(Int_t id)
{
  // Handle radio buttons: toggle between hi/lo and dac on/off
  if (id>=GAINRADIO && id<DACRADIO) {

    if (id%2==0) {
      int i2 = (id - GAINRADIO -2) /2;
      if (fGainRB[1][i2]->WidgetId() != id) {
	std::cout << "GreenADC::DoRadio: error in mapping IDs" << std::endl;
      } else {
	fGainRB[0][i2]->SetState(kButtonUp);
	fGainRB[1][i2]->SetState(kButtonDown);
	fNewGain[i2] = GAINHI; // high gain
      }
    } else {
      int i2 = (id - GAINRADIO -1) /2;
      if (fGainRB[0][i2]->WidgetId() != id) {
	std::cout << "GreenADC::DoRadio: error in mapping IDs" << std::endl;
      } else {
	fGainRB[1][i2]->SetState(kButtonUp);
	fGainRB[0][i2]->SetState(kButtonDown);
	fNewGain[i2] = GAINLO;  // low gain
      }
    }
  } else if (id>=DACRADIO) {
    
    if (id%2==0) {
      int i2 = (id - DACRADIO -2) /2;
      if (fDACRB[1][i2]->WidgetId() != id) {
	std::cout << "GreenADC::DoRadio: error in mapping IDs" << std::endl;
      } else {
	fDACRB[0][i2]->SetState(kButtonUp);
	fDACRB[1][i2]->SetState(kButtonDown);
	fNewDAC[i2] = DACOFF; // DAC off
      }
    } else {
      int i2 = (id - DACRADIO -1) /2;
      if (fDACRB[0][i2]->WidgetId() != id) {
	std::cout << "GreenADC::DoRadio: error in mapping IDs" << std::endl;
      } else {
	fDACRB[1][i2]->SetState(kButtonUp);
	fDACRB[0][i2]->SetState(kButtonDown);
	fNewDAC[i2] = DACON; // DAC on
      }
    }
    
  } else {
    std::cout << "GreenADC::DoRadio: bad button ID" << std::endl;
  }
}



Bool_t GreenADC::getValsADC() {
   // Replace settings labels with readback values from all ADCs
  Bool_t good=kTRUE;
  for (Int_t i=0; i<fNumADC; i++) good = getValADC(i) && good;
  ResetRadio();
  return good;
}

Bool_t GreenADC::getValADC(Int_t index) {
  // Replace settings label with readback values from ADC
  char buff[30];
  int errFlag;
  struct greenRequest gRequest;
  int command, command_type;
  long par1, par2, par3;
  char *msgReq = (char *)"ADC Get Data";
  char *reply = (char *)"Y";
  
  command_type = COMMAND_HAPADC;    gRequest.command_type = command_type;
  command = HAPADC_GET_CSR;        gRequest.command = command;
  par1 = index;                 gRequest.par1 = par1;
  par2 = 0;                         gRequest.par2 = par2;
  par3 = 0;                        gRequest.par3 = par3;
  strcpy(gRequest.message,msgReq);   gRequest.reply = reply;
  errFlag = GreenSockCommand(crateNumber,&gRequest);
  
  //   std::cout << "GreenSockCommand returned " << gRequest.message << std::endl;
  if (errFlag == SOCK_OK) {
    command = gRequest.command;
    par1 = gRequest.par1;
    par2 = gRequest.par2;
    par3 = 0;                        gRequest.par3 = par3;
    //     std::cout << "Command: " << command << " par1: " 
    //          << par1 << " par2: " << par2 << std::endl;
    if (par2==-1) { 
      //       not inited
      //       std::cout << "Board not initialized " << i << std::endl;
      fGain[index]=-1;
      fDAC[index]=-1;
      fSetLab[index]->SetText("ADC not initialized");
      return kFALSE;
    }
    if ((par2 & 0x10)==16) fGain[index]=GAINHI;
    else fGain[index]=GAINLO;
    if ((par2 & 0x8)==8) fDAC[index]=DACON;
    else fDAC[index]=DACOFF;
  } else {
    printf("ERROR accessing socket!");
    return kFALSE;
  }
  
  sprintf(buff, "%s Gain, DAC Noise %s", ( (fGain[index]==GAINLO) ? "Low" : "High"), 
	  (fDAC[index]==DACOFF ? "Off" : "On"));
  fSetLab[index]->SetText(buff);
  fSetLab[index]->Resize(fSetLab[index]->GetDefaultSize());
  
  return kTRUE;
}



Int_t GreenADC::getNumADC() {
  // get number of adcs listed in crate's utilities
   int errFlag;
   struct greenRequest gRequest;
   int command, par1, par2, par3, command_type;
   char *msgReq = (char *)"ADC Get Number";
   char *reply = (char *)"Y";
  
   command_type = COMMAND_HAPADC;    gRequest.command_type = command_type;
   command = HAPADC_GET_NUMADC;      gRequest.command = command;
   par1 = 0;                         gRequest.par1 = par1;
   par2 = 0;                         gRequest.par2 = par2;
   par3 = 0;                        gRequest.par3 = par3;
   strcpy(gRequest.message,msgReq);  gRequest.reply = reply;
   errFlag = GreenSockCommand(crateNumber,&gRequest);

   if (errFlag == SOCK_OK) {
     command = gRequest.command;
     par1 = gRequest.par1;
     par2 = gRequest.par2;
   } else {
     printf("ERROR accessing socket!");
     return -1;
   }
   return par2;
 }


Int_t GreenADC::getLabelADC(Int_t index) {
  // get integer label for ADC index, as listed in crate's utilities
   int errFlag;
   struct greenRequest gRequest;
   int command, par1, par2, par3, command_type;
   char *msgReq = (char *)"ADC Get Label";
   char *reply = (char *)"Y";
  
   command_type = COMMAND_HAPADC;    gRequest.command_type = command_type;
   command = HAPADC_GET_LABEL;       gRequest.command = command;
   par1 = index;                     gRequest.par1 = par1;
   par2 = 0;                         gRequest.par2 = par2;
   par3 = 0;                        gRequest.par3 = par3;
   strcpy(gRequest.message,msgReq);  gRequest.reply = reply;
   errFlag = GreenSockCommand(crateNumber,&gRequest);

   if (errFlag == SOCK_OK) {
     command = gRequest.command;
     par1 = gRequest.par1;
     par2 = gRequest.par2;
     //     std::cout << "Command: " << command << " " << msgReq << " par1: " 
     //	  << par1 << " par2: " << par2 << std::endl;
   } else {
     printf("ERROR accessing socket!");
     return -1;
   }
   return par2;
 }


Bool_t GreenADC::setValsADC() {
   // Read Buttons and set values for all ADCs
  Bool_t good=kTRUE;
  for (Int_t i=0; i<fNumADC; i++)  good =(setValADC(i)==0) && good;
  return good;
}


Int_t GreenADC::setValADC(Int_t index) { 
  // Set current values of Gain and DAC noise to ADC

  int errFlag;
  struct greenRequest gRequest;
  int command, command_type, par1, par2, par3;
  char *msgReq = (char *)"ADC Set Data";
  char *reply = (char *)"Y";
  Int_t replyFlag = 0;

  int value = 0;
  if (fNewGain[index]==GAINHI) value += 2; // set high gain
  if (fNewDAC[index]==DACON) value += 1;  // turn dac on

  command_type = COMMAND_HAPADC;    gRequest.command_type = command_type;
  command = HAPADC_SET_CSR;         gRequest.command = command;
  par1 = index;                 gRequest.par1 = par1;
  par2 = value;                     gRequest.par2 = par2;
  par3 = 0;                        gRequest.par3 = par3;
  strcpy(gRequest.message,msgReq);   gRequest.reply = reply;
  
  errFlag = GreenSockCommand(crateNumber,&gRequest);

  if (errFlag == SOCK_OK) {
    //printf ("GM: MESSAGE FROM SERVER:\n%s\n", msgReq);
    if ((int) gRequest.command != 1 ) {
      if ((int) gRequest.command == -2) {
	// this should be CODA running error
	replyFlag = 2;
      } else if ((int) gRequest.command == -1) {
	// bad adc (probably not initialized)
	replyFlag = 1;
      } else {
	printf("Error:Server replied with ADC error code: %d \n",
	       (int) gRequest.command);
	replyFlag = 1;
      } 
    } else {
      if (index != (int) gRequest.par1) {
	printf("Server replied with wrong ADC number: %d  instead of %d \n",
	       (int) fLabel[gRequest.par1], fLabel[index]);
	replyFlag = 1;
      } 
    }
  } else {
    printf("setValADC:: ERROR accessing socket!");
    replyFlag = 1;
  }
  
  if (replyFlag ==0 ) {
    if ((par2 & 16) == 16) fGain[index]=GAINHI; // extract gain bit from csr
    else fGain[index] = GAINLO;
    fGain[index] = ( ((par2 & 16) == 16) ? GAINHI : GAINLO );
    fDAC[index] = ( ((par2 & 8)==8) ? DACON : DACOFF );
    char buff[30];
    sprintf(buff, "%s Gain, DAC Noise %s", 
	    ( fGain[index]==GAINLO ? "Low" : "High"), 
	    (fDAC[index]==DACON ? "Off" : "On"));
    fSetLab[index]->SetText(buff);
  }
  return replyFlag;
}  





