#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../GM/cfSock/cfSock_types.h"
#include "../GM/config/GMSock.h"
#include "../GM/timebrd/HAPTB_cf_commands.h"

#define GM_HAPTB   1000
#define GM_TB_GET  1001
#define GM_TB_SET  1002

class GTimeboard
{
  private:
    int crate_number;
    std::string crate_name;
    int rd_value;
    int is_value;
    int os_value;

    int setParTB(const int parNum, const int value);

  public:
    GTimeboard(int rd, int is, int os);
    int getRDvalue_in();
    void setRDvalue_in(int i);
    int getISvalue_in();
    void setISvalue_in(int i);
    int getOSvalue_in();
    void setOSvalue_in(int i); 
    bool getValsTB_in();
    bool setValsTB_in();
};
