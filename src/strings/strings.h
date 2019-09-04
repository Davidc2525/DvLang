#ifndef STRING_H
#define STRING_H
#include "../types.h"
#include <iostream>
#include "../heap/heap.h"

using namespace std;
using namespace Heap;

  //TODO api para librerias 
  //extern "C" char *dv_str_to_c_srt(Heap::Heap *heap, intdv adr);
  //extern "C" intdv c_str_to_dv_srt(Heap::Heap *heap, char *str);
  
namespace STRING
{

class String
{
  public:
    /**
     * Convierte una cadena de DvLang a un char* de C/C++.
     * Se debe liberar manualmente la cadena char* devuelta
    */
    static char *dv_str_to_c_srt(Heap::Heap *heap, intdv adr);

    /**
     * Convierte un char* C/C++ a una cadena DvLang
     * y devuelve la direccion de la cadena en la memoria virtual de DvVm
    */
    static intdv c_str_to_dv_srt(Heap::Heap *heap, char *str);
    
    
    /**
     * Convierte un const char* C/C++ a una cadena DvLang
     * y devuelve la direccion de la cadena en la memoria virtual de DvVm
    */
    static intdv c_str_to_dv_srt(Heap::Heap *heap, const char *str);

    /**
     * Convierte un std::string C++ a una cadena DvLang
     * y devuelve la direccion de la cadena en la memoria virtual de DvVm
    */
    static intdv c_str_to_dv_srt(Heap::Heap *heap, string str);
};

} // namespace STRING

#endif