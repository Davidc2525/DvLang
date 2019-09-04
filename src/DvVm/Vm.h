
#ifndef DEBUG
#define DEBUG false
#endif

#include <stdlib.h>
#include <iostream>
//#include "../cpu/cpu.h"
#include "../process/process.h"

using namespace std;

namespace DvVm
{



class VmArgs{
  public:
    string program_file;
    int argc;
    char** argv;
    intdv max_heap;
    intdv max_f_stack;
    intdv max_e_stack;
    intdv max_size_program;
};

class Vm
{
  public:
    Vm(VmArgs*);
    ~Vm();
    void run();
    void stop();

  private:
    //CPU::Cpu *cpu;
  PROCESS::Process * process;
};

} // namespace DvVm
