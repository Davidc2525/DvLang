#include "_socket_.h"

#include <strings/strings.h>
#include <types.h>
#include <process/DvVmEnv.h>
#include <heap/heap.h>
#include <iostream>
#include <vector>

using namespace std;

//------------------
static _Socket_ *_socket_;
bool stoped = false;
void __attribute__((constructor)) std_init(void)
{
#if DEBUG
    cout << "iniciando libnet" << endl;
#endif
    _socket_ = _Socket_::getInstace();
}

void __attribute__((destructor)) std_dest(void)
{
#if DEBUG
    cout << "deteniendo libnet" << endl;
#endif
    if (!stoped)
    {
        free(_socket_);
        stoped = true;
    }
}

extern "C"
{

    intdv net_create_socket(DvVmEnv vm, dv_string adr, intdv port)
    {
        return _socket_->create_socket(0, 0, STRING::String::dv_str_to_c_srt(vm.process->heap, adr), port);
    }

    intdv net_connect(DvVmEnv vm, dv_string adr, intdv port)
    {
        return _socket_->_connect(STRING::String::dv_str_to_c_srt(vm.process->heap, adr), port);
    }

    intdv net_accept(DvVmEnv vm, intdv s_fd)
    {
        return _socket_->_accept(s_fd);
    }

    intdv net_write(DvVmEnv vm, intdv s_fd, dv_string content)
    {
        int len = vm.process->heap->get(content);

        char *ctn = STRING::String::dv_str_to_c_srt(vm.process->heap, content);

        return _socket_->_write(s_fd, ctn, len);
    }

    intdv net_send(DvVmEnv vm, intdv s_fd, dv_string content)
    {
        int len = vm.process->heap->get(content);
        char *ctn = STRING::String::dv_str_to_c_srt(vm.process->heap, content);

        return _socket_->_send(s_fd, ctn, len);
    }

    dv_string net_read_s(DvVmEnv vm, intdv s_fd,  intdv len)
    {
        char *buffer = (char *)calloc(len, sizeof(char *));

        int n = _socket_->_read(s_fd, buffer, len);

        intdv adr = c_str_to_dv_srt(vm.process->heap, buffer);

        free(buffer);
        return adr;
    }

    intdv net_read_o(DvVmEnv vm, intdv s_fd,  intdv len)
    {
        char *buffer = (char *)calloc(len, sizeof(char *));

        int n = _socket_->_read(s_fd, buffer, len);

        intdv adr = c_str_to_dv_srt(vm.process->heap, buffer);

        /*Class Read en DvLang
        
            class Read{
                string data;
                int len;
            }
        */
        intdv obj_r = vm.process->heap->Malloc(3);
        vm.process->heap->heap[obj_r] = 2; //num of fields
        vm.process->heap->heap[obj_r+1] = adr;
        vm.process->heap->heap[obj_r+2] = n;


        free(buffer);
        return obj_r;
    }

    /**
     * net_read, lee de la conexin socket (s_fd)
     * al igual q en C/C++ se le pasa un puntero (dv_str_pointer)
     * para almacenar la informacion leida del socket, ese puntero
     * es la direccion de la variable en DvLang a donde se vaciaran los datos
    */
    intdv net_read(DvVmEnv vm, intdv s_fd, intdv dv_str_pointer, intdv len)
    {
        char *buffer = (char *)calloc(len, sizeof(char *));

        int n = _socket_->_read(s_fd, buffer, len);

        intdv adr = c_str_to_dv_srt(vm.process->heap, buffer);

        //al puntero se le asigna la direccion
        //en memoria de cadena creada por lectura
        vm.process->heap->heap[dv_str_pointer] = adr;

        free(buffer);
        return n;
    }

    intdv net_close_socket(DvVmEnv vm, intdv s_fd)
    {
        return _socket_->_close(s_fd);
    }
}
