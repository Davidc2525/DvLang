#if !defined(PROCESS_H)
#define PROCESS_H

#include "Instructions.h"
#include "../stack/estack.h"
#include "../heap/heap.h"
#include "strings_sets.h"
#include "../strings/strings.h"
#include <dlfcn.h>
#include <iostream>
#include <thread>
#include "../types.h"
#include <unordered_map>
#include <array>
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

#include <ffi.h>

namespace PROCESS
{

class Process;
class Cpu;
class Native;
class MetaFunction;
class LibLoad;

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



class MetaFunc{
public:
  char *name;
  char *signature;
  ffi_type type;
  unsigned int argc;
  unsigned int id;
  void * func;
  char* lib_name;
};

class Lib{
  public:
  Lib(){};
  Lib(char* lib_name);

  void load_func(int index_abs,MetaFunc * info);

  intdv call(int id_func,PROCESS::Cpu *cpu);

  char * name;
  void * handle;
  MetaFunc ** funs;
  //cif_info ** cache_funs;
  int index = 0;
};

class LibLoad{
  public:
    LibLoad();
    
    void load(Lib*);
    intdv call(int id_func,PROCESS::Cpu *cpu);    
    void load_func(MetaFunc * info);   

    int get_id_func(char*); 

    std::unordered_map<char*, Lib*> libs;
    std::unordered_map<char*, int> names;
    Lib ** funs;
    int index = 0;
};


class MetaFunction//TODO borrar
{
public:
  char *name;
  char *signature;
  unsigned int type;
  unsigned int argc;
  unsigned int id;
  void (*func)();
};

class Native//TODO borrar
{
  template <typename Func, Func f>
  friend class store_func_helper;

private:
  Native();

public:
  // Native();
  /* Here will be the instance stored. */
  static Native *instance;
  /* Static access method. */
  static Native *getInstance();

  Heap::Heap *heap;
  template <typename Func, Func f>
  void store(const std::string &name, MetaFunction meta);

  template <typename Ret, typename... Args>
  Ret call(int func, Args... args);

  template <typename Ret>
  Ret call_wrap(int func);

  template <typename Ret>
  Ret call_wrap2(int func, PROCESS::Cpu *cpu);

  int get(std::string name);

  template<typename fun>
  int _register_(fun func_r){
    int id = index;
    func_r(); 
    return id;
  };


  std::unordered_map<int, std::pair<MetaFunction, std::type_index>> m_func_map;
  std::unordered_map<std::string, int> names;
  //std::unordered_map<int, MetaFunction> metas;
  int index = 0;
};

#define FUNC_MAP_STORE(map, func, meta) (map).store<decltype(&func), &func>(#func, meta);

} // namespace PROCESS

#endif