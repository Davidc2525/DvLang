
#include "code_generator.h"

namespace DVLANG
{
    namespace COMPILER_
    {
        namespace GENERATE{
            Operand::Operand (Obj * obj) {
                type = obj->type; val = obj->val; adr = obj->adr;
                switch (obj->kind) {
                    
                    case Obj::Con: kind = Con; this->obj = obj; break;
                    
                    case Obj::Var: this->obj = obj; if (obj->level == 0) kind = Static; else kind = Local;
                    break;
                    
                    case Obj::Meth: kind = Meth; this->obj = obj; break;
                    case Obj::Func: kind = Func; this->obj = obj; break;
                    
                    default: cout<<"cannot create operand"<<endl;
                }
                
            }

            Operand::Operand (int val) {
                kind = Con; type = SymbolTable::intType; this->val = val;
            }
        }
    }
}
