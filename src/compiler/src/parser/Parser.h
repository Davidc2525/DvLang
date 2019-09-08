

#if !defined(PARSER_COCO_PARSER_H__)
#define PARSER_COCO_PARSER_H__

#include<iostream>
#include <process/Instructions.h>
#include <numbers/numbers.h>
#include "../compiler/generate/symbol_table/symbol_table.h"
#include "../compiler/generate/code_generator/code_generator.h"
using namespace std;
using namespace DVLANG::COMPILER_::GENERATE;
using namespace PROCESS;

class lib_load_info{
  public:
    bool load_any = false;
    string name;
};

typedef  void(*type_f)(string) ;


#include "Scanner.h"

namespace PARSER {


class Errors {
public:
	int count;			// number of errors detected

	Errors();
	Errors(Scanner *scanner);
	void printMsg(int line, int col, const wchar_t *s);

	void SynErr(int line, int col, int n);
	void Error(int line, int col, const wchar_t *s);
	void Warning(int line, int col, const wchar_t *s);
	void Warning(const wchar_t *s);
	void Exception(const wchar_t *s);


public:
	Scanner *scanner;

}; // Errors

class Parser {
private:
	enum {
		_EOF=0,
		_ident=1,
		_number=2,
		_charConst=3,
		_stringConst=4,
		_floatLit=5,
		_class=6,
		_if=7,
		_else=8,
		_while=9,
		_print=10,
		_return=11,
		_void=12,
		_final=13,
		_new=14,
		_package=15,
		_comma=16,
		_colon=17,
		_llopen=18,
		_llclose=19,
		_bopen=20,
		_bclose=21,
		_popen=22,
		_pclose=23,
		_sinte=24,
		_EOL=25,
		_comment=26
	};
	int maxT;

	Token *dummyToken;
	int errDist;
	int minErrDist;

	void SynErr(int n);
	void Get();
	void Expect(int n);
	bool StartOf(int s);
	void ExpectWeak(int n, int follow);
	bool WeakSeparator(int n, int syFol, int repFol);

public:
	Scanner *scanner;
	Errors  *errors;

	Token *t;			// last recognized token
	Token *la;			// lookahead token

SymbolTable   *tab;
  CodeGenerator   *code;
  void * _p;
  Obj * curMethod;

  lib_load_info  * lib_info = new lib_load_info;

  /*eliminar comillas de la cadena*/
  string onlyContent(string str){
    return str.substr(1,str.length()-2);
  }
    
  bool isMethodDecl(){

    if(la->kind==_llopen){
      return true;
    }

    return false;
  }

 
  bool followedByColon(){
    cout<<"followedByColon"<<endl;
    Token * x = la;

    while(x->kind == _ident || x->kind == _comma||x->kind == _bopen || x->kind == _bclose ){
      cout<<"next: "<<x->kind<<endl;
      x = scanner->Peek();
    }
    
    cout<<"IsColon: "<< (x->kind == _colon)<<" curr: "<<x->kind<<" t: "<<t->kind<<endl;
    return x->kind == _colon; 
  }
  bool followedBySinte(){
    cout<<"followedByColon"<<endl;
    Token * x = la;

    while(x->kind == _ident || x->kind == _comma || x->kind == _colon||x->kind == _bopen || x->kind == _bclose ){
      cout<<"next: "<<x->kind<<endl;
      x = scanner->Peek();
    }
    
    return x->kind == _sinte; 
  }

  
  bool NextTokenIsType() {
    if (la->kind != _ident) return false;
    Obj* obj = tab->find(coco_string_create_char(la->val));
    cout<<"|||||||||||||||||||||----- NextTokenIsType: "<<coco_string_create_char(la->val)<<" is type?: "<<(obj->kind == Obj::Type)<<endl;
    return obj->kind == Obj::Type;
  }

  void RelationExpr(Operand*x){
    if(x->fromExprType==Operand::OneExpr){
      code->load(x);
    
      code->put(ICONST);
      code->put(0);
    
      code->put(CMPI);
      code->put(JLE);
      code->put(0);
    }else{

      if(x->relaExprSub==1){ // restar cuando es: a >= b seia a >= b-1
        code->put(ICONST);
        code->put(1);
        code->put(SUB);
      }
      code->put(CMPI);
      code->put(x->relaExprOp);
      code->put(0);
    }
  }

  void FunctionCall(Operand * x){

    if(x->obj->native){
      cout<<" Native call: "<<x->obj->native_id<<endl;
      code->put(ICONST);
      code->put(x->obj->native_id);
      code->put(NCALL);

      if (x->obj->type == tab->noType){
        cout<<"                         FUNCTION NO TYPE, POPING"<<endl;
        code->put(POP);
      }
    }else{
      cout<<" Call: "<<x->obj->adr<<endl;
      if(true){//modo stack
          code->put(ICONST);
          code->put(x->adr);
          code->put(x->co_call?CO_CALL:CALL_S);

      }else{// modo next
          code->put(CALL);
          code->put(x->adr);
      }
    }
  }


	Parser(Scanner *scanner);
	~Parser();
	void SemErr(const wchar_t* msg);

	void Program();
	void LoadExp();
	void LibLoad();
	void ConstDecl();
	void VarDecl(Obj * vd);
	void ClassDecl();
	void FuncDecl();
	void NativeDecl();
	void Type(Struct * type);
	void MethodDecl(Obj * clazz );
	void FormPars(int & npars);
	void Block();
	void Statement();
	void Designator(Operand * x);
	void ActPars(Operand * m);
	void Expr(Operand * x);
	void Mcode();
	void Relop(int & op, int & s);
	void LogicOr(Operand * x);
	void LogicAnd(Operand * x);
	void RelaExpr(Operand * x);
	void AddSub(Operand * x);
	void BitExpr(Operand * x);
	void Addop(int & op);
	void BitShiftExpr(Operand * x);
	void Bitop(int & op);
	void Term(Operand * x);
	void BitShiftop(int & op);
	void Factor(Operand * x);
	void Mulop(int & op);
	void ASM();
	void StatementSequence();
	void McodeStatement();
	void Executable();
	void Directive(int & addres);
	void Label();
	void OneByteOp(int & op);
	void TwoByteOp(int & op);
	void Address(int & addres);
	void KnownAddress(int & addres);
	void Load();

	void Parse();

}; // end Parser

} // namespace


#endif

