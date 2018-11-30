#include "GreenADC18.h"
#include "GMSock.h"


//ClassImp(GreenADC)

GreenADC18::GreenADC18(Int_t crate_number, const char * crate_name,
		   const TGWindow *p, UInt_t w, UInt_t h) :
  TGCompositeFrame(p, w, h), crateNumber(crate_number)
{
  crateName = new TString(crate_name);
}

GreenADC18::~GreenADC18() {
}


void GreenADC18::Init(ULong_t backgrnd) {

  TGLayoutHints *framout = new TGLayoutHints(kLHintsTop | kLHintsExpandX | 
					     kLHintsExpandY, 
					     5, 5, 5, 5);
  TGLayoutHints *center = new TGLayoutHints(kLHintsTop|kLHintsCenterX ,5,5,5,5);
  //TGLayoutHints *left = new TGLayoutHints(kLHintsLeft ,5,5,5,5);
  //TGLayoutHints *right = new TGLayoutHints(kLHintsRight ,5,5,5,5);


  SetBackgroundColor(backgrnd);
  TGCompositeFrame *tcf1 = new TGGroupFrame(this, crateName->Data(), 
					    kVerticalFrame);
  tcf1->SetBackgroundColor(backgrnd);
  AddFrame(tcf1,framout);

  TGCompositeFrame *tcf1a = new TGHorizontalFrame(this,50,50);
  tcf1a->SetBackgroundColor(backgrnd);
  TGCompositeFrame *tcf1b = new TGHorizontalFrame(this,50,50);
  tcf1b->SetBackgroundColor(backgrnd);

  TGLayoutHints *fL_Top = new TGLayoutHints(kLHintsCenterX | 
					    kLHintsTop, 2, 2, 5, 1);
  TGLayoutHints *fL_Bot = new TGLayoutHints(kLHintsCenterX | 
					    kLHintsBottom, 2, 2, 5, 1);
  tcf1->AddFrame(tcf1a,fL_Top);
  tcf1->AddFrame(tcf1b,fL_Bot);

  tcf1a->SetLayoutManager(new TGMatrixLayout(tcf1a, 0, 8, 10));
  tcf1b->SetLayoutManager(new TGMatrixLayout(tcf1b, 1, 0, 10));

  //
  // get initial info on ADCs in the crate
  //
  // get the number of ADCs

  fNumADC=getNumADC();
  cout << "Number of ADC18 board to be loaded: " << fNumADC 
       << "\n" << endl;

  if (fNumADC==-1) return;

  // column heads:
  TGLabel *lab;
  char buff[20];

  //   ADC labels, 1st column
  sprintf(buff, "Label");
  tcf1a->AddFrame(lab = new TGLabel(tcf1a, new TGString(buff)));
  lab->SetBackgroundColor(backgrnd);

  //   IntGain settings, 2nd column
  sprintf(buff, "Int");
  tcf1a->AddFrame(lab = new TGLabel(tcf1a, new TGString(buff)));
  lab->SetBackgroundColor(backgrnd);

  //   ConvGain settings, 3nd column
  sprintf(buff, "Conv");
  tcf1a->AddFrame(lab = new TGLabel(tcf1a, new TGString(buff)));
  lab->SetBackgroundColor(backgrnd);

  //   DAC settings, 4nd column
  sprintf(buff, "-----");
  tcf1a->AddFrame(lab = new TGLabel(tcf1a, new TGString(buff)));
  lab->SetBackgroundColor(backgrnd);

  //   DAC settings, 4nd column
  sprintf(buff, "DAC");
  tcf1a->AddFrame(lab = new TGLabel(tcf1a, new TGString(buff)));
  lab->SetBackgroundColor(backgrnd);

  //   DAC settings, 4nd column
  sprintf(buff, "Settings");
  tcf1a->AddFrame(lab = new TGLabel(tcf1a, new TGString(buff)));
  lab->SetBackgroundColor(backgrnd);

  //   DAC settings, 4nd column
  sprintf(buff, "-----");
  tcf1a->AddFrame(lab = new TGLabel(tcf1a, new TGString(buff)));
  lab->SetBackgroundColor(backgrnd);

  //   sample, last column
  sprintf(buff, "Sample by:");
  tcf1a->AddFrame(lab = new TGLabel(tcf1a, new TGString(buff)));
  lab->SetBackgroundColor(backgrnd);


  for (Int_t i=0; i<fNumADC; i++) {

    fLabel[i]=getLabelADC(i);
    
    fIntGain[i]=999;
    fConvGain[i]=999;
    fDAC[i]=999;
    fSample[i]=999;

    char buff[6];

    // add ADC labels, 1st column
    TGLabel * lab;
    sprintf(buff, "ADC %d", fLabel[i]);
    lab = new TGLabel(tcf1a, new TGString(buff));
    lab->SetBackgroundColor(backgrnd);
    tcf1a->AddFrame(lab);
    
    TGTextBuffer *tbuf = new TGTextBuffer(6);
    // add int gain values, second column
    sprintf(buff, "N/A");
    tbuf->AddText(0,buff);
    intBox[i] = new TGTextEntry(tcf1a, tbuf, 221);
    intBox[i]->Resize(30, intBox[i]->GetDefaultHeight());
    tcf1a->AddFrame(intBox[i]);

    TGTextBuffer *tbuf2 = new TGTextBuffer(6);
    // add conv gain values, second column
    sprintf(buff, "N/A");
    tbuf2->AddText(0,buff);
    convBox[i] = new TGTextEntry(tcf1a, tbuf2, 222);
    convBox[i]->Resize(30, convBox[i]->GetDefaultHeight());
    tcf1a->AddFrame(convBox[i]);
 

    // add dacnoise radio buttons, third column

    fDACRB[0][i] = new TGRadioButton(tcf1a,"Tri",DACRADIO18+i*10+0);
    fDACRB[0][i]->SetBackgroundColor(backgrnd);
    fDACRB[0][i]->Associate(this);
    fDACRB[1][i] = new TGRadioButton(tcf1a,"Saw",DACRADIO18+i*10+1);
    fDACRB[1][i]->SetBackgroundColor(backgrnd);
    fDACRB[1][i]->Associate(this);
    fDACRB[2][i] = new TGRadioButton(tcf1a,"Const",DACRADIO18+i*10+2);
    fDACRB[2][i]->SetBackgroundColor(backgrnd);
    fDACRB[2][i]->Associate(this);
    fDACRB[3][i] = new TGRadioButton(tcf1a,"Off",DACRADIO18+i*10+3);
    fDACRB[3][i]->SetBackgroundColor(backgrnd);
    fDACRB[3][i]->Associate(this);

    tcf1a->AddFrame(fDACRB[0][i]);
    tcf1a->AddFrame(fDACRB[1][i]);
    tcf1a->AddFrame(fDACRB[2][i]);
    tcf1a->AddFrame(fDACRB[3][i]);


    //TGComboBox* temp = new TGComboBox(tcf1a,100);

    sampleBox.push_back(new TGComboBox(tcf1a,100));

    for (int k = 0; k < 4; k++) {
      TGString entry;
      entry+=pow(2,k);
      entry+="   ";
      sampleBox[i]->AddEntry(entry, k+1);
      }

    sampleBox[i]->Resize(65, 25);


    // get current setting info
    getValADC(i);


    sampleBox[i]->Select(1,kTRUE);

    tcf1a->AddFrame(sampleBox[i],center);


    //fNewIntGain[i]=fIntGain[i];
    //fNewConvGain[i]=fConvGain[i];
    fNewDAC[i]=fDAC[i];

    //ResetRadio(i);

  }


  //ResetRadio();


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
  tcf1b->AddFrame(bt = new TGTextButton(tcf1b,"Cancel",23));
  bt->SetBackgroundColor(backgrnd);
  bt->Associate(this);
  bt->Resize(90, bt->GetDefaultHeight());
  
  tcf1b->Resize(tcf1b->GetDefaultSize());


  getValsADC();


}


