#include <vxWorks.h>

LOCAL int delay = 360; // taskdelay(3600) means 60 sec
//LOCAL int delay = 2*3600; // taskdelay(3600) means 60 sec

void auto_PITA();

void auto_PITA_print();
void auto_PITA_setDelay(int del);
