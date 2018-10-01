#include <vxWorks.h>
#include <stdio.h>

//#include "../module_map.h"(stderr," \n *** Debug 1  \n");
#include "../module_map.h"

#include "auto_led.h"

void print_auto_led() {
  fprintf(stderr,"\n THis is all dummny... no auto_led to be used.\n");
  return;
}

STATUS init0AutoLED() {
  return(OK);
}

STATUS initAutoLED() {
  return(OK);
}

STATUS checkLockAutoLED() {
    return(ERROR);
}

int enableDiffAutoLED(int state) {
  return(state);
}

int enableBaseAutoLED(int state) {
  return(state);
}

int toggleDiffAutoLED() {
  return(0);
}

int requestToggleDiffAutoLED() {
  return(0);
}


int setBaseAutoLED(int value) {
    return(-1);
}

int setDiffAutoLED(int value) {
  return(-1);
}

int getDiffAutoLED() {
  return(-1);
}


int getBaseAutoLED() {
  return(-1);
}

int getStateDiffAutoLED() {
  return(-1);
}


STATUS stepAutoLED() {
  return(OK);
}

STATUS unlockAutoLED() {
  return(ERROR);
}

STATUS lockAutoLED() {
  return(ERROR);
}

int inProgressAutoLED() {
  return(-1);
}

STATUS cycleAutoLED() {
  return(ERROR);
}