Bool_t GreenADC18::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
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
      //cout << "Radio button pushed " << parm1 << endl;
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


void GreenADC18::ResetRadio(Int_t i) 
{
  //for (Int_t i=0; i<fNumADC; i++) {
  // align settings on radio buttons with values of fGain and fDAC
    if (fDAC[i]==DACTRI) {
      fDACRB[0][i]->SetState(kButtonDown);
      fDACRB[1][i]->SetState(kButtonUp);
      fDACRB[2][i]->SetState(kButtonUp);
      fDACRB[3][i]->SetState(kButtonUp);
      //cout << "DAC for board #" << i << " has been set to: TRI " << endl;
    } else if (fDAC[i]==DACSAW) {
      fDACRB[0][i]->SetState(kButtonUp);
      fDACRB[1][i]->SetState(kButtonDown);
      fDACRB[2][i]->SetState(kButtonUp);
      fDACRB[3][i]->SetState(kButtonUp);
      //cout << "DAC for board #" << i << " has been set to: SAW" << endl;
    } else if (fDAC[i]==DACCONST) {
      fDACRB[0][i]->SetState(kButtonUp);
      fDACRB[1][i]->SetState(kButtonUp);
      fDACRB[2][i]->SetState(kButtonDown);
      fDACRB[3][i]->SetState(kButtonUp);
      //cout << "DAC for board #" << i << " has been set to: CONST" << endl;
    } else {
      fDACRB[0][i]->SetState(kButtonUp);
      fDACRB[1][i]->SetState(kButtonUp);
      fDACRB[2][i]->SetState(kButtonUp);
      fDACRB[3][i]->SetState(kButtonDown);
      //cout << "DAC for board #" << i << " has been set to: OFF" << endl;
      
    }      
    //}
}



