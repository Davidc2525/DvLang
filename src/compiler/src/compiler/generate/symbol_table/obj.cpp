
#include "symbol_table.h"

namespace DVLANG
{
    namespace COMPILER_
    {
        namespace GENERATE
        {
                Obj::Obj(int kind, string name, Struct * type) {
                    this->kind = kind; this->name = name; this->type = type;
                }

               

                string Obj::toString(){
                    if(this==NULL){return "NULL";}
                    //Struct * tp = this->type;
                    string ts("Obj{\n");
                   
                    if(this->type){
                        ts.append(" type: ").append(this->type->toString()).append("\n");
                    }else{
                        ts.append(" type: NULL");
                    }
                    ts.append(" kind: ").append(to_string(this->kind)).append("\n");
                    ts.append(" level: ").append(to_string(this->level)).append("\n");
                    ts.append(" native: ").append(to_string(this->native)).append("\n");
                    ts.append(" native_id: ").append(to_string(this->native_id)).append("\n");
                    ts.append(" locals: ").append(this->locals->toString()).append("\n");
                    ts.append("}\n");
                    return ts; 
                }
        }
    }
}