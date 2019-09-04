

#include "Vm.h"
#include "../program/program.h"
#include "../process/Instructions.h"
//#include "../cpu/cpu.h"


#include <stdio.h>
#include <fstream>
#include <iostream>
#include <thread>
#include <cstring>

using namespace PROCESS;


namespace DvVm
{
Vm::Vm(DvVm::VmArgs * args)
{ 
    if (args->max_heap == 0)
    {
        args->max_heap = 10000;
        if (DEBUG)
        {
            cout << " MAX HEAP NO SET IN CONF, DEFAULT SET '10000': " << endl;
        }
    }

    if (args->program_file.empty())
    {
        if (DEBUG)
        {
            cout << " FILE PROGRAM NO SET, DvVm exit(1);" << endl;
            exit(1);
        }
    }

    string programfile = args->program_file;
    intdv maxheap = args->max_heap;
    intdv maxfstack = args->max_f_stack;
    intdv maxestack = args->max_e_stack;
    intdv maxsizeprogram = args->max_size_program;
    //int mainP = 0;
    //int sizeP = 55;

    PROGRAM::DataProgram * data;
    int pos = programfile.find_last_of(".");
    string ext = programfile.substr(pos+1);
    if(ext=="json"){ 
        data = PROGRAM::readProgramJson(programfile);
    }else if(ext=="dvc"){
        //data = PROGRAM::readProgram(programfile);
    }else{
        cout << "LA EXTENCION: "<< ext <<", NO ES ADECUADA. " << endl;
        exit(1);
    }
    

    PROCESS::Args *processArgs = new PROCESS::Args();
    
    //process args
    processArgs->max_heap = maxheap;
    processArgs->max_e_stack = maxestack;
    processArgs->max_f_stack = maxfstack; // TODO delete
    processArgs->max_size_program = maxsizeprogram;
    //cpu args
    processArgs->main = data->main;
    processArgs->sizeP = data->size;
    processArgs->program = data->inst;
    processArgs->entrys = data->entrys;
    processArgs->globals = data->globals;
    processArgs->argc = args->argc;
    processArgs->argv = args->argv;
    

    process = new PROCESS::Process(processArgs);
    process->libload = data->libload;
   
}
void Vm::run()
{
    process->start();
    /*while(true){
        process->start();
    }*/
}

void Vm::stop()
{
    //cpu->stop();
}
Vm::~Vm()
{

}

} // namespace DvVm
