

#ifndef PARAMS_DVLANG_COMPILER_H_
#define PARAMS_DVLANG_COMPILER_H_

#include<iostream>
using namespace std;

namespace DVLANG
{
    namespace COMPILER_
    {
        class Params{
            public: 
                string input;
                string output;
                bool run = false;

                Params(){};
                Params(string input,string output);
        };
    }
}


#endif