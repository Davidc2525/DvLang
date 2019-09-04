#if !defined(ARRAY_SIZE)
#define ARRAY_SIZE(x) (sizeof((x)) / sizeof((x)[0]))
#endif
#include "../process/strings_sets.h"
#include "../proto/objFile.pb.h"
#include "program.h"

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <cstring>

#include <rapidjson/prettywriter.h>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

#include "rapidjson/filewritestream.h"
#include <rapidjson/writer.h>
#include <cstdio>
using namespace std;
using namespace rapidjson;
using namespace PROGRAM; 


/**
 * lee el archivo del programa y crea
 * un int* q contiene las instruciones
 * que ejecutara el CPU

PROGRAM::DataProgram *PROGRAM::readProgram(string fileName)
{
    fstream input(fileName, ios::in | ios::binary);

    DvObjecFile::Program programObj;
    if (programObj.ParseFromIstream(&input))
    {

        PROGRAM::DataProgram *args = new PROGRAM::DataProgram();
        args->inst = new intdv[programObj.size()];
#if DEBUG
        cout << "Firma: " << programObj.signature() << endl;
        cout << "Tamaño del programa: " << programObj.size() << endl;
#endif
        //programReturn.inst = new int[programObj.size()];
        DvObjecFile::Code p = programObj.code();
#if DEBUG
        cout << "code: " << endl;
#endif
        for (int x = 0; x < programObj.size(); x++)
        {
#if DEBUG
            cout << "line: " << x + 1 << ". " << p.inst(x) << endl;
#endif
            args->inst[x] = p.inst(x);
        }

        args->size = programObj.size();
        args->main = programObj.mainp();

        return args;
    }
    else
    {

        cout << "Error al leer el programa" << endl;
        exit(1);
    }
    //return program;
}*/

/** Esta funcion sera eliminada, el creador del archivo dvc, lo escribe el compilador
void PROGRAM::writeProgram(string filename, int main, int size, int *program)
{
    //int programSize = 55; //sizeof(program)/sizeof(int);
    cout << "size program " << size << endl;
    DvObjecFile::Program programObj;
    programObj.set_signature("DVVM");
    programObj.set_size(size);
    programObj.set_mainp(main);
    DvObjecFile::Code *code = programObj.mutable_code();
    for (int x = 0; x < size; x++)
    {
        code->add_inst(program[x]);
    }

    fstream output(filename, ios::out | ios::trunc | ios::binary);
    programObj.SerializeToOstream(&output);
} */


