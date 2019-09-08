
#ifndef HEAP_H
#define HEAP_H

#include "../types.h"
#include <iostream>
#include <stdlib.h>
//#include <iterator>
//#include <array>
//#include <inttypes.h>
#include <cstdint> 

using namespace std;

namespace Heap {

    class Freedon {
    public:
        Freedon() {};
        intdv from;
        intdv to;
        int len;

        string toString() {
            string ts;
            #ifdef _WIN32 
            #elif __linux__
                // linux
                ts += "from: ";
                ts += to_string(from);
                ts += ", to: ";
                ts += to_string(to);
                ts += ", len: ";
                ts += to_string(len);
            #elif __unix__ // all unices, not all compilers
                // Unix
            #elif __APPLE__
                // Mac OS, not sure if this is covered by __posix__ and/or __unix__ though...
            #endif
            return ts;
        }
    };

    class Freedons {
    public:
        unsigned int _size;
        unsigned int size = 0;
        intdv lastIndex = 0;
        Freedon **freedons;

        Freedon *get(int index) {

            if (index < _size && freedons[index] != NULL) {

                return freedons[index];
            }
            //cout << "NULL: "<<index << endl;
            return NULL;
        }

        Freedon *set(unsigned int index, Freedon *f) {
            if (freedons[index] == NULL) {
                size++;
            }
            if (index > _size - 1) {
            #if DEBUG
                cout << "resizing" << endl;
            #endif
                _size = index + 1;
                freedons = (Freedon **) realloc(freedons, (_size) * sizeof(Freedon));
            }
            lastIndex = index;
            freedons[index] = f;
            return freedons[index];
        }

        void remove(int index) {
            if (freedons[index] != NULL) {
                size--;
            }
            freedons[index] = NULL;
        }

        Freedons(unsigned int s) {
            _size = s;
            freedons = (Freedon **) calloc(_size, sizeof(Freedon *));
        }
    };

    class Heap {
    public:
        Heap() {};

        Heap(intdv);

        ~Heap();

        intdv get(intdv);

        void put(intdv, intdv);

        /**
         * lugar donde se almacena los datos (memoria)
         * */
        intdv *heap;

        /**
         * apuntador de la parte frontal de la memoria (memoria libre)
         * */
        intdv heap_pointer = 0;

        /**
         * cambia el temaño de memmoria
         * asignada a una direccion
         * */
        intdv Realloc(intdv adr, intdv newSize);

        /**
         * reserva size espacio de memoria y los rellena a 0
         *
         * */
        intdv Calloc(intdv size);

        /**
         * reserva size espacio de memoria
         * */
        intdv Malloc(intdv size);

        /**
         * busca en memoria libre una area de size espacios
         * */
        Freedon *SearchInFree(intdv size);

        /**
         * libera la direccion de momemoria adr
         * y ese espacio liberado lo registra
         *
         * cada vez q se libera un espacio  en la memoria
         * tiene q hacer una serie de pasos para
         * juntar los espacios libres delado de la direccion
         * a liberar actual, para crear bloques de memoria libres
         * grandes y no dejar fragmentos pequeños
         * */
        void Free(intdv adr);


        /**
         * busca espacio de momoria libre por adr
         * */
        intdv FindByFrom(intdv adr);

        /**
         * rellena un espacio de memoria a 0
         * */
        intdv Fill(intdv adr, intdv fillWith);
        
        /**
         * TODO
         * */
        intdv HeapFree();

        Freedons *freedons;

    private:
        /* data */
        //Freedon *freedons;
        intdv MAX_HEAP;
    };

} // namespace Heap

#endif