void GreenADC18::DoRadio(Int_t id)
{
  //cout << "id=" << id << endl;
  
  Int_t adcnum, butnum;
  
  if (id>=DACRADIO18) {
    
    
    id-=DACRADIO18;
    adcnum=id/10;
    butnum=id%10;
    
    //cout << "adcnum=" << adcnum << endl;
    //cout << "but=" << butnum << endl;

    fDACRB[0][adcnum]->SetState(kButtonUp);
    fDACRB[1][adcnum]->SetState(kButtonUp);
    fDACRB[2][adcnum]->SetState(kButtonUp);
    fDACRB[3][adcnum]->SetState(kButtonUp);
    
    fDACRB[butnum][adcnum]->SetState(kButtonDown);
    
    fDAC[adcnum] = butnum; // DAC off
    
    
    
  } else {
    cout << "GreenADC18::DoRadio: bad button ID" << endl;
  }
}


Int_t GreenADC18::getNumADC() {
  // get number of adcs listed in crate's utilities
  int errFlag;
  struct greenRequest gRequest;
  int command, par1, par2, command_type;
  char *msgReq = (char *)"ADC Get Number";
  char *reply = (char *)"Y";
  
  command_type = COMMAND_ADC18;    gRequest.command_type = command_type;
  command = ADC18_GET_NUMADC;      gRequest.command = command;
  par1 = 0;                         gRequest.par1 = par1;
  par2 = 0;                         gRequest.par2 = par2;
  strcpy(gRequest.message,msgReq);  gRequest.reply = reply;
  errFlag = GreenSockCommand(crateNumber,&gRequest);

  if (errFlag == SOCK_OK) {
    command = gRequest.command;
    par1 = gRequest.par1;
    par2 = gRequest.par2;
  } else {
    printf("ERROR accessing socket!\n");
    return -1;
  }
  return par2;
}


Int_t GreenADC18::getLabelADC(Int_t index) {
  // get integer label for ADC index, as listed in crate's utilities
  int errFlag;
  struct greenRequest gRequest;
  int command, par1, par2, command_type;
  char *msgReq = (char *)"ADC Get Label";
  char *reply = (char *)"Y";
  
  command_type = COMMAND_ADC18;    gRequest.command_type = command_type;
  command = ADC18_GET_LABEL;       gRequest.command = command;
  par1 = index;                     gRequest.par1 = par1;
  par2 = 0;                         gRequest.par2 = par2;
  strcpy(gRequest.message,msgReq);  gRequest.reply = reply;
  errFlag = GreenSockCommand(crateNumber,&gRequest);

  if (errFlag == SOCK_OK) {
    command = gRequest.command;
    par1 = gRequest.par1;
    par2 = gRequest.par2;
  } else {
    printf("ERROR accessing socket!\n");
    return -1;
  }
  return par2;
}


