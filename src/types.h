/*definicion de tipos q se usaran a lo largo del programa*/
#ifndef TYPES_H

    #define TYPES_H

    #ifndef cpu_arch 
        #define cpu_arch 32
    #endif

    #if cpu_arch == 64
        /**
         * Tipo entero.
         * es de 32 bits si -Dcpu_arch es configurado como 32 bits o 64 bits
         * 
         * se usa para los valores de las stacks,heaps y programa
         * el programa a ejcutar sera afectado por la arquitectura q se use
        */
        typedef long long int intdv; 
    #else
        /**
         * Tipo entero.
         * es de 32 bits si -Dcpu_arch es configurado como 32 bits o 64 bits
         * 
         * se usa para los valores de las stacks,heaps y programa
         * el programa a ejcutar sera afectado por la arquitectura q se use
        */
        typedef int intdv; 
    #endif

    /**
     * tipo cadena en DvLang
    */
    typedef intdv dv_string; 
    
    typedef intdv dv_int;
    typedef intdv dv_float;
    typedef intdv dv_double;

#endif