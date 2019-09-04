
#include "symbol_table.h"

namespace DVLANG
{
    namespace COMPILER_
    {
        namespace GENERATE
        {
            Struct::Struct(int kind){
                this->kind = kind;
                this->nFields = 0;
                this->fields = NULL;
                this->elemType = NULL;
            }

            Struct::Struct(int kind, Struct * elemType){
                this->kind = kind;
                this->elemType = elemType;
                this->nFields = 0;
                this->fields = new Obj();
            }

            Struct::Struct(int kind, Struct & elemType){
                this->kind = kind;
                this->elemType = &elemType;
                this->nFields = 0;
                this->fields = new Obj();
            }

            void Struct::assign(Struct * o){
                this->kind = o->kind;
                this->elemType = (o->elemType);
                this->nFields = o->nFields;
                this->fields = o->fields;
            }

            bool Struct::isRefType(){
                return kind == Class || kind == Arr || kind == String;
            };
            bool Struct::equals(Struct * other){
                if(kind == Arr){
                    return other->kind == Arr && other->elemType == elemType;
                }else{
                    return this->kind==other->kind;
                }
            };
            bool Struct::compatibleWith(Struct * other){
                return this->equals(other)
                    || this == SymbolTable::nullType && other->isRefType()
                    || other == SymbolTable::nullType && this->isRefType();
            };
            bool Struct::assignableTo(Struct * dest){
                return this->equals(dest)
                    || this == SymbolTable::nullType && dest->isRefType()
                    || kind == Arr && dest->kind == Arr && dest->elemType == SymbolTable::noType;
            };

            string Struct::toString(){
                if(this==NULL){return "NULL";}
               // Struct * et = this->elemType;
                //Obj * fls = this->fields;

                string ts("Struct{\n");
                ts.append(" kind: ").append(to_string(this->kind)).append("\n");
                if(this->elemType){
                    ts.append(" elenType: ").append(this->elemType->toString()).append("\n");
                }else{
                    ts.append(" elenType: NULL\n");
                }

                ts.append(" nFields: ").append(to_string(this->nFields)).append("\n");

                if(this->fields){
                    ts.append(" fields: ").append(this->fields->toString()).append("\n");
                }else{
                    ts.append(" fields: NULL");
                }
                ts.append("}\n");
                return ts;
            };


        }; // GENERATE

    }; // COMPILER

}; // DVLANG