Bool_t GreenADC18::getValsADC() {
  // Replace settings labels with readback values from all ADCs
  Bool_t good=kTRUE;
  for (Int_t i=0; i<fNumADC; i++) good = getValADC(i) && good;
  return good;
}
Bool_t GreenADC18::getValADC(Int_t index) {
  // get integer label for ADC index, as listed in crate's utilities
  char buff[10];
  int errFlag;
  struct greenRequest gRequest;
  int command, par1, par2, par3, command_type;
  char *msgReq = (char *)"ADC18 Get Sample";
  char *reply = (char *)"Y";
  
  command_type = COMMAND_ADC18;     gRequest.command_type = command_type;
  command = ADC18_GET_SAMP;         gRequest.command = command;
  par1 = index;                     gRequest.par1 = par1;
  par2 = 0;                         gRequest.par2 = par2;
  par3 = 0;                         gRequest.par3 = par3;
  strcpy(gRequest.message,msgReq);  gRequest.reply = reply;
  errFlag = GreenSockCommand(crateNumber,&gRequest);

  if (errFlag == SOCK_OK) {
    command = gRequest.command;
    par1 = gRequest.par1;
    par2 = gRequest.par2;
    fSample[index] = par2;
    //cout << "sample is currently set to:" << par2 << endl;

    if      (par2==1) sampleBox[index]->Select(1); 
    else if (par2==2) sampleBox[index]->Select(2);
    else if (par2==4) sampleBox[index]->Select(3);
    else if (par2==8) sampleBox[index]->Select(4);

  } else {
    printf("ERROR accessing socket! Int\n");
  }


  command_type = COMMAND_ADC18;     gRequest.command_type = command_type;
  command = ADC18_GET_INT;          gRequest.command = command;
  par1 = index;                     gRequest.par1 = par1;
  par2 = 0;                         gRequest.par2 = par2;
  par3 = 0;                         gRequest.par3 = par3;
  strcpy(gRequest.message,msgReq);  gRequest.reply = reply;
  errFlag = GreenSockCommand(crateNumber,&gRequest);

  if (errFlag == SOCK_OK) {
    command = gRequest.command;
    par1 = gRequest.par1;
    par2 = gRequest.par2;
    fIntGain[index] = par2;
    sprintf(buff, "%i", par2);
    intBox[index]->SetText(buff);
  } else {
    printf("ERROR accessing socket! Int\n");
  }


  command_type = COMMAND_ADC18;     gRequest.command_type = command_type;
  command = ADC18_GET_CONV;         gRequest.command = command;
  par1 = index;                     gRequest.par1 = par1;
  par2 = 0;                         gRequest.par2 = par2;
  par3 = 0;                         gRequest.par3 = par3;
  strcpy(gRequest.message,msgReq);  gRequest.reply = reply;
  errFlag = GreenSockCommand(crateNumber,&gRequest);

  if (errFlag == SOCK_OK) {
    command = gRequest.command;
    par2 = gRequest.par2;
    //    cout << "Command: " << command << " " << msgReq << " par1: " 
    // << par1 << " par2: " << par2 << endl;
    fConvGain[index] = par2;
    sprintf(buff, "%i", par2);
    convBox[index]->SetText(buff);
  } else {
    printf("ERROR accessing socket! Conv\n");
  }


  command_type = COMMAND_ADC18;     gRequest.command_type = command_type;
  command = ADC18_GET_DAC;         gRequest.command = command;
  par1 = index;                     gRequest.par1 = par1;
  par2 = 0;                         gRequest.par2 = par2;
  par3 = 0;                         gRequest.par3 = par3;
  strcpy(gRequest.message,msgReq);  gRequest.reply = reply;
  errFlag = GreenSockCommand(crateNumber,&gRequest);

  if (errFlag == SOCK_OK) {
    command = gRequest.command;
    par2 = gRequest.par2;

    //cout << "Command: " << command << " " << msgReq << " par1: " 
    //<< par1 << " par2: " << par2 << endl;
    
    if (par2==DACSAW){
      fDAC[index]=DACSAW;
    }
    else if (par2==DACCONST){
      fDAC[index]=DACCONST;
    }
    else if (par2==DACTRI){
      fDAC[index]=DACTRI;
    }
    else fDAC[index]=DACOFF18;
    
  } else {
    printf("ERROR accessing socket! Conv\n");
  }
  
  ResetRadio(index);

  return kTRUE;
  
  
}


