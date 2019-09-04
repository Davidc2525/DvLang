
#include <stdlib.h>
//#include <cstdlib>
//#include <pthread.h>
#include <unistd.h>
#include "process.h"

using namespace PROCESS;

Process::Process(Args *args)
{ 
    #if DEBUG
        cout << "Iniciando procesador" << endl;
    #endif
    defaultArgs = args;
    //estack = new STACK::Stack(defaultArgs->max_e_stack);
    heap = new Heap::Heap(defaultArgs->max_heap);
    heap->heap_pointer = args->max_heap;
   
    //libload = new LibLoad;
    global_heap = new Heap::Heap(defaultArgs->max_heap);
    strings_set = new STRING_SET::Strings();
    strings_set->set_entrys(args->entrys);
}

int Process::fork(Args *args)
{
    int ccpu = cpuCounter++;
    int maxfstack = args->max_f_stack;
    int maxestack = args->max_e_stack;
    int maxheap = args->max_heap;
    int maxsizeprogram = args->max_size_program;

    CpuArgs *cpuArgs = new CpuArgs();
    //CPU::Args * cpuArgs = new CPU::Args();
    cpuArgs->process = this;
    cpuArgs->max_size_program = args->max_size_program;
    cpuArgs->isFirstCpu = firstCpuIsRunning;
    cpuArgs->index = ccpu;   
    cpuArgs->globals = args->globals;
    cpuArgs->max_e_stack = maxestack;
    cpuArgs->max_f_stack = maxfstack;

 
    Cpu * cpu = new Cpu(args->main, args->sizeP, args->program, cpuArgs);
    cpus[cpu->get_index()] = cpu;

    firstCpuIsRunning = false;

    //cpu->run();
    ///cpuCounter--;
    return cpu->get_index();
    
}

void Process::init_cpu(int ccpu){
    Cpu*cpu = cpus[ccpu]; 
   
    int argc = defaultArgs->argc;
    int argv = this->heap->Malloc(argc);
    for(int x =0 ;x<argc;x++){
        this->heap->put(argv+1+x,STRING::String::c_str_to_dv_srt( this->heap,defaultArgs->argv[x]));
    }
    cpu->estack->Push(argc);
    cpu->estack->Push(argv);


    if(ccpu == 0){
        std::thread thread_cpu(&Cpu::run,cpu);
        thread_cpu.join();
    }else{
        std::thread thread_cpu(&Cpu::run,cpu);
        thread_cpu.detach();
        //sleep(1);
    }
}

Cpu * Process::get_cpu(int cpuid){
    return cpus[cpuid];
}
void Process::start()
{
    init_cpu(fork(defaultArgs));
}

Args * Process::get_default_args(){
    return defaultArgs;
}
Process::~Process()
{
}
