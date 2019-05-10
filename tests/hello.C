#include <iostream>

using namespace std;

class Hello{
  public:
    void speak(){
      cout<<"Eat my ass, raw"<<endl;
    }
    
    int return_int_internal(){
      //int ta_dah[5];
      //ta_dah[0] = 6; ta_dah[1] = 15; ta_dah[2] = 7; ta_dah[3] = 142; ta_dah[4] = 2;
      int ta_dah = 5;
      return ta_dah;
    }
};

extern "C"{
  Hello* init(){return new Hello();}
  void say_it_all(Hello* hello){hello->speak();}
  int return_int(Hello* hello){return hello->return_int_internal();}
  int add_two(int i){
    return i + 2;
  }
}