/**
 * Cargar programa en formato JSON
*/
DataProgram *PROGRAM::readProgramJson(string fileName)
{
    PROGRAM::DataProgram *args = new PROGRAM::DataProgram();
    FILE *fp = fopen(fileName.c_str(), "rb"); // non-Windows use "r"

    char readBuffer[65536];
    FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    Document d;
    d.ParseStream(is);

    args->main = d["main"].GetInt();
    args->size = d["size"].GetInt();
    args->globals = d["globals"].GetInt();
    args->inst = new intdv[args->size];
    args->entrys = (STRING_SET::Entry **) calloc(1000,sizeof(STRING_SET::Entry));

    const Value &code = d["code"];
    for (SizeType i = 0; i < code.Size(); i++)
    {
        #if cpu_arch == 64
            intdv inst = code[i].GetInt64();
        #else
            intdv inst = code[i].GetInt();
        #endif
#if DEBUG
        if (i == args->main)
        {
            cout << "line: " << i << ". " << inst << " MAIN " << endl;
        }
        else
        {
            cout << "line: " << i << ". " << inst << endl;
        }
#endif
        args->inst[i] = inst;   
    }
    
    #if DEBUG
        cout << "Loading constants strings. " << endl;
    #endif

    if(d.HasMember("strings")){
        const Value &strings = d["strings"];

        for(SizeType i = 0; i< strings.Size();i++){
            string v = strings[i].GetString();
            args->entrys[i] = new STRING_SET::Entry(v,v.length());
            #if DEBUG
                cout << "Add string constant: " << strings[i].GetString() << endl;
            #endif
        }
    }

    if(d.HasMember("libs_symbols"))
    {
        PROCESS::LibLoad *libload = new PROCESS::LibLoad;
        args->libload = libload;

        const Value &libs = d["libs_symbols"];

        if (!libs.IsArray())
        {
            cout << "mal formato: libs_symbols debe ser una lista" << endl;
            exit(1);
        }

        for (SizeType l = 0; l < libs.Size(); l++)
        {
            const char *lib_name = libs[l]["lib_name"].GetString();
            PROCESS::Lib *lib = new PROCESS::Lib((char *)lib_name);
            libload->load(lib);

            const Value &symbols = libs[l]["symbols"];

            if (!symbols.IsArray())
            {
                cout << "mal formato: symbols debe ser una lista" << endl;
                exit(1);
            }

            for (SizeType s = 0; s < symbols.Size(); s++)
            {
                const char *symbol_name = symbols[s]["name"].GetString();
                int symbol_argc = symbols[s]["argc"].GetInt();

                //cout << "lib_symbol: " << lib_name << " symbol: " << symbol_name << endl;

                PROCESS::MetaFunc *meta_func = new PROCESS::MetaFunc;
                meta_func->argc = symbol_argc;
                meta_func->name = (char *)symbol_name;
                meta_func->lib_name = lib->name;
                libload->load_func(meta_func);
            }
        }
    }

    if(false&&d.HasMember("libs"))//borrar
    {
        PROCESS::LibLoad * libload = new PROCESS::LibLoad;
        args->libload = libload;

        const Value &libs = d["libs"];

        for (SizeType l = 0; l < libs.Size(); l++)
        {
            const Value &lib = libs[l];

            rapidjson::Value::ConstMemberIterator M;
            const char *lib_name;
            for (M = lib.MemberBegin(); M != lib.MemberEnd(); M++)
            {

                lib_name = M->name.GetString();

                //cout << "lib:" << lib_name << " " << endl;
                PROCESS::Lib * lib = new PROCESS::Lib((char*)lib_name);
                libload->load(lib);

                for (SizeType x = 0; x < M->value.Size(); x++)
                {

                    rapidjson::Value::ConstMemberIterator MF;
                    const char *func_name;
                    for (MF = M->value[x].MemberBegin(); MF != M->value[x].MemberEnd(); MF++)
                    {
                        
                        const char *func_name = MF->name.GetString();
                        const Value &func = MF->value;

                        PROCESS::MetaFunc *meta_func = new PROCESS::MetaFunc;
                        meta_func->argc = func["argc"].GetInt64();
                        meta_func->name =  (char*)func_name;
                        meta_func->lib_name = lib->name;
                        libload->load_func(meta_func);

                        //cout << " func: " << func_name << " " << func["argc"].GetInt64() << endl;
                    }
                }
            }
        }
    }

#if DEBUG
    cout << "CPU ARCH: " << cpu_arch << endl;
    cout << "Firma: " << d["signature"].GetString() << endl;
    cout << "MAIN: " << args->main << endl;
    cout << "Tamaño del programa: " << args->size << endl;
#endif
    //fclose(fp);
    return args;
}

