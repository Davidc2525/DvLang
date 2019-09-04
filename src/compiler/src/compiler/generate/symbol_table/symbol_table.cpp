
#include "symbol_table.h"

namespace DVLANG
{
    namespace COMPILER_
    {
        namespace GENERATE
        {
            int SymbolTable::globals = 0;
            Scope * SymbolTable::curScope = NULL; // current top scope
            int SymbolTable::curLevel = NULL; // nesting level of current scope
            Struct * SymbolTable::boolType = NULL; // predefined types
            Struct * SymbolTable::intType = NULL; // predefined types
            Struct * SymbolTable::charType = NULL; // predefined types
            Struct * SymbolTable::floatType = NULL; // predefined types
            Struct * SymbolTable::doubleType = NULL; // predefined types
            Struct * SymbolTable::stringType = NULL;
            Struct * SymbolTable::nullType = NULL;
            Struct * SymbolTable::noType = NULL;

            Obj * SymbolTable::chrObj = NULL; // predefined objects
            Obj * SymbolTable::ordObj = NULL;
            Obj * SymbolTable::lenObj = NULL;
            Obj * SymbolTable::noObj = NULL;

            Obj * SymbolTable::insert(int kind, string name, Struct * type){
                Obj * obj = new Obj(kind,name,type);
                cout<<" insert st, obj kind: "<<kind<<", name "<<name<<", type kind: "<<type->kind<<", level: "<<curLevel<<endl;
                if(type->kind == Struct::Arr){
                    cout<<"  - elemtype kind: "<<type->elemType->kind<<endl;

                }
                if(kind == Obj::Var){
                    obj->adr = curScope->nVars;
                    curScope->nVars++;

                    obj->level = curLevel;

                   if( curLevel == 0){
                       globals = globals+1;;
                   }
                }

                Obj * p = curScope->locals;
                Obj * last = NULL;

                while(p!=NULL){
                    if(p->name == name) cout<<name<<" declared twice"<<endl;
                    last = p;
                    p = p->next;
                }

                if(last == NULL){
                    curScope->locals = obj;
                }else{
                    last->next = obj;
                }
               // cout<<" inserted obj: "<<obj->name<<", p: "<<obj<<endl;
                return obj;
            }

            Obj* SymbolTable::find (string name){
                /*Obj *obj;
                Scope *scope;
	            scope = curScope;
                while (scope != NULL) {  // for all open scopes
                    obj = scope->locals;
                    while (obj != NULL) {  // for all objects in this scope
                        if (obj->name == name){
                             //cout<<" find obj: "<<obj->name<<", p: "<<obj<<endl;
                             return obj;}
                        obj = obj->next;
                    }
                    scope = scope->outer;
                }*/

                for(Scope * s = curScope; s != NULL; s = s->outer){
                    for(Obj * p = s->locals; p != NULL; p = p->next){
                        if(p->name == name) {
                            cout<<" find st, obj kind: "<<p->kind<<", name "<<p->name<<", type kind: "<<p->type->kind<<endl;
                            if(p->type->kind == Struct::Arr){
                                cout<<"  - elemtype kind: "<<p->type->elemType->kind<<endl;

                            }
                            return p;
                        }
                    }
                }
                cout<<name<<" is undeclared"<<endl;
                return noObj;
            };

           Obj* SymbolTable::findField (string name, Struct * t){

                for(Obj * p = t->fields; p != NULL; p = p->next){
                    if(p->name == name) return p;
                }

                cout<<name<<" field is undeclared"<<endl;
                return SymbolTable::noObj;
            };

            void SymbolTable::openScope(){
                Scope * s = new Scope();
                s->outer = curScope;
                curScope = s;
                curLevel++;
            };

            void SymbolTable::closeScope(){
                curScope = curScope->outer;
                curLevel--;
            };

            SymbolTable::SymbolTable(){
               init();
            }

