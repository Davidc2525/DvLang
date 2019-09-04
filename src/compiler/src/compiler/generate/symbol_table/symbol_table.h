
#ifndef DVLANG_SYMBOLTABLE_H__
#define DVLANG_SYMBOLTABLE_H__

#include <process/process.h>
#include <iostream>
#include <vector>
#include "type_table.h"
namespace DVLANG
{
    namespace COMPILER_
    {
        namespace GENERATE{
            using namespace std;
            class Obj;
            class Struct;
            class Scope;
            class SymbolTable;

            class Struct{//type
                public:
                static const int
                    None = 0,
                    Int = 1, Float = 2, Double = 3, //numbers
                    String = 4,
                    Char = 11,
                    Arr = 5,
                    Class = 6,
                    Pack = 7, //TODO
                    Bool = 8,
                    Func = 9,
                    Meth = 10;

                
                int kind;           // None, Int, String, Arr, Class
                Struct * elemType = NULL;  // Arr: element type
                int nFields;        // Class: number of fields
                Obj * fields = NULL;       // Class: list of fields

                Struct(int kind);
                Struct(int kind, Struct * elemType);
                Struct(int kind, Struct & elemType);
                bool isRefType();
                bool equals(Struct * other);
                bool compatibleWith(Struct * other);
                bool assignableTo(Struct * dest);
                void assign(Struct * o);
                string toString();
            };

            class Obj{
                public:
                static const int Con = 0, Var = 1, Type = 2, Meth = 3, Func = 4;

                Obj(int kind, string name, Struct * type);
                Obj(){};
                
                int kind;           // Con, Var, Type, Meth, Func
                string name;
                bool is_ref = false;
                Struct * type = NULL;
                Obj *next = NULL;

                /*----------------------------------------------------*/
                bool native = false;//Meth,Func: native
                int  native_id = 0; //Meth,Func: native

               

                int val;            // Con: value

                int adr;            // Var, Meth,Func: address
                int level;          // Var: 0 = global, 1 = local

                int nPars;          // Meth,Func: number of parameters
                Obj * locals;       // Meth,Func: parameters and local objects


                string toString();
            };

            class Scope{
                public:
                Scope *outer;   // to the next outer scope
                Obj *locals;    // to the objects in this scope
                int nVars;      // number of variables in this scope (for address allocation)

                string toString();

            };

            class LibSymbolPair{
                public:
                    string lib_name;
                    string symbol_name;
                    int argc;
            };

            class LibSymbolStore{
                public:
                LibSymbolStore(){};

                void put(string lib_name, string symbol_name,int argc){
                    int index = get_lib(lib_name);
                    if(index == -1){
                        cout<<"Error: "<<lib_name<<" no found";
                        exit(1);
                    }

                    LibSymbolPair * s = new LibSymbolPair;
                    s->lib_name = lib_name;
                    s->symbol_name = symbol_name;
                    s->argc = argc;

                    lib_symbol.push_back(s);
                }

                int get_lib(string lib_name){
                    int index = -1;
                    for(int  x = 0; x < lib_symbol.size();x++){
                        if(lib_symbol[x]->lib_name == lib_name){
                            index = x;
                            break;
                        }
                    }
                }
                int get_symbl(string symbol_name){
                    int index = -1;
                    for(int  x = 0; x < lib_symbol.size();x++){
                        if(lib_symbol[x]->symbol_name == symbol_name){
                            index = x;
                            break;
                        }
                    }
                    return index;
                }

                void dump(){
                    for(int  x = 0; x < lib_symbol.size();x++){
                       cout<<x<<" "<<lib_symbol[x]->lib_name<<" "<<lib_symbol[x]->symbol_name<<" "<<lib_symbol[x]->argc<<endl;
                    }
                }

                vector<LibSymbolPair*> lib_symbol;
            };

            class SymbolTable{
                public:
                static int globals;
                SymbolTable();
                LibSymbolStore * lib_symbols = new LibSymbolStore;
                //static PROCESS::Native * native; // current top scope
                static Scope * curScope; // current top scope
                static int curLevel; // nesting level of current scope
                static Struct * intType; // predefined types
                static Struct * charType; // predefined types
                static Struct * boolType;
                static Struct * floatType;
                static Struct * doubleType;
                static Struct * stringType;
                static Struct * nullType;
                static Struct * noType;
                static Obj * chrObj; // predefined objects
                static Obj * ordObj;
                static Obj * lenObj;
                static Obj * noObj;

                static Obj * insert (int kind, string name, Struct * type);
                static Obj * find (string name);
                static Obj * findField (string name, Struct * type);
                static void openScope();
                static void closeScope();
                static void init();
            };
        }
    }
} // DVLANG



#endif
