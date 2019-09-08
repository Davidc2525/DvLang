# DvLang :tada: :confetti_ball:
Lenguaje de programacion DvLang

# Requerimientos

## Compilacion
- g++
- make
- coco-cpp

## Librerias
- libffi
- rapidjson

## En adicion a los requerimientos anteriores, compilacion para Windows
- mingw-w64
- [mingw-std-threads](https://github.com/meganz/mingw-std-threads) *solo es necesario cuando la compilacion se lleva a cabo en versiones muy antiguas de Windows* 


# Descarga de codigo fuente y compilacion
Luego de instalar los requerimientos, procedemos con la compilacion del codigo fuente.

```bash
$ git clone https://github.com/Davidc2525/DvLang.git
$ cd DvLang
$ mkdir out
$ make 
$ cd src/compiler
$ make
$ cd ../../
```

Si la compilacion se lleva a cabo en windows


```bash
$ git clone https://github.com/Davidc2525/DvLang.git
$ cd DvLang
$ mkdir out
$ make -f Makefile.win
$ cd src/compiler
$ make
$ cd ../../
```

__Nota:__ *make -f Makefile.win* se use igual para las otras partes donde se compila parte del codigo.

__Nota:__ En __.out/__  se encuentran los binarios para linux y windows, de igual forma necesita tener las librerias requeridas instaladas o disponibles en el sistema.


# Programando en DvLang 


Luego de compilar es hora de programar en DvLang


```c
    //test/dvlang.dv
    package main
    {   
        /*
        * Sumar dos numeros
        * */
        int sumar(int a, int b){
            return a + b;
        }
        
        void main(){
            print(sumar(20,5));
        }
    }

```
Luego compilamos el codigo fuente y ejecutamos

```bash
$ out/Dvc test/dvlang.dv -o out.dvc.json
``` 
Se muestra informacion sobre la compilacion 

```bash
...
INST [16] : 5
INST [17] : 0
INST [18] : 0
INST [19] : 41
INST [20] : 7
INST [21] : 27
INST [22] : 28
INST [23] : 29
FILE out.dvc.json
MAIN 10
SIZE 24
libs: 0
compilado.
```

Luego de la compilacion, ejecutamos:

```bash
$ out/DvVm out.dvc.json
``` 
La salida de la ejecucion es la suma de los operandos en el codigo

```bash
25
``` 

# Interoperabilidad
La API de DvLang te permite ejecutar funciones nativas escritas en C/C++ 

Vamos a escribir y usar una libreria de C++

archivo: __/src/libs_std/std/std.cpp__ *C++*

```c++
...
    int println(DvVmEnv vm, dv_string str)
    {
        char *c_str = STRING::String::dv_str_to_c_srt(vm.process->heap, (str));
        int size = vm.process->heap->get(str);
        for (int x = 0; x < size; x++)
        {
            cout << c_str[x];
        }
        cout << endl;
       
        free(c_str);
        return size;
    }
...
```

Descritas las funciones nativas que queremos usar de lado de DvLang, compilamos.

__Nota:__ Todas las funciones descritas en C/C++ para usar como funciones nativas en DvLang, deben tener como primer parametro __DvVmEnv vm__ que es una instancia del entorno de ejecucion, es auntomaticamente pasado cuando se llama a dicha funcion desde __DvLang__, los parametros que quieres usar, se describen luego del __vm__, como en este ejemplo, la funcion debe imprimir una cadena, la recibe como segundo parametro. Mas adelante se muestra un ejemplo.

En la rruta __src/libs_std/std/__, hay archivos que sirven como plantilla para crear otras librerias de funciones nativas.

Dentro de __src/libs_std/std/__ ejecutamos:

```bash
$ cd src/libs_std/std
$ make clear compile
$ cd ../../../
``` 

Eso nos deja una libreria __DvLang/src/libs_std/std/libstd.so__ que usaremos en un archivo fuente de DvLang

archivo:  __DvLang/test/std.dv__ *DvLang*

```c
package std

@load_lib "--tu_home--/DvLangVM/src/libs_std/std/libstd.so";

{
    ...
    /**
    * Native println, imprime str y al final coloca un salto de linea
    */
    native int println(string str);
    ...
}

```

Se tiene que usar la sentencia: __@load_lib "path.so"__ con la rruta de la libreia compilada para bincular con la declaracion dentro de DvLang, El compilador hace el resto.

__Nota:__ Como se menciono anteriormente, en esta parte de la declaracion de la funcion nativa, solo se describe un solo parametro, ques la cadena que se quiere imprimir.

Luego hacemos uso de las funciones nativas creadas.

archivo: __DvLang/test/dvlang_native.dv__ *DvLang*

```c
    package main
    @include "test/std.dv";
    {   
        string hola(string mundo){
            return "hola " + mundo;
        }
        void main(){
            
            println(hola("mundo c:"));
        }
    }

```

Compilamos el archivo fuente

```bash
$ out/Dvc test/dvlang_native.dv -o out.dvc.json
```
Se muestra informacino sobre la compilacion.

```bash
...
INST [101] : 39
INST [102] : 1
INST [103] : 0
INST [104] : 88
INST [105] : 41
INST [106] : 0
INST [107] : 1
INST [108] : 31
INST [109] : 27
INST [110] : 28
INST [111] : 29
FILE out.dvc.json
MAIN 98
SIZE 112
libs: 10
 nueva lib: /home/david/DvLangVM/src/libs_std/std/libstd.so
  [0] 1 add symbol, lib: /home/david/DvLangVM/src/libs_std/std/libstd.so s: std_itos
  [1] 2 add symbol, lib: /home/david/DvLangVM/src/libs_std/std/libstd.so s: println
  [2] 2 add symbol, lib: /home/david/DvLangVM/src/libs_std/std/libstd.so s: println_ref
  [3] 2 add symbol, lib: /home/david/DvLangVM/src/libs_std/std/libstd.so s: strlen
  [4] 2 add symbol, lib: /home/david/DvLangVM/src/libs_std/std/libstd.so s: std_u_sleep
  [5] 2 add symbol, lib: /home/david/DvLangVM/src/libs_std/std/libstd.so s: std_sleep
  [6] 2 add symbol, lib: /home/david/DvLangVM/src/libs_std/std/libstd.so s: std_malloc
  [7] 2 add symbol, lib: /home/david/DvLangVM/src/libs_std/std/libstd.so s: std_calloc
  [8] 2 add symbol, lib: /home/david/DvLangVM/src/libs_std/std/libstd.so s: std_realloc
  [9] 2 add symbol, lib: /home/david/DvLangVM/src/libs_std/std/libstd.so s: std_free
compilado.
```


Ahora ejecutamos.

```bash
$ out/DvVm out.dvc.json
```
Apreciemos su salida:

```bash
hola mundo c:
```

# Clases
En __DvLang__ se puede definir clases, veamos como se hace. Archivo:  __DvLang/test/clase1.dv__ *DvLang*

```java
...
class Usuario {

    string nombre;
    int anios;

    Usuario construct(string n,int a){
        this.nombre = n;
        this.anios = a;
        return this;
    }
}

{

    void main(){
        Usuario n1;

        u1 = new Usuario("Lola",25);

        println(u1.nombre+", tiene "+u1.anios+" años.");
        //se recomienda usar -> std_itos(u1.anios)
        //para concatenar numero con cadena 
    }

}
...
```


Compilamos y ejecutamos

```bash
$ out/Dvc test/dvlang_native.dv -o out.dvc.json
$ out/DvVm out.dvc.json
```

Apreciemos su salida:

```bash
Lola, tiene 25 años.
```
__Nota:__ Es recomendable usar __*std_itos*__ para cuando se quiera imprimir un numero por terminal.


# Arrays
No pueden faltar las listas o arrays, en __DvLang__ se pueden crear lista de elementos y/o lista de objetos de un tipo de clase.

Archivo: __DvLang/test/array1.dv__ *DvLang*


```java

package main

@include "test/std.dv";

class Usuario {

    string nombre;
    int anios;

    Usuario construct(string n,int a){
        this.nombre = n;
        this.anios = a;
        return this;
    }
}

{

    void main(){
        int cantidad,x;
        Usuario u;
        Usuario [] users;

        cantidad = 100;
        users = new Usuario[cantidad];

        x = 0;
        while (x < cantidad) {
            users[x] = new Usuario("usuario_"+std_itos(x),x);
            x = x + 1;
        }
      
      
        x = 0;
        while (x < cantidad) {
            u = users[x];
            println(u.nombre+", tiene "+std_itos(u.anios)+" años.");
            x = x + 1;
        }
    }

}
```
Compilamos y ejecutamos

```bash
$ out/Dvc test/array1.dv -o out.dvc.json
$ out/DvVm out.dvc.json
```

Apreciemos su salida:

```bash
...
INST [206] : 163
INST [207] : 27
INST [208] : 28
INST [209] : 29
FILE out.dvc.json
MAIN 107
SIZE 210
libs: 10
 nueva lib: /home/david/DvLangVM/src/libs_std/std/libstd.so
  [0] 1 add symbol, lib: /home/david/DvLangVM/src/libs_std/std/libstd.so s: std_itos
  [1] 2 add symbol, lib: /home/david/DvLangVM/src/libs_std/std/libstd.so s: println
  [2] 2 add symbol, lib: /home/david/DvLangVM/src/libs_std/std/libstd.so s: println_ref
  [3] 2 add symbol, lib: /home/david/DvLangVM/src/libs_std/std/libstd.so s: strlen
  [4] 2 add symbol, lib: /home/david/DvLangVM/src/libs_std/std/libstd.so s: std_u_sleep
  [5] 2 add symbol, lib: /home/david/DvLangVM/src/libs_std/std/libstd.so s: std_sleep
  [6] 2 add symbol, lib: /home/david/DvLangVM/src/libs_std/std/libstd.so s: std_malloc
  [7] 2 add symbol, lib: /home/david/DvLangVM/src/libs_std/std/libstd.so s: std_calloc
  [8] 2 add symbol, lib: /home/david/DvLangVM/src/libs_std/std/libstd.so s: std_realloc
  [9] 2 add symbol, lib: /home/david/DvLangVM/src/libs_std/std/libstd.so s: std_free
compilado.


usuario_0, tiene 0 años.
usuario_1, tiene 1 años.
usuario_2, tiene 2 años.
usuario_3, tiene 3 años.
usuario_4, tiene 4 años.
usuario_5, tiene 5 años.
usuario_6, tiene 6 años.

...
```
# Control de flujo

__DvLang__ cuenta con estructuras de control: *while*, *if*, *if_else*, veamos un ejemplo
Archivo: __DvLang/test/dvlang_flow.dv__ *DvLang*


```java
package main
@include "test/std.dv";
{   
    void main(){
        int x;

        x = 25;

        println("Primera comparacion");

        if x == 25
            println("x es igual a 25");
        


        x = 26;

        println("Segunda comparacion");

        if x == 25 
            println("x es igual a 25");
        else
            println("x es diferente a 25");
        
       
       
        x = 27;

        println("Tercera comparacion");

        if x == 25 {
            println("x es igual a 25");
        } else {
            println("x es diferente a 25");
        }

        //Bucles
        
        println("---------- Incrementar x ----------");

        x = 1;
        while x <= 10 {
            println("Valor de x: "+std_itos(x));
            x = x + 1;
        }

        println("---------- Decrementar x ----------");

        x = 10;
        while x >= 1 {
            println("Valor de x: "+std_itos(x));
            x = x - 1;
        }
    }
}
```

Compilamos y ejecutamos

```bash
$ out/Dvc test/dvlang_flow.dv -o out.dvc.json
$ out/DvVm out.dvc.json
```

Apreciemos su salida:

```bash
Primera comparacion
x es igual a 25
Segunda comparacion
x es diferente a 25
Tercera comparacion
x es diferente a 25
---------- Incrementar x ----------
Valor de x: 1
Valor de x: 2
Valor de x: 3
Valor de x: 4
Valor de x: 5
Valor de x: 6
Valor de x: 7
Valor de x: 8
Valor de x: 9
Valor de x: 10
---------- Decrementar x ----------
Valor de x: 10
Valor de x: 9
Valor de x: 8
Valor de x: 7
Valor de x: 6
Valor de x: 5
Valor de x: 4
Valor de x: 3
Valor de x: 2
Valor de x: 1
```
Los "__(__"  "__)__" son opcionales en las sentencias *if* y *while*. Los "__{__"  "__}__" en estas estructuras tambien son opcionales, a menos que se ejecuten mas de una lineas de codigo.  
```c
    // sin ( ) y { }
    if x == 1 print(x);
    
    // con {}, por que hay mas de una linea

    if( x==1 ){ 
        print(x);
        print(x+1);
    }
```

# Proxima liberacion **

- Llamadas a funciones como corrutinas

# Contribuye

__DvLang__ aun esta en desarrollo :hammer:, si quieres contribuir puedes contactarme :email:.

Gmail: __David25pcxtreme@gmail.com__ 

Facebook: [Colmenares David](https://m.facebook.com/estoy.aburrido.940)