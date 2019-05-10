#include "GBMW.h"

using namespace std;

GBMW::GBMW(){
  killSwitchOff = "OFF";
  killSwitchOn  = "ON";
  defaultStatus = "**********";
  defaultActive = "****";
  defaultButton = "Start Beam Modulation";

  killSwitchText = "Kill Switch is " + killSwitchOff;
  statusText = defaultStatus;
  activeText = "Beam Modulation script is " + defaultActive;
  BMWButtonText = defaultButton;
  BMWTestType = 0;
  setPoint = 0;
  
}

string GBMW::getKSText_in(){
  cout<<"Are other text variables unhealthy?"<<endl;
  cout<<"        "<<setPoint<<endl;
  cout<<"Retrieiving kill switch which is "<<killSwitchText<<"..."<<endl;
  return killSwitchText;
}
void GBMW::setKSText_in(string newKSText){
  cout<<"Changing KS Text. It is"<<killSwitchText<<", and it will now be "<<newKSText<<endl;
  killSwitchText = newKSText;
  cout<<"KS text changed. It is now "<<killSwitchText<<endl;
}
string GBMW::getStatText_in(){return statusText;}
void GBMW::setStatText_in(string newStatText){statusText = newStatText;}
string GBMW::getActText_in(){return activeText;}
void GBMW::setActText_in(string newActText){activeText = newActText;}
string GBMW::getButtonText_in(){return BMWButtonText;}
void GBMW::setButtonText_in(string newButtonText){BMWButtonText = newButtonText;}
int GBMW::getTestType_in(){return BMWTestType;}
void GBMW::setTestType_in(int newTestType){BMWTestType = newTestType;}
int GBMW::getSetPoint_in(){return setPoint;}
void GBMW::setSetPoint_in(int newSetPoint){setPoint = newSetPoint;}

void GBMW::BMWStartTest_in() {    
  struct greenRequest gRequest;
  int command, command_type, par1, par2, par3;
  char *msgReq = (char *)"BMW Start Test";
  char *reply = (char *)"Y";

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
  BMWTestStep_in();

}

void GBMW::BMWTestStep_in() {    
  int value;
  bool kill_switch;

  struct greenRequest gRequest;
  int command, command_type, par1, par2, par3;
  char *msgReq = (char *)"BW Test Set Data";
  char *reply = (char *)"Y";

  //
  // get value from tent
  //
  value = setPoint;
  cout << " writing new set point " << value << " to " << getTestType_in() << endl;
  //
  // send set message for obj, value
  //
  command_type = COMMAND_BMW;    gRequest.command_type = command_type;
  command = BMW_TEST_SET_DATA;   gRequest.command = command;
  par1 = getTestType_in();            gRequest.par1 = par1;
  par2 = value;                  gRequest.par2 = par2;
  par3 = 0;                      gRequest.par3 = par3;
  strcpy(gRequest.message,msgReq);   gRequest.reply = reply;

  if (GreenSockCommand(Crate_CountingHouse,&gRequest) == SOCK_OK) {
    command = gRequest.command;
    par1 = gRequest.par1;
    par2 = gRequest.par2;
    // par1 is kill switch!
    if (par1 != 0) {
      kill_switch = true;
    } else {
      kill_switch = false;
    }
  } else {
    printf("ERROR accessing socket!\n"); 
    return;
  }
  
  /**if (!kill_switch) {    
    TTimer* ctimer = new TTimer(4000,true);
    TQObject::Connect(ctimer, "Timeout()", "GreenMonster", this, 
		      "BMWTestStep_in()");
    ctimer->Start(4000, true);
  }**/

}

void GBMW::BMWChangeStatus_in() {
  struct greenRequest gRequest;
  int command, par1, par2, par3, command_type;
  char *msgReq = (char *)"BMW Status Change";
  char *reply = (char *)"Y";
  // get bmw status
  bool bmw_running = BMWCheckStatus_in();

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
  BMWCheckStatus_in();
  printf("BMWChangeStatus():: BMW_START = %d\n", BMW_START);
  printf("BMWChangeStatus():: command = %d\n", command);
  printf("BMWChangeStatus():: Exiting...\n");
}


void GBMW::BMWSetKill_in() {
  struct greenRequest gRequest;
  int command, par1, par2, par3, command_type;
  char *msgReq = (char *)"BMW status check";
  char *reply = (char *)"Y";
  bool bmw_running,kill_switch;

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
      bmw_running = true;
    } else { 
      bmw_running = false;
    }
    if (par2 != 0) {
      kill_switch = true;
    } else { 
      kill_switch = false;
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

  BMWCheckStatus_in();  // check bmw status
}

