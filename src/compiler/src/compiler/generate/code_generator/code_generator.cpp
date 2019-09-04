#include "code_generator.h"

namespace DVLANG
{
namespace COMPILER_
{
namespace GENERATE
{

void CodeGenerator::put(intdv x){
    program[pc++] = x;
};
void CodeGenerator::put2(intdv x){

};
void CodeGenerator::put4(intdv x){

};
void CodeGenerator::assign(Operand *x, Operand *y){

};
void CodeGenerator::fixup(intdv adr){
    program[adr] = pc;
};
void CodeGenerator::store(Operand * x){
     switch (x->kind)
    {
    case Operand::Con:
       
        break;
    case Operand::Static:
        put(PROCESS::OPCODES::ISTORE);
        put(x->adr);
        break;
    case Operand::Local:
        put(PROCESS::OPCODES::ISTOREL);
        put(x->adr);
        break;
    case Operand::Fld: // assert: object base address is on the stack
        put(PROCESS::OPCODES::IPF);
        put(x->adr);
        break;
    case Operand::Elem: // assert: base address and index are on stack
        put(PROCESS::OPCODES::IASTORE);
        //put(x->adr);
        break;
    case Operand::Stack:
        break; // nothing (already loaded)
    default:
        cout << ("cannot store this") << endl;
    }
    
}
void CodeGenerator::load(Operand *x)
{
   /*
    if(x->obj!=NULL){
    //cout<<"  LOAD: "<<x->obj->name<<" kind: "<<x->kind<<endl;
        
        if(x->obj->is_ref){
        
            switch (x->kind)
            {
            
            case Operand::Static:
                put(PROCESS::OPCODES::ADRL);
                put(x->adr);
                break;
            
            case Operand::Local:
                put(PROCESS::OPCODES::ADRL);
                put(x->adr);
                break;
            case Operand::Fld: // assert: object base address is on the stack
                put(PROCESS::OPCODES::IGF);
                put(x->adr);
                break;
            case Operand::Elem: // assert: base address and index are on stack
                put(PROCESS::OPCODES::IALOAD);
                //put(x->adr); 
                break;
            case Operand::Stack:
                break; // nothing (already loaded)
            default:
                cout << ("cannot load this value") << endl;
            }
            
            return;
        }
    }*/
    //cout<<"--------||||||||||operand load"<<x->type->kind<<endl;
    //cout<<"--------||||||||||operand load op kind"<<x->kind<<endl;
    switch (x->kind)
    {
    case Operand::Con:
        if(
            x->type == SymbolTable::intType
             ||x->type == SymbolTable::floatType
        
        ){

            put(PROCESS::OPCODES::ICONST);
            
            put(x->val);
        }

        if(x->type == SymbolTable::stringType
        ||
        x->type->kind == Struct::Arr
        ){
    //cout<<"--------||||||||||operand load string"<<x->type->kind<<endl;


            put(PROCESS::OPCODES::LDCS);
            
            put(x->val);
        }
        break;
    case Operand::Func:
           //put(PROCESS::OPCODES::ICONST);
            //put(x->adr);
            break;
    case Operand::Static:
        put(PROCESS::OPCODES::ILOAD);
        put(x->adr);
        break;
    case Operand::Local:
        put(PROCESS::OPCODES::ILOADL);
        put(x->adr);
        break;
    case Operand::Fld: // assert: object base address is on the stack
        put(PROCESS::OPCODES::IGF);
        put(x->adr);
        break;
    case Operand::Elem: // assert: base address and index are on stack
        put(PROCESS::OPCODES::IALOAD);
        //put(x->adr);
        break;
    case Operand::Stack:
        break; // nothing (already loaded)
    default:
        cout << ("cannot load this value") << endl;
    }

    x->kind = Operand::Stack;
};
intdv *CodeGenerator::program = NULL;
intdv CodeGenerator::pc = 0;
intdv CodeGenerator::main = -1;
int CodeGenerator::globals = 0;
Strings * CodeGenerator::string_set = NULL;

void CodeGenerator::init()
{
    string_set = new Strings();
    
    program = (intdv *)malloc(1000000 * sizeof(intdv));
    pc = 0;
    main = -1;
    globals = 0;
};

void CodeGenerator::write(string filename,LibSymbolStore * libs) {
    for(int x = 0;x<pc;x++){
        cout<<"INST ["<<x<<"] : "<<program[x]<<endl;
    }
    
    int count_libs = libs->lib_symbol.size();

    PROGRAM::SymbInfo** lib_memo = 
            (PROGRAM::SymbInfo**)malloc(count_libs*sizeof(PROGRAM::SymbInfo*));
    
    for(int x = 0 ; x < count_libs;x++){
        
        PROGRAM::SymbInfo* si = new PROGRAM::SymbInfo;
        si->lib_name = libs->lib_symbol[x]->lib_name;
        si->symb_name= libs->lib_symbol[x]->symbol_name;
        si->argc= libs->lib_symbol[x]->argc;
        
        lib_memo[x] = si;
    }
    PROGRAM::LibInfo *li = new PROGRAM::LibInfo;
    li->count=count_libs;
    li->libs = lib_memo; 

    PROGRAM::writeProgramJson(filename,main,pc,globals,program,string_set,li);
}

} // namespace GENERATE
} // namespace COMPILER_
} // namespace DVLANG
