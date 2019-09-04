
#include <strings/strings.h>
#include <numbers/numbers.h>
#include <types.h>
#include <process/DvVmEnv.h>
#include <heap/heap.h>
#include <iostream>
#include <unistd.h>

using namespace std;

void __attribute__((constructor)) std_init(void)
{
#if DEBUG
    cout << "iniciando libstd" << endl;
#endif
}

void __attribute__((destructor)) std_dest(void)
{
#if DEBUG
    cout << "deteniendo libstd" << endl;
#endif
}

extern "C"
{
    int strlen(DvVmEnv vm, dv_string str); //proto

    //--------- pasar a libstrconv
    intdv std_itos(DvVmEnv vm, intdv integer)
    {

        string _toString = to_string(integer);

        return STRING::String::c_str_to_dv_srt(vm.process->heap, _toString);
    }
    intdv std_ftos(DvVmEnv vm, intdv _float)
    {

        string _toString = to_string(NUMBERS::Numbers::ConverToFloat(_float));

        return STRING::String::c_str_to_dv_srt(vm.process->heap, _toString);
    }
    //--------- pasar a libstrconv

    int std_u_sleep(DvVmEnv vm, intdv usec)
    {
        usleep(usec * 1000);
        return usec * 1000;
    }

    int std_sleep(DvVmEnv vm, intdv sec)
    {
        sleep(sec);
        return sec;
    }

    int println(DvVmEnv vm, dv_string str)
    {
        char *c_str = STRING::String::dv_str_to_c_srt(vm.process->heap, (str));
        int size = vm.process->heap->get(str);
        for (int x = 0; x < size; x++)
        {
            cout << c_str[x];
        }
        cout << endl;
        //cout << c_str << endl;
        //printf("println: s:%d, %s\n",vm.process->heap->get(str),c_str);
        free(c_str);
        return strlen(vm, str);
    }

    int println_ref(DvVmEnv vm, dv_string str_ref)
    {
        char *c_str = STRING::String::dv_str_to_c_srt(vm.process->heap, vm.process->heap->get(str_ref));
        int size = vm.process->heap->get(vm.process->heap->get(str_ref));
        for (int x = 0; x < size; x++)
        {
            cout << c_str[x];
        }
        cout << endl;
        //cout << c_str << endl;
        //printf("println: s:%d, %s\n",vm.process->heap->get(str),c_str);
        free(c_str);
        return 0;
    }

    int print(DvVmEnv vm, dv_string str)
    {
        char *c_str = STRING::String::dv_str_to_c_srt(vm.process->heap, str);
        cout << c_str;
        free(c_str);
        return 0;
    }

    int strlen(DvVmEnv vm, dv_string str)
    {
        int l = 0;
        if (str == 0)
        {
            l = 0;
        }
        else
        {
            l = vm.process->heap->get(str);
            if (l > 0)
            {
                l = l - 1;
            }
        }
        return l;
    }

    //--memo

    int std_malloc(DvVmEnv vm, intdv size)
    {
        return vm.process->heap->Malloc(size);
    }

    int std_calloc(DvVmEnv vm, intdv size)
    {
        return vm.process->heap->Calloc(size);
    }

    int std_realloc(DvVmEnv vm, intdv adr, intdv new_size)
    {
        return vm.process->heap->Realloc(adr, new_size);
    }

    int std_free(DvVmEnv vm, dv_string str)
    {
        vm.process->heap->Free(str);
        return 1;
    }
}
