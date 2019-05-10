#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../GM/cfSock/cfSock_types.h"
#include "../GM/config/GMSock.h"
#include "../GM/timebrd/HAPTB_cf_commands.h"

#define BMW_KILL    2001
#define BMW_GET_STATUS    2002
#define BMW_START   2003
#define BMW_CHECK_ALIVE   2004
#define BMW_TEST_START   2005
#define BMW_TEST_SET_DATA   2006
#define BMW_UNKILL    2007

using namespace std;

class GBMW{
  private:
    string killSwitchOff;
    string killSwitchOn;
    string defaultStatus;
    string defaultActive;
    string defaultButton;
    string killSwitchText;
    string statusText;
    string activeText;
    string BMWButtonText;
    int BMWTestType;
    int setPoint;

    void setKSText_in(string newKSText);
    void setStatText_in(string newStatText);
    void setActText_in(string newActText);
    void setButtonText_in(string newButtonText);

    void BMWTestStep_in();
    void BMWDelayCheck_in();
    void BMWCheckActiveFlag_in();

  public:
    GBMW();
    void BMWStartTest_in();
    void BMWChangeStatus_in();
    void BMWSetKill_in();
    bool BMWCheckStatus_in();
    void BMWActiveProbe_in();

    string getKSText_in();
    string getStatText_in();
    string getActText_in();
    string getButtonText_in();
    int getTestType_in();
    void setTestType_in(int newTestType);
    int getSetPoint_in();
    void setSetPoint_in(int newSetPoint);
};
