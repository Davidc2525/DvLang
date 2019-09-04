#include "process.h"
#include "DvVmEnv.h"
namespace PROCESS
{

/**
 * Lib, carga una libreria
*/
Lib::Lib(char *lib_name)
{
    name = lib_name;
    funs = (MetaFunc **)calloc(1000, sizeof(MetaFunc *));
    //cache_funs = (ffi_cif **)calloc(100, sizeof(ffi_cif *));
}

/**
 * Busca y carga la funcion en info->name
 * si existe se carga y se almacena dentro de info->func
 * para usarse luego en Lib::call
*/
void Lib::load_func(int index_abs,MetaFunc *info)
{
    info->func = dlsym(handle, info->name);
    char *error;

    if ((error = dlerror()) != NULL)
    {
        cout << "Error al cargar func: " << info->name << ", error: " << error << endl;
        exit(1);
    } 

    funs[index_abs] = info;
    //index++;
}
/**
 * llamar funcion nativa dentro de una libreria
 * id_func es la id de la funcion a llamar
 * cpu es el cpu el cual ejecuta la llamada, para tener
 * acceso a la pila.
*/
intdv Lib::call(int id_func, PROCESS::Cpu *cpu)
{

    MetaFunc *mf = funs[id_func];
    int argc = mf->argc + 1;
    DvVmEnv native_vm_arg = {cpu->process, cpu->cpu_index};

    intdv ret;

    ffi_type native_vm_type;
    ffi_type *native_vm_type_elements[3];

    /* construccion de tipo para
        DvVmEnv, se le pasa como 
        primer argumento a todas las funciones
        q se llaman
     */
    native_vm_type.size = 0;
    native_vm_type.alignment = 0;
    native_vm_type.type = FFI_TYPE_STRUCT;
    native_vm_type.elements = native_vm_type_elements;
    native_vm_type_elements[0] = &ffi_type_pointer;
    native_vm_type_elements[1] = &ffi_type_sint;
    native_vm_type_elements[2] = NULL;

    ffi_cif cif;
    ffi_type *args[argc];

    void *values[argc];
    intdv v[argc];

    //llenar tipos argumentos
    for (int x = 0; x < argc; x++)
    {
        args[x] = &ffi_type_slong;
    }

    args[0] = &native_vm_type;
    values[0] = &native_vm_arg;

    //llenar valores de argumentos
    for (int x = argc - 1; x >= 1; x--)
    {
        v[x] = cpu->estack->Pop();
        values[x] = &v[x];
#if DEBUG
        cout << "x: " << x << ", VAL: " << v[x] << endl;
#endif
    }

    //preparar funcion a llamar
    ffi_status status = ffi_prep_cif(&cif, FFI_DEFAULT_ABI, argc, &ffi_type_slong, args);
    if (status == FFI_OK)
    {
#if DEBUG
        cout << "Llamando func: " << mf->name << " lib: " << mf->lib_name << endl;
#endif
        //llamar funcion
        ffi_call(&cif, FFI_FN(mf->func), &ret, values);
        return ret;
    }
    else
    {
        cout << "ffi_prep_cif failed:" << status << endl;
    }
}

//-----------------------------------------
/**
 * LibLoad lleva el registro de las librerias y funciones nativas
*/
LibLoad::LibLoad()
{
#if DEBUG
    cout << "LibLoad" << endl;
#endif
    funs = (Lib **)calloc(1000, sizeof(Lib *));
}

/**
 * carga una nueva libreria
*/
void LibLoad::load(Lib *lib)
{
    lib->handle = dlopen(lib->name, RTLD_NOW | RTLD_GLOBAL);
    if (!lib->handle)
    {
        cout << "Error al cargar libreria: " << dlerror() << endl;
    }

    libs.insert(std::make_pair(lib->name, lib));
#if DEBUG
    cout << "Registrada libreria: " << lib->name << endl;
#endif
}

/**
 * carga una nueva funcion desde info->lib_name
*/
void LibLoad::load_func(MetaFunc *info)
{
    if (libs.find(info->lib_name) == libs.end())
    {
        cout << "no existe libreria: " << info->lib_name << " para cargar func: " << info->name << endl;
        return;
    }
    Lib *lib = libs.at(info->lib_name);

    lib->load_func(index,info);

    //fuardo el id por nombre, para obtener el id de la
    //funcion en tiempo de compilacion
    names.insert(std::make_pair(info->name, index));

    funs[index] = lib;


#if DEBUG
    cout << " Registrado symbolo: ["<<index<<"] " << info->name << " de libreria: " << info->lib_name << endl;
#endif
    index++;
}

/**
 * llama a Lib::call
*/
intdv LibLoad::call(int id_func, PROCESS::Cpu *cpu)
{
#if DEBUG
    cout << "LibLoad call: " << id_func << endl;
#endif
    Lib *lib = funs[id_func];
    if (lib == NULL)
    {
        cout << "Funcion con id: " << id_func << ", no esta registrada." << endl;
        return 0;
    }
    return lib->call(id_func, cpu);
}

/**
 * obtener id de una funcion por su nombre
*/
int LibLoad::get_id_func(char *func_name)
{

    if (names.find(func_name) == names.end())
    {
        return -1;
    }

    return names.at(func_name);
}

} // namespace PROCESS