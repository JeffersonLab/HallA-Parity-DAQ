#include "GScan.h"

using namespace std;

void GScan::SCNDoRadio(Int_t id) {    
  
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


void GScan::SCNUpdateStatus(Int_t id) {    
  
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


Bool_t GScan::SCNCheckStatus() {
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

void GScan::SCNSetStatus(Int_t status) {
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


void GScan::SCNSetValues() {
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

void GScan::SCNSetValue(Int_t which, Int_t value) {
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

void GScan::SCNCheckValues() {
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

