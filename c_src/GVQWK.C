#include "GVQWK.h"

using namespace std;

GVQWK::GVQWK(int s1, int g1, int b1, int s2, int g2, int b2){
  samples_val_1 = s1;
  gate_val_1 = g1;
  block_val_1 = b1;
  samples_val_2 = s2;
  gate_val_2 = g2;
  block_val_2 = b2;
}

int GVQWK::getSampVal1_in(){return samples_val_1;}
int GVQWK::getGateVal1_in(){return gate_val_1;}
int GVQWK::getBlokVal1_in(){return block_val_1;}
int GVQWK::getSampVal2_in(){return samples_val_2;}
int GVQWK::getGateVal2_in(){return gate_val_2;}
int GVQWK::getBlokVal2_in(){return block_val_2;}
void GVQWK::setSampVal1_in(int i){samples_val_1 = i;}
void GVQWK::setGateVal1_in(int i){gate_val_1 = i;}
void GVQWK::setBlokVal1_in(int i){block_val_1 = i;}
void GVQWK::setSampVal2_in(int i){samples_val_2 = i;}
void GVQWK::setGateVal2_in(int i){gate_val_2 = i;}
void GVQWK::setBlokVal2_in(int i){block_val_2 = i;}

bool GreenVQWK::getValsVQWK() {
  // Replace text entry fields with current values,
  // retrieved from VQWK
  char buff[10];
  int errFlag;
  struct greenRequest gRequest;
  int command, par1, par2, par3, command_type;
  char *msgReq = (char *)"VQWK Get Data";
  char *reply = (char *)"Y";
 
  command_type = COMMAND_VQWK;    gRequest.command_type = command_type;
  command = VQWK_GET_DATA;        gRequest.command = command;
  par1 = VQWK_SPB;                 gRequest.par1 = par1;
  par2 = 0;                        gRequest.par2 = par2;
  par3 = 0;                        gRequest.par3 = par3;
  strcpy(gRequest.message,msgReq);   gRequest.reply = reply;
   // printf("I am here where you thought I was");
  printf("COMMAND_VQWK is %d \n",COMMAND_VQWK); 
  errFlag = GreenSockCommand(crateNumber,&gRequest);
 
  printf ("cfSockCommand returned :  %d \n",errFlag);
  if (errFlag == SOCK_OK) {
    command = gRequest.command;
    par1 = gRequest.par1;
    par2 = gRequest.par2;
    CurrentSPB = par2;
    sprintf(buff, "%i", par2);
    tentSPB->SetText(buff);
  } else {
    std::cout << "ERROR accessing socket!" << std::endl;
  }
 
  command_type = COMMAND_VQWK;    gRequest.command_type = command_type;
  command = VQWK_GET_DATA;        gRequest.command = command;
  par1 = VQWK_GD;                 gRequest.par1 = par1;
  par2 = 0;                        gRequest.par2 = par2;
  par3 = 0;                        gRequest.par3 = par3;
  strcpy(gRequest.message,msgReq);   gRequest.reply = reply;
  errFlag = GreenSockCommand(crateNumber,&gRequest);
  if (errFlag == SOCK_OK) {
    par2 = gRequest.par2;
    CurrentGD = par2;
    sprintf(buff, "%i", par2);
    tentGD->SetText(buff);
  } else {
    std::cout << "ERROR accessing socket!" << std::endl;
  }
 
  command_type = COMMAND_VQWK;    gRequest.command_type = command_type;
  command = VQWK_GET_DATA;        gRequest.command = command;
  par1 = VQWK_NB;                 gRequest.par1 = par1;
  par2 = 0;                        gRequest.par2 = par2;
  par3 = 0;                        gRequest.par3 = par3;
  strcpy(gRequest.message,msgReq);   gRequest.reply = reply;
  errFlag = GreenSockCommand(crateNumber,&gRequest);
 
  if (errFlag == SOCK_OK) {
    par2 = gRequest.par2;
    CurrentNB = par2;
    sprintf(buff, "%i", par2);
    tentNB->SetText(buff);
  } else {
    std::cout << "ERROR accessing socket!" << std::endl;
  }
  return kTRUE;
}


