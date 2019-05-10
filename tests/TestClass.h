#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <bits/stdc++.h> 

using namespace std;

class TestClass{
  private:
    int crate_id;
    long command1;
    long command2;
    long par1;
    long par2;
    long par3;
    string message;
    string reply;
    
    TestClass();
    void read_strings();

  public:
    TestClass(int cid, long c1, long c2, long p1, long p2, long p3);
    void print_params_in();
};
