#include "GTimeboard.h"

using namespace std;

GTimeboard::GTimeboard(int rd, int is, int os){
  crate_number = 0; crate_name = "CH";
  rd_value = rd; is_value = is; os_value = os;
}

int GTimeboard::getRDvalue_in(){return rd_value;}
void GTimeboard::setRDvalue_in(int i){rd_value = i;}
int GTimeboard::getISvalue_in(){return is_value;}
void GTimeboard::setISvalue_in(int i){is_value = i;}
int GTimeboard::getOSvalue_in(){return os_value;}
void GTimeboard::setOSvalue_in(int i){os_value = i;}

bool GTimeboard::getValsTB_in() {
  // Replace text entry fields with current values,
  // retrieved from TB
  char buff[10];
  int errFlag;
  struct greenRequest gRequest;
  int command, par1, par2, par3, command_type;
  char *msgReq = (char *)"TB Get Data";
  char *reply = (char *)"Y";
  
  command_type = COMMAND_HAPTB;    gRequest.command_type = command_type;
  command = HAPTB_GET_DATA;        gRequest.command = command;
  par1 = HAPTB_RD;                 gRequest.par1 = par1;
  par2 = 0;                        gRequest.par2 = par2;
  par3 = 0;                        gRequest.par3 = par3;
  strcpy(gRequest.message,msgReq);   gRequest.reply = reply;
  printf("COMMAND_HAPTB is %d \n",COMMAND_HAPTB); 
  errFlag = GreenSockCommand(crate_number,&gRequest);
 
  printf ("cfSockCommand returned :  %d \n",errFlag);
  if (errFlag == SOCK_OK) {
    command = gRequest.command;
    par1 = gRequest.par1;
    par2 = gRequest.par2;
    rd_value = par2;
  } else {
    std::cout << "ERROR accessing socket!" << std::endl;
  }
 
  command_type = COMMAND_HAPTB;    gRequest.command_type = command_type;
  command = HAPTB_GET_DATA;        gRequest.command = command;
  par1 = HAPTB_IT;                 gRequest.par1 = par1;
  par2 = 0;                        gRequest.par2 = par2;
  par3 = 0;                        gRequest.par3 = par3;
  strcpy(gRequest.message,msgReq); gRequest.reply = reply;
  errFlag = GreenSockCommand(crate_number,&gRequest);
  if (errFlag == SOCK_OK) {
    par2 = gRequest.par2;
    is_value = par2;
  } else {
    std::cout << "ERROR accessing socket!" << std::endl;
  }
  
  command_type = COMMAND_HAPTB;    gRequest.command_type = command_type;
  command = HAPTB_GET_DATA;        gRequest.command = command;
  par1 = HAPTB_OS;                 gRequest.par1 = par1;
  par2 = 0;                        gRequest.par2 = par2;
  par3 = 0;                        gRequest.par3 = par3;
  strcpy(gRequest.message,msgReq);   gRequest.reply = reply;
  errFlag = GreenSockCommand(crate_number,&gRequest);
  
  if (errFlag == SOCK_OK) {
    par2 = gRequest.par2;
    os_value = par2;
  } else {
    std::cout << "ERROR accessing socket!" << std::endl;
  }
  return true;
}

bool GTimeboard::setValsTB_in() {
  // Set current values of text entry fields to TB:
  int replyFlag, value;
  bool success = true;
  
  replyFlag = setParTB(HAPTB_RD,rd_value);
  if (replyFlag == 0) {
  } else if (replyFlag == 2){
    std::cout << "Cannot set parameter, CODA run in progress" << std::endl;
    //tentRD->SetText("RunInProgress");
    success = false;
  } else {
    std::cout << "Unknown error, cannot set TB parameter" << std::endl;
    //tentRD->SetText("ERROR");
    success = false;
  }
  
  replyFlag = setParTB(HAPTB_IT,is_value);
  if (replyFlag == 0) {
  } else if (replyFlag == 2){
    std::cout << "Cannot set parameter, CODA run in progress" << std::endl;
    //tentIT->SetText("RunInProgress");
    success = false;
  } else {
    std::cout << "Unknown error, cannot set TB parameter" << std::endl;
    //tentIT->SetText("ERROR");
    success = false;
  }

  replyFlag = setParTB(HAPTB_OS,os_value);
  if (replyFlag == 0) {
  } else if (replyFlag == 2){
    std::cout << "Cannot set parameter, CODA run in progress" << std::endl;
    //tentOS->SetText("RunInProgress");
    success = false;
  } else {
    std::cout << "Unknown error, cannot set TB parameter" << std::endl;
    //tentOS->SetText("ERROR");
    success = false;
  }
  
  return success;
}

int GTimeboard::setParTB(const int parNum, const int value) {
  // Set value of parameter on TB
  int errFlag;
  struct greenRequest gRequest;
  int command, command_type, par1, par2, par3;
  char *msgReq = (char *)"TB Set Data";
  char *reply = (char *)"Y";
  int replyFlag = 0;
  
  command_type = COMMAND_HAPTB;    gRequest.command_type = command_type;
  command = HAPTB_SET_DATA;        gRequest.command = command;
  par1 = parNum;                   gRequest.par1 = par1;
  par2 = value;                    gRequest.par2 = par2;
  par3 = 0;                        gRequest.par3 = par3;
  strcpy(gRequest.message,msgReq);   gRequest.reply = reply;
  
  errFlag = GreenSockCommand(crate_number,&gRequest);
  //errFlag = 0;
  //cout<<"The value being passed to setParTB here is: "<<value<<endl;
  //  printf ("cfSockCommand returned :  %d \n",errFlag);
  if (errFlag == SOCK_OK) {
    //printf ("GM: MESSAGE FROM SERVER:\n%s\n", msgReq);
    if ((int) gRequest.command != 1) {
      if ((int) gRequest.command == -2) {
	// this should be CODA running error
	replyFlag = 2;
      } else {
	printf("Error:Server replied with TB error code: %d \n",
	       (int) gRequest.command);
	replyFlag = 1;
      }
    } else {
      if (parNum != (int) gRequest.par1) {
	printf("Server replied with wrong TB number: %d  instead of %d \n",
	       (int) gRequest.par1, parNum);
	replyFlag = 1;
      }
      if (value != (int) gRequest.par2) {
	printf("Server replied with wrong TB set value: %d instead of %d \n",
	       (int) gRequest.par2, value);
	replyFlag = 1;
	}
    }
  } else {
    printf("setParTB:: ERROR accessing socket!");
    replyFlag = 1;
  }
  return replyFlag;
} 

extern "C"{
  GTimeboard* init(int rd, int is, int os){return new GTimeboard(rd, is, os);}
  int getRDvalue(GTimeboard* obj){return obj->getRDvalue_in();}
  void setRDvalue(GTimeboard* obj, int i){obj->setRDvalue_in(i);}
  int getISvalue(GTimeboard* obj){return obj->getISvalue_in();}
  void setISvalue(GTimeboard* obj, int i){obj->setISvalue_in(i);}
  int getOSvalue(GTimeboard* obj){return obj->getOSvalue_in();}
  void setOSvalue(GTimeboard* obj, int i){obj->setOSvalue_in(i);}
  
  bool getValsTB(GTimeboard* obj){obj->getValsTB_in();}
  bool setValsTB(GTimeboard* obj){obj->setValsTB_in();}
  
}
