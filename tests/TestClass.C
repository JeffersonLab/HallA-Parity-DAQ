#include "TestClass.h"

using namespace std;

TestClass::TestClass(){
  crate_id = 0;
  command1 = 2000; command2 = 2001;
  par1 = 0; par2 = 0; par3 = 0;
  message = "default"; reply = "standard";

}

TestClass::TestClass(int cid, long c1, long c2, long p1, long p2, long p3){
  crate_id = cid; command1 = c1; command2 = c2;
  par1 = p1; par2 = p2; par3 = p3;

  read_strings();
}

void TestClass::read_strings(){
  ifstream file("transfer.txt");
  string str; int ind = 0;
  while(getline(file, str)){
    if(ind == 0) message = str;
    else reply = str;
    ind++;
  }
  system("pwd");
  system("rm -f transfer.txt");
}

void TestClass::print_params_in(){
  cout<<"Crate ID: "<<crate_id<<endl;
  cout<<"Command Type: "<<command1<<endl;
  cout<<"Command ID: "<<command2<<endl;
  cout<<"Param 1: "<<par1<<endl;
  cout<<"Param 2: "<<par2<<endl;
  cout<<"Param 3: "<<par3<<endl;
  cout<<"Message: "<<message<<endl;
  cout<<"Reply: "<<reply<<endl;
}

extern "C"{
  TestClass* init(int cid, long c1, long c2, long p1, long p2, long p3){
    return new TestClass(cid, c1, c2, p1, p2, p3);
  }
  void print_params(TestClass* tc){tc->print_params_in();}
}
