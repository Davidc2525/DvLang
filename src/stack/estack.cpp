#include <iostream>
#include <exception>
#include <stdlib.h>
#include "estack.h"
using namespace std;

namespace STACK
{

Stack::Stack(int ms)
{
    max_stack = ms;
    sp = 0;
    stack = new intdv[max_stack];
};

Stack::~Stack(){};

void Stack::Push(intdv val) 
{
    int nsp = sp++;
    if(sp>max_stack){
        cout<<"EXPRESION STACK OVERFLOW"<<endl;
        throw "EXPRESION STACK OVERFLOW";
        //exit(1);
    }
    stack[nsp] = val;
}
intdv Stack::Pop() 
{
    int nsp = --sp;
    if(sp<0){
        cout<<"EXPRESION STACK UNDERFLOW"<<endl;
        throw "EXPRESION STACK UNDERFLOW";
    }
    /*if(sp<0){
        cout<<"ERROR "<<ip<<endl;
        for(int x = ip-3;x<100;x++){
            if(x == ip){
            cout<<"\033[1;37m"<<x<<": "<<program[x]<<"\033[0;0m"<<endl;
            }else{
            cout<<x<<": "<<program[x]<<endl;
            }
            if(program[x] == HLT){
                break;
            }
        }

        cout<<"EXPRESION STACK UNDERFLOW"<<endl;
        exit(1);
    }*/
    return stack[nsp--];
}

} // namespace STACK
