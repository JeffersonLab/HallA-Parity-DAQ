#include <vxWorks.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include "../module_map.h"

using namespace std;

extern void setCleanSCAN(int);
extern void setDataSCAN(int,int);

void SetClean(int val);
void SetScanData(int ch, int val);
void usage();

int main(int argc, char **argv){
  if(argc==1 || ( strcmp("--help",argv[1])==0 ) ){
    usage();
    return 0;
  }else if( strcmp("--setClean",argv[1])==0 ){
    int val = atoi(argv[2]);
    if(val==0 || val==1)
      SetClean(val);
    else{
      cout<<"setClean can only accept 0 or 1. You provided: "<<val<<endl;
      return 1;
    }
    return 0;
  }else if( strcmp("--setScanData",argv[1])==0 ){
    int ch = atoi(argv[2]);
    int val = atoi(argv[3]);
    if(ch!=1 || ch!=2 || ch!=3 || ch!=4){
      cout<<"setScanData can only accept 1,2,3 or 4. You provided: "<<ch<<endl;
      return 1;
    }else if(abs(val)<32766){
      SetClean(0);
      SetScanData(ch,val);
      return 0;
    }else{
      cout<<"setScanData can only take |value| <32766. You provided: "<<val<<endl;
      return 1;
    }
  }else{
    usage();
    return 0;
  }

  return 0;
}

void usage(){
  cout<<"usage: autoAPI [options]: this only runs one option at a time\n"
      <<" --setClean [0/1]: set clean data flag. option can be only 0 or 1\n"
      <<" --setScanData <ch> <val> : ch can be 1-4, val can be in int\n"
      <<" --help: this printout\n";
}

void SetClean(int val){
    setCleanSCAN(val);
    cout<<"\nClean now set to: "<<val<<endl;
}

void SetScanData(int ch, int val) {    
    setDataSCAN(ch,val);
}