Bool_t GreenADC18::setValsADC() {
  // Replace settings labels with readback values from all ADCs
  Bool_t good=kTRUE;
  for (Int_t i=0; i<fNumADC; i++) good = setValADC(i) && good;
  return good;
}


Bool_t GreenADC18::setValADC(Int_t index) {
  // get integer label for ADC index, as listed in crate's utilities
  char buff[10];
  int errFlag;
  struct greenRequest gRequest;
  int command, par1, par2, par3, command_type;
  char *msgReq = (char *)"ADC18 Set Data";
  char *reply = (char *)"Y";
  int value;


  // CalPed(index);

  // First set the int gain

  value = atoi(intBox[index]->GetText());

  if (value>=0&&value<=3) {
    fNewIntGain[index]=value;
    
    command_type = COMMAND_ADC18;     gRequest.command_type = command_type;
    command = ADC18_SET_INT;          gRequest.command = command;
    par1 = index;                     gRequest.par1 = par1;
    par2 = value;                     gRequest.par2 = par2;
    par3 = 0;                         gRequest.par3 = par3;
    strcpy(gRequest.message,msgReq);  gRequest.reply = reply;
    errFlag = GreenSockCommand(crateNumber,&gRequest);
    
    if (errFlag == SOCK_OK) {
      command = gRequest.command;
      par1 = gRequest.par1;
      par2 = gRequest.par2;
    } else {
      printf("ERROR accessing socket!\n");
    }
  } else {
    printf("ERROR: Int Value is out of range! Try (0-3)...\n");
    sprintf(buff, "%i", fIntGain[index]);
    intBox[index]->SetText(buff);
  }

  // Then set the conv gain 

  value = atoi(convBox[index]->GetText());

  if (value>=0&&value<=15) {
    
    fNewConvGain[index]=value;
    
    command_type = COMMAND_ADC18;     gRequest.command_type = command_type;
    command = ADC18_SET_CONV;         gRequest.command = command;
    par1 = index;                     gRequest.par1 = par1;
    par2 = value;                         gRequest.par2 = par2;
    par3 = 0;                         gRequest.par3 = par3;
    strcpy(gRequest.message,msgReq);  gRequest.reply = reply;
    errFlag = GreenSockCommand(crateNumber,&gRequest);
    
    if (errFlag == SOCK_OK) {
      command = gRequest.command;
      par2 = gRequest.par2;
    } else {
      printf("ERROR accessing socket!\n");
    }
  } else {
    printf("ERROR: Conv Value is out of range! Try (0-15)...\n");
    sprintf(buff, "%i", fConvGain[index]);
    convBox[index]->SetText(buff);
  }
 
  // Now setting type of DAC

  command_type = COMMAND_ADC18;     gRequest.command_type = command_type;
  command = ADC18_SET_DAC;         gRequest.command = command;
  par1 = index;                     gRequest.par1 = par1;
  par2 = fDAC[index];               gRequest.par2 = par2;
  par3 = 0;                         gRequest.par3 = par3;
  strcpy(gRequest.message,msgReq);  gRequest.reply = reply;
  errFlag = GreenSockCommand(crateNumber,&gRequest);
  
  if (errFlag == SOCK_OK) {
    command = gRequest.command;
    par2 = gRequest.par2;
    //cout << "dac has been set to:" << par2 << endl;
  } else {
    printf("ERROR accessing socket!\n");
  }

  // Finally, set the sample

  command_type = COMMAND_ADC18;     gRequest.command_type = command_type;
  command = ADC18_SET_SAMP;         gRequest.command = command;
  par1 = index;                     gRequest.par1 = par1;
  par2 = pow(2,sampleBox[index]->GetSelected()-1);    gRequest.par2 = par2;
  par3 = 0;                         gRequest.par3 = par3;
  strcpy(gRequest.message,msgReq);  gRequest.reply = reply;
  errFlag = GreenSockCommand(crateNumber,&gRequest);
  
  if (errFlag == SOCK_OK) {
    command = gRequest.command;
    par2 = gRequest.par2;
  } else {
    printf("ERROR accessing socket!\n");
  }
  
  if      (par2==1) sampleBox[index]->Select(1); 
  else if (par2==2) sampleBox[index]->Select(2);
  else if (par2==4) sampleBox[index]->Select(3);
  else if (par2==8) sampleBox[index]->Select(4);
  


  fNewIntGain[index]=fIntGain[index];
  fNewConvGain[index]=fConvGain[index];
  //fNewDAC[index]=fDAC[index];
  CalPed(index);

   return kTRUE;


}


