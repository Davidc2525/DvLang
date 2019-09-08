#if !defined(LIBLOAD_H)
#define LIBLOAD_H


#ifdef _WIN32 
    #include <windows.h>
#elif __linux__
    // linux
    #include <dlfcn.h>
#elif __unix__ // all unices, not all compilers
    // Unix
#elif __APPLE__
    // Mac OS, not sure if this is covered by __posix__ and/or __unix__ though...
#endif

#include <ffi.h>
#include <unordered_map>


namespace PROCESS
{

class Cpu;

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

}

#endif