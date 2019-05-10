#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../GM/cfSock/cfSock_types.h"
#include "../GM/config/GMSock.h"
#include "../GM/timebrd/HAPTB_cf_commands.h"

class GVQWK
{
  private:
    int samples_val_1;
    int gate_val_1;
    int block_val_1;
    int samples_val_2;
    int gate_val_2;
    int block_val_2;

  public:
    GVQWK(int s1, int g1, int b1, int s2, int g2, int b2);
    int getSampVal1_in();
    int getGateVal1_in();
    int getBlokVal1_in();
    int getSampVal2_in();
    int getGateVal2_in();
    int getBlokVal2_in();
    void setSampVal1_in(int i);
    void setGateVal1_in(int i);
    void setBlokVal1_in(int i);
    void setSampVal2_in(int i);
    void setGateVal2_in(int i);
    void setBlokVal2_in(int i);
    
};
