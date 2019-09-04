#include <iostream>
#include <stdlib.h>
#include "DvVm/Vm.h"
#include "process/Instructions.h"
#include "heap/heap.h"
#include "stack/estack.h"
//#include "proto/objFile.pb.h"
#include "program/program.h"
#include "strings/strings.h"
//#include <stdio.h>
//#include <fstream>
//#include <iostream>
//#include <cstring>
//#include <stdlib.h>

//#define MAX_HEAP 50

//#include "posit.h"

using namespace std;
using namespace PROCESS;

int main(int argc, char **argv)
{   
#if DEBUG
    cout << "DvMv "<< cpu_arch<<"bits" << endl;
#endif
   
    if (argc < 2)
    {
        cout << "Falta el programa a ejecutar" << endl;
        cout << "Uso: DvVm 'rruta del programa'" << endl;
       // cout << "Uso depuracion: DvVmDB 'ruta del programa'" << endl;

        cout << "autor: \033[1;34mDavid25pcxtreme@gmail.com\033[0;0m" << endl;
        exit(0);
    }

    DvVm::VmArgs * vma = new DvVm::VmArgs;
   
    vma->program_file = argv[1];       //program file
    vma->argc = argc;
    vma->argv = argv;
    vma->max_heap  = 1024*1024*200;    // heap size
    vma->max_f_stack =  1000*1024*100; // frame stack
    vma->max_e_stack =  1024*1024*100; // expresion stack
    vma->max_size_program = 1000000;   // max size program
    
#if DEBUG  
    cout << "Parametros para maquina virtual:" << endl;
    cout << "Programa: " << vma->program_file << endl;
    cout << "Numero de argumentos para programa: " << vma->argc << endl;
    cout << "Pila de marcos: " << vma->max_f_stack << endl;
    cout << "Pila de expresiones: " << vma->max_e_stack << endl;
    cout << "Memoria maxima: " << vma->max_heap << endl;
    cout << "Tamaño maximo de programa: " << vma->max_size_program << endl;
#endif

    DvVm::Vm *maquina = new DvVm::Vm(vma);

    maquina->run();

    return 0;
}