void GreenADC18::CalPed(Int_t index) {


  TString file = "/adaqfs/home/apar/caryn/devices/adc18/ped_adcx";
  file+=fLabel[index];
  file+=".txt";

  const char *file_name = file;

  char ch;

  ifstream input(file_name);
  
  //  int num=1, i=0;//Caryn comment out



  string value;

  vector <string> igain;
  vector <string> cgain;
  vector <string> board;
  vector <string> channel;
  vector <string> dac;

  int column=0; 
  string temp;
  //  int conv_count=0; //Caryn comment out 

  while(input.good()) {
    
    input.get(ch);
    
    if (ch=='\t') {column++; temp="";}
    else if (ch=='\n') {column=0; temp="";}
    else {
      
      temp+=ch;
      
      if      (column==0) {igain.push_back(temp);    }
      else if (column==1) {cgain.push_back(temp);    }
      else if (column==2) {board.push_back(temp);    }
      else if (column==3) {channel.push_back(temp);  } 
      else if (column==5&&atoi(temp.c_str())>500) 
	{dac.push_back(temp);     temp=""; }
    }
  }
  
  /*cout <<igain.size()<<endl;
  cout <<cgain.size()<<endl;
  cout <<board.size()<<endl;
  cout <<channel.size()<<endl;
  cout <<dac.size()<<endl;*/
  
  
  input.close();
  
  cout << "int=" << fIntGain[index] 
    << " conv=" << fConvGain[index] 
    << " board=" << fLabel[index] 
    << endl;
    
    cout << "\nFor gain setting (" << fIntGain[index] << "," 
    << fConvGain[index] << ") " 
    << "you will need to apply the following command:\n" << endl;
  
  cout << "adc18_setpedestal(" << index;
  
  for (int chan=0; chan<4; chan++) {
    
    for (int j=0; j<(int) igain.size(); j++){
      
      
      TString buffer1;
      buffer1+=fIntGain[index];
      TString buffer2;
      buffer2+=fConvGain[index];
      TString buffer3;
      buffer3+=fLabel[index];
      TString buffer4;
      buffer4+=chan;
      
      Bool_t h = igain[j]==buffer1;
      Bool_t k = cgain[j]==buffer2;
      //      Bool_t l = board[j]==buffer3;//Caryn comment out
      Bool_t m = channel[j]==buffer4;


      
      if (h&&k&&m) {
	/*cout << buffer1 << endl;
	cout << buffer2 << endl;
	cout << buffer3 << endl;
	cout << buffer4 << endl;*/
	
	cout<<","<<dac[j];
	fPedDac[chan][index]=atoi(dac[j].c_str());
	break;
      }

      
    }
      ;
    
  }
  
  //  char buff[10];//Caryn comment out
  int errFlag;
  struct greenRequest gRequest;
  int command, par1, par2, par3, command_type;
  char *msgReq = (char *)"ADC18 Set Data";
  char *reply = (char *)"Y";
  //int value;
  
  // Oh, and we need to change the pedestal DAC
  
  for (int chan=0; chan<4; chan++) {
  
    command_type = COMMAND_ADC18;     gRequest.command_type = command_type;
    command = ADC18_SET_PED;          gRequest.command = command;
    par1 = index;                     gRequest.par1 = par1;
    par2 = chan;                      gRequest.par2 = par2;
    //par3 = 0;                         gRequest.par3 = par3;
    par3 = fPedDac[chan][index];      gRequest.par3 = par3;
    strcpy(gRequest.message,msgReq);  gRequest.reply = reply;
    errFlag = GreenSockCommand(crateNumber,&gRequest);
    
    if (errFlag == SOCK_OK) {
      command = gRequest.command;
      par2 = gRequest.par2;
      //par3 = gRequest.par3;
      //cout << "dac has been set to:" << par2 << endl;
    } else {
      printf("ERROR accessing socket!\n");
    }
  }

  cout << ")\n" <<  endl;

}
  