bool GBMW::BMWCheckStatus_in() {
  // get bmw status
  struct greenRequest gRequest;
  int command, par1, par2, par3, command_type;
  char *msgReq = (char *)"BMW status check";
  char *reply = (char *)"Y";
  bool bmw_running,kill_switch;
  
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
      bmw_running = true;
      printf("BMWCheckStatus():: bmw_running = %d\n", bmw_running);
    } else { 
      bmw_running = false;
      printf("BMWCheckStatus():: bmw_running = %d\n", bmw_running);
    }
    if (par2 != 0) {
      kill_switch = true;
    } else { 
      kill_switch = false;
    }
  } else {
    printf("ERROR accessing socket!\n");
    return false;
  }
  
  //  printf("% d  %d\n",par1,par2);

  if (kill_switch) {
    cout<<"Inside KS block..."<<endl;
    setKSText_in("Kill Switch is " + killSwitchOn);
  } else {
    setKSText_in("Kill Switch is " + killSwitchOff);
  }
  if (bmw_running) {
    cout<<"Inside running block..."<<endl;
    setStatText_in("Beam Modulation is " + killSwitchOn);
    setButtonText_in("Set Kill Switch");
    return true;
  } else {
    setStatText_in(defaultStatus);
    setButtonText_in(defaultButton);
    return false;
  }
}

void GBMW::BMWDelayCheck_in() {
  BMWCheckActiveFlag_in();
}

void GBMW::BMWActiveProbe_in() {
  // get bmw status
  BMWCheckActiveFlag_in();
  // recheck after some delay
  //    in order to use timer, or any other QT relying tool, 
  //    one must generate a dictionary using rootcint... 
  /**TTimer* ctimer = new TTimer(4000,true);
  TQObject::Connect(ctimer, "Timeout()", "GreenMonster", this, 
		    "BMWDelayCheck()");
  ctimer->Start(1000, true);**/
  cout<<"Kill switch text?"<<endl;
  cout<<"        "<<killSwitchText<<endl;
}

void GBMW::BMWCheckActiveFlag_in() {
  // get bmw status
  struct greenRequest gRequest;
  int command, par1, par2, par3, command_type;
  char *msgReq = (char *)"BMW status check";
  char *reply = (char *)"Y";
  bool active=false;
  
  command_type = COMMAND_BMW;    gRequest.command_type = command_type;
  command = BMW_CHECK_ALIVE;     gRequest.command = command;
  par1 = 0;                      gRequest.par1 = par1;
  par2 = 0;                      gRequest.par2 = par2;
  par3 = 0;                      gRequest.par3 = par3;
  strcpy(gRequest.message,msgReq);   gRequest.reply = reply;
  if (GreenSockCommand(Crate_CountingHouse,&gRequest) == SOCK_OK) {
    par2 = gRequest.par2;
    if (par2 != 0) {
      active = true;
    } else { 
      active = false;
    }
  } else {
    printf("ERROR accessing socket!\n"); 
  }

  if (active) {
    setActText_in("Beam Modulation script is " + defaultActive);
  } else {
    setActText_in("Beam Modulation script is " + defaultActive);
  }
}

extern "C"{
  GBMW* init(){return new GBMW();}
  string getKSText(GBMW* obj){
    cout<<"Starting outer method..."<<endl;
    string return_text = obj->getKSText_in();
    cout<<"On the other side!"<<endl;
    cout<<"I've got the value. I'm returning "<<return_text<<"..."<<endl;
    return return_text;
  }
  string getStatText(GBMW* obj){return obj->getStatText_in();}
  string getActText(GBMW* obj){return obj->getActText_in();}
  string getButtonText(GBMW* obj){return obj->getButtonText_in();}
  int getTestType(GBMW* obj){return obj->getTestType_in();}
  int getSetPoint(GBMW* obj){return obj->getSetPoint_in();}
  void setTestType(GBMW* obj, int newTestType){obj->setTestType_in(newTestType);}
  void setSetPoint(GBMW* obj, int newSetPoint){obj->setSetPoint_in(newSetPoint);}

  bool BMWCheckStatus(GBMW* obj){return obj->BMWCheckStatus_in();}
  void BMWActiveProbe(GBMW* obj){return obj->BMWActiveProbe_in();}
  void BMWChangeStatus(GBMW* obj){return obj->BMWChangeStatus_in();}
  void BMWStartTest(GBMW* obj){return obj->BMWStartTest_in();}
  void BMWSetKill(GBMW* obj){return obj->BMWSetKill_in();}
}