Bool_t GreenVQWK::setValsVQWK(const int index) {
  // Set current values of text entry fields to VQWK:
  Int_t replyFlag;
  int value;
  Bool_t success=kTRUE;
  
  value = atoi(tentSPB->GetText());
  int NewSPB = value;
  replyFlag = setParVQWK(VQWK_SPB,value,index);
  if (replyFlag==0) {
    CurrentSPB=NewSPB;
  } else if (replyFlag==2){
    std::cout << "Cannot set parameter, CODA run in progress" << std::endl;
    tentSPB->SetText("RunInProgress");
    success = kFALSE;
  } else {
    std::cout << "Unknown error, cannot set VQWK parameter" << std::endl;
    //tentSPB->SetText("ERROR");
    success = kFALSE;
  }
  //  printf("Samples per block is : %d \n",atoi(tentSPB->GetText()));
  
  value = atoi(tentGD->GetText());
  int NewGD = value;
  replyFlag = setParVQWK(VQWK_GD,value,index);
  if (replyFlag==0) {
    CurrentGD=NewGD;
  } else if (replyFlag==2){
    std::cout << "Cannot set parameter, CODA run in progress" << std::endl;
    tentGD->SetText("RunInProgress");
    success = kFALSE;
  } else {
    std::cout << "Unknown error, cannot set VQWK parameter" << std::endl;
    //  tentGD->SetText("ERROR");
    success = kFALSE;
  }
  //printf("Gate Delay is : %d \n",atoi(tentGD->GetText()));
    
  value = atoi(tentNB->GetText());
  int NewNB = value;
  replyFlag = setParVQWK(VQWK_NB,value,index);
  if (replyFlag==0) {
    CurrentNB=NewNB;
  } else if (replyFlag==2){
    std::cout << "Cannot set parameter, CODA run in progress" << std::endl;
    tentNB->SetText("RunInProgress");
    success = kFALSE;
  } else {
    std::cout << "Unknown error, cannot set VQWK parameter" << std::endl;
    //tentNB->SetText("ERROR");
    success = kFALSE;
  }
  
  return success;
}

Int_t GreenVQWK::setParVQWK(const int parNum, const int value, const int index) {
  // Set value of parameter on VQWK
  int errFlag;
  struct greenRequest gRequest;
  int command, command_type, par1, par2, par3;
  char *msgReq = (char *)"VQWK Set Data";
  char *reply = (char *)"Y";
  Int_t replyFlag = 0;

  command_type = COMMAND_VQWK;    gRequest.command_type = command_type;
  command = VQWK_SET_DATA;        gRequest.command = command;
  par1 = parNum;                   gRequest.par1 = par1;
  par2 = value;                    gRequest.par2 = par2;
  par3 = index;                        gRequest.par3 = par3;
  strcpy(gRequest.message,msgReq);   gRequest.reply = reply;
  

  errFlag = GreenSockCommand(crateNumber,&gRequest);
    printf ("cfSockCommand returned :  %d \n",errFlag);
  if (errFlag == SOCK_OK) {
    printf ("GM: MESSAGE FROM SERVER:\n%s\n", msgReq);
    if ((int) gRequest.command != 1) {
      if ((int) gRequest.command == -2) {
	// this should be CODA running error
	replyFlag = 2;
      } else {
	printf("Error:Server replied with VQWK error code: %d \n",
	       (int) gRequest.command);
	replyFlag = 1;
      }
    } else {
      if (parNum != (int) gRequest.par1) {
	printf("Server replied with wrong VQWK number: %d  instead of %d \n",
	       (int) gRequest.par1, parNum);
	replyFlag = 1;
      }
      if (value != (int) gRequest.par2) {
	printf("Server replied with wrong VQWK set value: %d instead of %d \n",
	       (int) gRequest.par2, value);
	replyFlag = 1;
	}
    }
  } else {
    printf("setParVQWK:: ERROR accessing socket!");
    replyFlag = 1;
  }
  return replyFlag;
}

extern "C"{
  GVQWK* init(int s1, int g1, int b1, int s2, int g2, int b2){return new GVQWK(s1, g1, b1, s2, g2, b2);}
  int getSampVal1(GVQWK* obj){return obj->getSampVal1_in();}
  int getGateVal1(GVQWK* obj){return obj->getGateVal1_in();}
  int getBlokVal1(GVQWK* obj){return obj->getBlokVal1_in();}
  int getSampVal2(GVQWK* obj){return obj->getSampVal2_in();}
  int getGateVal2(GVQWK* obj){return obj->getGateVal2_in();}
  int getBlokVal2(GVQWK* obj){return obj->getBlokVal2_in();}
  void setSampVal1(GVQWK* obj, int i){obj->setSampVal1_in(i);}
  void setGateVal1(GVQWK* obj, int i){obj->setGateVal1_in(i);}
  void setBlokVal1(GVQWK* obj, int i){obj->setBlokVal1_in(i);}
  void setSampVal2(GVQWK* obj, int i){obj->setSampVal2_in(i);}
  void setGateVal2(GVQWK* obj, int i){obj->setGateVal2_in(i);}
  void setBlokVal2(GVQWK* obj, int i){obj->setBlokVal2_in(i);}
  
}
