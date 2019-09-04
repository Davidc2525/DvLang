
#include "process.h"
#ifndef _DvVmEnv_H_
#define _DvVmEnv_H_

typedef struct
{
    //char* (*dv_str_to_c_str)(intdv str);
    PROCESS::Process *process;
    int cpu_id;
} DvVmEnv;

#endif