void PROGRAM::writeProgramJson(
    string filename, 
    int main, 
    int size, 
    int globals,
    intdv *program,   
    STRING_SET::Strings * string_set, 
    LibInfo* libs
){
    //int programSize = 55; //sizeof(program)/sizeof(int);
    cout << "FILE " << filename << endl;
    cout << "MAIN " << main << endl;
    cout << "SIZE " << size << endl;

    const char *json = "{\"signature\":\"\", \"main\":0,\"size\":0,\"globals\":0,\"strings\":[],\"libs_symbols\":[],\"code\":[]}";
    Document d;
    
    d.Parse(json);
    
    d["signature"].SetString("DVVM");
    d["main"].SetInt(main);
    d["size"].SetInt(size);
    d["globals"].SetInt(globals);

    
    Value &strings = d["strings"];    
    if(string_set!=NULL){

        for (int x = 0; x < string_set->length(); x++)
        {
            STRING_SET::Entry e = string_set->get(x); 
            #if DEBUG
                cout << "Inserting literal string: "<<e.content << endl;
            #endif
            Value nc;
            nc.SetString(e.content.c_str(),e.len,d.GetAllocator()); 
           
            strings.PushBack(nc,d.GetAllocator());
        }

    }

    Value &code = d["code"];    
    cout<<"size: "<<code.Size()<<endl;
    for (SizeType x = 0; x < size; x++)
    {
        Value nc;
        nc.SetInt64(program[x]);
        code.PushBack(nc,d.GetAllocator());
    }
    
    Value &libs_s = d["libs_symbols"];    
    cout<<"libs: "<<libs->count<<endl;
    string last_lib("");

    
    
    Value Symbol;
    Symbol.SetObject();
    Value * l_s = &Symbol;

    Value Symbols;
    Symbols.SetArray();
    Value * l_s_s = &Symbols;

    SizeType
     index = -1;
    for (SizeType x = 0; x < libs->count; x++)
    {//group
        
        SymbInfo *l = libs->libs[x];
        
        //cout<<"add lib to output "<<l->lib_name<<" "<<l->symb_name<<endl;

        if(last_lib != l->lib_name){
            last_lib = string(l->lib_name);

            cout<<" nueva lib: "<<last_lib<<endl;
            cout<<"  ["<<x<<"] 1 add symbol, lib: "<<last_lib<<" s: "<<l->symb_name<<endl;

            Value l_n;
            l_n.SetString(last_lib.c_str(),strlen(last_lib.c_str()),d.GetAllocator());
          
            l_s->SetObject();

            l_s->AddMember("lib_name",l_n,d.GetAllocator());
                      
           
            Value Symbols;
            Symbols.SetArray();
            l_s_s = &Symbols;
           

            Value key;//key
            key.SetString(l->symb_name.c_str(),strlen(l->symb_name.c_str()),d.GetAllocator());
          
            Value value;//value
            value.SetInt(l->argc);
          
            Value symbol;
            symbol.SetObject();
            symbol.AddMember("name",key,d.GetAllocator());
            symbol.AddMember("argc",value,d.GetAllocator());

            l_s_s->SetArray();
            l_s_s->PushBack(symbol,d.GetAllocator());
            
            l_s->AddMember("symbols",*l_s_s,d.GetAllocator());

            libs_s.PushBack(*l_s,d.GetAllocator());
            index++;
        }else{

            cout<<"  ["<<x<<"] 2 add symbol, lib: "<<last_lib<<" s: "<<l->symb_name<<endl;
           
            Value key;//key
            key.SetString(l->symb_name.c_str(),strlen(l->symb_name.c_str()),d.GetAllocator());
          
            Value value;//value
            value.SetInt(l->argc);
          
            Value symbol;
            symbol.SetObject();
            symbol.AddMember("name",key,d.GetAllocator());
            symbol.AddMember("argc",value,d.GetAllocator());

            Value &nc = libs_s[index]["symbols"];
            nc.PushBack(symbol,d.GetAllocator());
            //l_s_s->SetArray();
            //l_s_s->PushBack(symbol,d.GetAllocator());
        }
    
    }
   
    FILE *fp = fopen(filename.c_str(), "wb"); // non-Windows use "w"
    char writeBuffer[65536];
    FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
    
    #if DEBUG
        PrettyWriter<FileWriteStream> w(os);//dev    
    #endif

    #if !DEBUG
       Writer<FileWriteStream> w(os);// procution  
    #endif
    
    //
    d.Accept(w);
    fclose(fp);
}
