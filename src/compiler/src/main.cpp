#include <iostream>
#include "compiler/compiler.h"
//#include <heap/heap.h>

using namespace DVLANG;

int main(int argc, char **argv)
{

    PROCESS::Native *n = PROCESS::Native::getInstance();

    COMPILER_::Compiler com(argc, argv);
    std::cout << "compilado." << std::endl;

    return 0;
}