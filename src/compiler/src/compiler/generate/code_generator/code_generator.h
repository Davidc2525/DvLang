

#if !defined(DVLANG_CODE_GENERATOR_H__)
#define DVLANG_CODE_GENERATOR_H__
#include <process/Instructions.h>
#include <types.h>
#include "../symbol_table/symbol_table.h"
#include <program/program.h>
#include <process/strings_sets.h>


using namespace STRING_SET;

namespace DVLANG
{
    namespace COMPILER_
    {
        namespace GENERATE{

            class Operand {
                public:
                static const int 
                    Con = 0, 
                    Local = 1, 
                    Static = 2, 
                    Stack = 3, 
                    Fld = 4, 
                    Elem = 5, 
                    Meth = 6,
                    Func = 7;
                
                static const int 
                    OneExpr = 0,
                    RelaExpr = 1;
                
                int kind; // Con, Local, Static, ...
                Struct * type; // type of the operand
                int val; // Con: constant value
                int adr; // Local, Static, Fld, Meth: address
                Obj * obj; // Meth: method object
                int co_call;

                //expresion
                int fromExprType;
                int relaExprSub;
                int relaExprOp;
                //expresion

                Operand (Obj * obj);

                Operand (int val);
            };

            class CodeGenerator{
                public:
                    CodeGenerator(){
                        init();
                    };
                    static Strings * string_set; 
                    static intdv * program;
                    static intdv main;
                    static intdv pc;
                    static int globals;

                    static void put  (intdv x); 
                    static void put2 (intdv x);
                    static void put4 (intdv x);

                    static void assign(Operand * x, Operand * y);

                    static void fixup(intdv adr);

                    static void load (Operand * x);
                    static void store (Operand * x);

                    static void init();
                    static void write(string filename,LibSymbolStore* libs);
            };
        }
    }
} // DVLANG



#endif
