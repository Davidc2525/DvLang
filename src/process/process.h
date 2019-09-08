#if !defined(PROCESS_H)
#define PROCESS_H

#include "Instructions.h"
#include "../stack/estack.h"
#include "../heap/heap.h"
#include "strings_sets.h"
#include "../strings/strings.h"
//#include <dlfcn.h>
#include <iostream>

#ifdef _WIN32 
  #if 1
    #include <thread>
  #else
    #include <mingw.thread.h> 
  #endif
#elif __linux__
// linux
  #include <thread>
#elif __unix__ // all unices, not all compilers
// Unix
#elif __APPLE__
// Mac OS, not sure if this is covered by __posix__ and/or __unix__ though...
#endif



#include "../types.h"
//#include <unordered_map>
//#include <array>
#include <map>
#include <typeinfo>
#include <typeindex>
#include <string>
#include <type_traits>
#include <iostream>
#include <assert.h>
#include <cxxabi.h>
#include <sstream>

#include <stdexcept>

#include "lib.h"


namespace PROCESS
{

class Process;
class Cpu;
class Native;
//class MetaFunction;
//class LibLoad;

class Args
{
public:
  //process
  intdv max_f_stack;
  intdv max_e_stack;
  intdv max_heap;
  intdv max_size_program;
  //cpu
  intdv main;
  intdv sizeP;
  int globals;
  intdv *program;
  STRING_SET::Entry **entrys;
  int argc;
  char** argv;
  // bool isFirstCpu;
};

class Process
{
private:
  bool firstCpuIsRunning = false;
  intdv *program;
  Args *defaultArgs;
  Cpu *cpus[1000000];
  int cpuCounter = 0;

public:
  Process(Args *args);
  ~Process();
  void start();
  int fork(Args *args);
  void init_cpu(int ccpu);
  int join();
  Cpu *get_cpu(int cpuid);
  Args *get_default_args();
  LibLoad * libload;
  //intdv heap_pointer;
  /*heap*/
  Heap::Heap *heap;
  /*GLOBALheap*/
  Heap::Heap *global_heap;
  /*global stack*/
  STACK::Stack *g_stack;
  /* frame stack */
  STACK::Stack *frame_stack;
  /*expresion stack*/
  STACK::Stack *estack;

  STRING_SET::Strings *strings_set;

  int frame_pointer = 0;
  int stack_pointer = 0;
};

//CPU
class CpuArgs
{
public:
  int max_f_stack;
  int max_e_stack;
  int globals;
  //int max_heap;
  int max_size_program;
  PROCESS::Process *process;
  bool isFirstCpu;
  int index;
};

class Cpu
{
public:
  Cpu(int mainPC, int size, intdv *p, CpuArgs *);
  Cpu();
  ~Cpu();
  void run();
  bool stop();
  void setDebugg(bool d) { debugger = d; };
  bool getDebugg() { return debugger; };
  void getIsFirstCpu();
  intdv next();
  intdv next2();
  void eval(intdv);
  int get_index();

  int flag[8] = {0,0,0,0,0,0,0,0};

  STACK::Stack *get_e_stack();
  STACK::Stack *get_f_stack();

  intdv count = 0;
  bool isFirstCpu = false;
  bool running = true;
  int ip = 0;
  intdv *program;
  Process *process;
  bool debugger = true;
  int cpu_index;
  int initial_frame; 
  int frame_pointer = 0;
  int stack_pointer = 0;
  int main;
  //

  /*global stack*/
  STACK::Stack *g_stack;
  /* frame stack */
  STACK::Stack *frame_stack;
  /*expresion stack*/
  STACK::Stack *estack;
};

/*typedef struct{
    ffi_cif * cif;
    bool cache;
} cif_info;*/






} // namespace PROCESS

#endif