            void SymbolTable::init(){
                cout<<"iniciando tabla de simbolos"<<endl;
                //PROCESS::Native * n = PROCESS::Native::getInstance();
                //Obj *o;
                curScope = new Scope();
                curScope->outer = NULL;
                curLevel = 0;

                //predeclared types
                boolType = new Struct(Struct::Bool);
                intType = new Struct(Struct::Int);
                charType = new Struct(Struct::Char);
                floatType = new Struct(Struct::Float);
                doubleType = new Struct(Struct::Double);
                //stringType = new Struct(Struct::String);
                stringType = new Struct(Struct::Arr,charType);
                //string == char[]
                nullType = new Struct(Struct::Class);
                noType = new Struct(Struct::None);
                noObj = new Obj(Obj::Var,"???",noType);

                //create predecared object
                insert(Obj::Type,TYPES::BOOOLEAN,boolType);
                insert(Obj::Type,TYPES::INT,intType);
                insert(Obj::Type,TYPES::FLOAT,floatType);
                insert(Obj::Type,TYPES::DOUBLE,doubleType);
                insert(Obj::Type,TYPES::STRING,stringType);
                insert(Obj::Type,TYPES::_CHAR_,charType);
                insert(Obj::Con,TYPES::_NULL_,nullType);
                insert(Obj::Type,"void",noType);

                /*chrObj = insert(Obj::Func,"chr",stringType);
                chrObj->native = true;
                chrObj->native_id = n->get(chrObj->name);
                chrObj->locals = new Obj(Obj::Var,"i",intType);
                chrObj->nPars = 1;

                lenObj = insert(Obj::Func,"len",intType);
                lenObj->native = true;
                lenObj->native_id = n->get(lenObj->name);
                lenObj->locals = new Obj(Obj::Var,"a",new Struct(Struct::Arr,noType));
                lenObj->nPars = 1;

                Obj * strlenObj = insert(Obj::Func,"strlen",intType);
                strlenObj->native = true;
                strlenObj->native_id = n->get(strlenObj->name);
                strlenObj->locals = new Obj(Obj::Var,"str",stringType);
                strlenObj->nPars = 1;

                Obj * printlnObj = insert(Obj::Func,"println",noType);
                printlnObj->native = true;
                printlnObj->native_id = n->get(printlnObj->name);
                printlnObj->locals = new Obj(Obj::Var,"str",stringType);
                printlnObj->nPars = 1;

                Obj * printObj = insert(Obj::Func,"print",noType);
                printObj->native = true;
                printObj->native_id = n->get(printObj->name);
                printObj->locals = new Obj(Obj::Var,"str",stringType);
                printObj->nPars = 1;

                Obj * sleepObj = insert(Obj::Func,"native_sleep",intType);
                sleepObj->native = true;
                sleepObj->native_id = n->get(sleepObj->name);
                sleepObj->locals = new Obj(Obj::Var,"s",intType);
                sleepObj->nPars = 1;

                Obj * msleepObj = insert(Obj::Func,"native_u_sleep",intType);
                msleepObj->native = true;
                msleepObj->native_id = n->get(msleepObj->name);
                msleepObj->locals = new Obj(Obj::Var,"ms",intType);
                msleepObj->nPars = 1;

                Obj * sumObj = insert(Obj::Func,"sum",intType);
                sumObj->native = true;
                sumObj->native_id = n->get(sumObj->name);
                sumObj->locals = new Obj(Obj::Var,"a",intType);
                sumObj->locals->next = new Obj(Obj::Var,"b",intType);
                sumObj->nPars = 2;

                Obj * itoObj = insert(Obj::Func,"native_itos",stringType);
                itoObj->native = true;
                itoObj->native_id = n->get(itoObj->name);
                itoObj->locals = new Obj(Obj::Var,"a",intType);
                itoObj->nPars = 2;*/


                /*cout<<"find: "<<find("strlen")->toString()<<";"<<endl;
                cout<<"find: "<<find("len")->toString()<<";"<<endl;
                cout<<"find: "<<find("chr")->toString()<<";"<<endl;
                */

            }
        }
    } // namespace COMPILER
} // namespace DVLANG
