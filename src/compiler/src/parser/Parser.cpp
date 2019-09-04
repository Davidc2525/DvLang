

#include <wchar.h>
#include "Parser.h"
#include "Scanner.h"


namespace PARSER {


void Parser::SynErr(int n) {
	if (errDist >= minErrDist) errors->SynErr(la->line, la->col, n);
	errDist = 0;
}

void Parser::SemErr(const wchar_t* msg) {
	if (errDist >= minErrDist) errors->Error(t->line, t->col, msg);
	errDist = 0;
}

void Parser::Get() {
	for (;;) {
		t = la;
		la = scanner->Scan();
		if (la->kind <= maxT) { ++errDist; break; }

		if (dummyToken != t) {
			dummyToken->kind = t->kind;
			dummyToken->pos = t->pos;
			dummyToken->col = t->col;
			dummyToken->line = t->line;
			dummyToken->next = NULL;
			coco_string_delete(dummyToken->val);
			dummyToken->val = coco_string_create(t->val);
			t = dummyToken;
		}
		la = t;
	}
}

void Parser::Expect(int n) {
	if (la->kind==n) Get(); else { SynErr(n); }
}

void Parser::ExpectWeak(int n, int follow) {
	if (la->kind == n) Get();
	else {
		SynErr(n);
		while (!StartOf(follow)) Get();
	}
}

bool Parser::WeakSeparator(int n, int syFol, int repFol) {
	if (la->kind == n) {Get(); return true;}
	else if (StartOf(repFol)) {return false;}
	else {
		SynErr(n);
		while (!(StartOf(syFol) || StartOf(repFol) || StartOf(0))) {
			Get();
		}
		return StartOf(syFol);
	}
}

void Parser::Program() {
		Expect(_package);
		Expect(_ident);
		Obj *vd = new Obj();  
		if (StartOf(1)) {
			LoadExp();
		}
		if (la->kind == 70 /* "@>l" */ || la->kind == 71 /* "@load_lib" */) {
			LibLoad();
		}
		
		while (StartOf(2)) {
			if (la->kind == _final) {
				ConstDecl();
			} else if (la->kind == _ident || la->kind == 28 /* "fun" */) {
				VarDecl(vd);
			} else {
				ClassDecl();
			}
		}
		Expect(_llopen);
		while (StartOf(3)) {
			if (la->kind == _ident || la->kind == _void || la->kind == 28 /* "fun" */) {
				FuncDecl();
			} else {
				NativeDecl();
			}
		}
		Expect(_llclose);
		
}

void Parser::LoadExp() {
		while (la->kind == 68 /* "@>" */ || la->kind == 69 /* "@include" */) {
			Load();
		}
}

void Parser::LibLoad() {
		if (la->kind == 70 /* "@>l" */) {
			Get();
		} else if (la->kind == 71 /* "@load_lib" */) {
			Get();
		} else SynErr(73);
		Expect(_stringConst);
		if(lib_info->load_any){
		 SemErr(L"No se puede cargar mas de una libreria por archivo.");
		}
		string lib_path(onlyContent(coco_string_create_char(t->val)));
		lib_info->name = lib_path;
		lib_info->load_any=true;
		cout<<"lib_load: "<<lib_info->name<<endl;
		
		Expect(_colon);
}

void Parser::ConstDecl() {
		Struct * type = new Struct(0); 
		Expect(_final);
		Type(type);
		Expect(_ident);
		Expect(27 /* "=" */);
		if (la->kind == _number) {
			Get();
		} else if (la->kind == _stringConst) {
			Get();
		} else SynErr(74);
		Expect(_colon);
}

void Parser::VarDecl(Obj * vd) {
		Struct * _type = new Struct(0); 
		Type(_type);
		cout<<"vd type: "<<_type->kind<<endl;;  
		Expect(_ident);
		*vd = *tab->insert(Obj::Var, coco_string_create_char(t->val), _type);  
		cout<<"- Var decl name: "<<vd->name<<", type kind: "<<vd->type->kind<<", obj kind: "<<vd->kind<<endl;
		if(vd->type->kind == Struct::Arr){
		cout<<"	elemtype kind: "<<vd->type->elemType->kind<<endl;
		}          
		
		while (la->kind == _comma) {
			Get();
			Expect(_ident);
			*vd = *tab->insert(Obj::Var, coco_string_create_char(t->val), _type); 
			cout<<"- Var decl name: "<<vd->name<<", type kind: "<<vd->type->kind<<", obj kind: "<<vd->kind<<endl;
			if(vd->type->kind == Struct::Arr){
			cout<<"	elemtype kind: "<<vd->type->elemType->kind<<endl;
			}
			
			
		}
		Expect(_colon);
}

void Parser::ClassDecl() {
		Obj * clazz,*vd;  string class_name; Struct * type = new Struct(Struct::Class); 
		Expect(_class);
		Expect(_ident);
		class_name = string(coco_string_create_char(t->val)); 
		clazz = tab->insert(Obj::Type,class_name,type);
		
		Expect(_llopen);
		tab->openScope(); cout<<"class decl: "<<class_name<<" scope opener: "<<tab->curLevel<<endl; 
		while (la->kind == _ident || la->kind == _void || la->kind == 28 /* "fun" */) {
			vd = (Obj*)malloc(sizeof(Obj)); 
			if (followedByColon()) {
				VarDecl(vd);
				clazz->type->nFields++;
				cout<<"class field add: "<<vd->name<<endl;
				clazz->type->fields = tab->curScope->locals;           
				
			} else {
				MethodDecl(clazz);
			}
		}
		clazz->type->fields = tab->curScope->locals; 
		Expect(_llclose);
		cout<<"class scope: "<<tab->curLevel<<", name:"<<clazz->name<<", fields: "<<clazz->type->nFields<<endl; 
		cout<<"class closing scope: "<<tab->curLevel<<endl;   tab->closeScope(); 
}

void Parser::FuncDecl() {
		int npars = 0; int p = 0; Obj * vd = new Obj(); Struct * type = new Struct(0); string name_method; 
		if (la->kind == _ident || la->kind == 28 /* "fun" */) {
			Type(type);
		} else if (la->kind == _void) {
			Get();
			type = tab->noType; 
		} else SynErr(75);
		Expect(_ident);
		name_method = string(coco_string_create_char(t->val));
		curMethod = tab->insert(Obj::Func,name_method,type); 
		tab->openScope(); 
		cout<<"metohd new scope opener: "<<tab->curLevel<<endl; 
		Expect(_popen);
		if (la->kind == _ident || la->kind == 28 /* "fun" */) {
			FormPars(npars);
		}
		Expect(_pclose);
		curMethod->nPars = npars;
		cout<<"numerp de parametros: "<<npars<<endl; 
		if(name_method=="main"){
		CodeGenerator::main = CodeGenerator::pc;
		
		}
		
		curMethod->locals = tab->curScope->locals;
		curMethod->adr = CodeGenerator::pc;
		code->put(ENTER);
		p = CodeGenerator::pc;
		code->put(curMethod->nPars);
		code->put(tab->curScope->nVars);
		
		
		Block();
		code->program[p]   = npars;
		code->program[p+1] = tab->curScope->nVars;
		
		if(curMethod->type == tab->noType){
		     code->put(EXIT);
		    code->put(RET);
		}
		
		if(name_method=="main"){
		 code->put(HLT);
		}
		cout<<"metohd  closing scope: "<<tab->curLevel<<endl;  tab->closeScope(); 
}

void Parser::NativeDecl() {
		bool ref = false;  Obj * native; int npars = 0;  Struct * _type = new Struct(0); 
		Expect(29 /* "native" */);
		if (la->kind == _ident || la->kind == 28 /* "fun" */) {
			Type(_type);
		} else if (la->kind == _void) {
			Get();
			_type = tab->noType; 
		} else SynErr(76);
		Expect(_ident);
		string symbol_name = string(coco_string_create_char(t->val));
		native = tab->insert(Obj::Func, symbol_name, _type);
		native->native=true;
		
		tab->openScope();
		
		
		Expect(_popen);
		if (la->kind == _ident || la->kind == 28 /* "fun" */) {
			FormPars(npars);
		}
		Expect(_pclose);
		if(lib_info->load_any){
		 Obj * nc = tab->find(symbol_name);
		 nc->nPars = npars;
		
		 tab->lib_symbols->put(lib_info->name,symbol_name,npars);
		
		 native->native_id = tab->lib_symbols->get_symbl(symbol_name);
		 cout<<"Register native: lib "<<lib_info->name<<" symbol: "<<symbol_name<<" args"<<nc->nPars<<" native id: "<< native->native_id<<endl;
		
		}else{
		
		 SemErr(L"---------------- No se ha cargao ninguna libreria aun.");
		
		}
		
		tab->closeScope();
		
		Expect(_colon);
}

void Parser::Type(Struct * type) {
		string name_type;  int is_arr = 0;  int is_fun = 0; 
		if (la->kind == 28 /* "fun" */) {
			Get();
			is_fun = 1; 
		}
		Expect(_ident);
		name_type = string(coco_string_create_char(t->val)); 
		if (la->kind == _bopen) {
			Get();
			Expect(_bclose);
			is_arr = 1; 
		}
		Struct * _type = tab->find(name_type)->type;
		
		*type = *_type;
		
		if(is_arr == 1){
		  Struct * _t = new Struct(Struct::Arr);
		  _t->elemType = new Struct(_type->kind);
		  _t->elemType->assign(_type);
		  *type = *_t;
		}
		
		if(is_fun == 1){
		  cout<<"------- posible argumento funcion: "<<name_type<<endl;
		Struct * _t = new Struct(Struct::Func);
		_t->elemType = new Struct(_type->kind);
		*type = *_t;
		}
		
		
		
}

void Parser::MethodDecl(Obj * clazz ) {
		int p = 0; int npars = 0; Obj * vd = new Obj(); Struct * type = new Struct(0); string name_method; 
		if (la->kind == _ident || la->kind == 28 /* "fun" */) {
			Type(type);
		} else if (la->kind == _void) {
			Get();
			type = tab->noType; 
		} else SynErr(77);
		Expect(_ident);
		name_method = string(coco_string_create_char(t->val));
		
		/* cout<<"------- metodo type kind : "<<type->kind<<endl;
		Struct * _t = new Struct(Struct::Meth);
		_t->elemType = new Struct(type->kind);
		*type = *_t;
		cout<<"------- to metodo type kind : "<<type->kind<<" elemType kind: "<<type->elemType->kind<<endl;
		*/
		
		curMethod = tab->insert(Obj::Meth,name_method,type); 
		tab->openScope(); 
		cout<<"metohd new scope opener: "<<tab->curLevel<<endl; 
		Expect(_popen);
		tab->insert(Obj::Var, "this", clazz->type);  
		if (la->kind == _ident || la->kind == 28 /* "fun" */) {
			FormPars(npars);
		}
		Expect(_pclose);
		curMethod->nPars = npars;
		cout<<"numerp de parametros: "<<npars+1<<endl; 
		//tab->insert(Obj::Var, "this", type);   
		
		curMethod->locals = tab->curScope->locals;
		curMethod->adr = CodeGenerator::pc;
		code->put(ENTER);
		
		p = CodeGenerator::pc;
		code->put(0);
		code->put(0);  
		
		        
		
		Block();
		code->program[p]  = npars + 1;
		code->program[p+1] = tab->curScope->nVars;
		
		if(curMethod->type == tab->noType){
		   code->put(EXIT);
		   code->put(RET);
		}
		
		cout<<"metohd  closing scope: "<<tab->curLevel<<endl;  tab->closeScope(); 
}

void Parser::FormPars(int & npars) {
		bool ref = false; Obj * vd; Struct * _type = new Struct(0); 
		Type(_type);
		Expect(_ident);
		npars++; vd = tab->insert(Obj::Var, coco_string_create_char(t->val), _type); 
		cout<<"- FormPar decl name: "<<vd->name<<", type kind: "<<vd->type->kind<<", obj kind: "<<vd->kind<<endl;
		if(vd->type->kind == Struct::Arr){
		cout<<"	elemtype kind: "<<vd->type->elemType->kind<<endl;
		}
		
		
		while (la->kind == _comma) {
			Get();
			Struct * _type2 = new Struct(0); 
			Type(_type2);
			Expect(_ident);
			npars++; vd = tab->insert(Obj::Var, coco_string_create_char(t->val), _type2);
			cout<<"- FormPar decl name: "<<vd->name<<", type kind: "<<vd->type->kind<<", obj kind: "<<vd->kind<<endl;
			if(vd->type->kind == Struct::Arr){
			cout<<"	elemtype kind: "<<vd->type->elemType->kind<<endl;
			}
			
			
		}
}

void Parser::Block() {
		Obj * vd = new Obj();  
		Expect(_llopen);
		while (StartOf(4)) {
			if (followedByColon()) {
				VarDecl(vd);
			} else {
				Statement();
			}
		}
		Expect(_llclose);
}

void Parser::Statement() {
		Operand * x = new Operand(0); Operand * y = new Operand(0); 
		switch (la->kind) {
		case _ident: {
			Designator(x);
			if (la->kind == _popen) {
				ActPars(x);
				FunctionCall(x); /*code->put(ICONST); code->put(x->obj->adr);code->put(CALL_S);*/ /*code->load(x); */  cout<<"call procedure: "<<x->obj->name<<" : "<<x->obj->adr<<endl; 
				if (x->obj->type != tab->noType){
				    cout<<"-------------- CALL PROCEDURE POPING"<<endl;
				    code->put(POP);
				  }
				
			} else if (la->kind == 27 /* "=" */) {
				Get();
				Expr(y);
				cout <<"   ||||||||||||| assignacion statement op expr type: "<<y->fromExprType<<endl; 
				if(y->fromExprType == Operand::OneExpr){
				code->load(y); code->store(x); 
				}else if(y->fromExprType == Operand::RelaExpr){
				  code->load(y);
				  if(y->relaExprSub==1){ // restar cuando es: a >= b seia a >= b-1
				     code->put(ICONST);
				     code->put(1);
				     code->put(SUB);
				   }
				   code->put(CMPI);
				   /*
				   | "!="(. op=JE ; s = 0; .)
				| ">" (. op=JLE; s = 0; .)
				| ">="(. op=JLE; s = 1; .)
				| "<" (. op=JGE; s = 0; .)
				| "<="(. op=JG ; s = 0; .)*/
				
				   switch(y->relaExprOp){
				     case JNE:{
				       code->put(JE+70);
				       break;
				     }
				     case JE:{
				       code->put(JNE+70);
				       break;
				     }
				     case JG:{
				       code->put(JL+70);
				       break;
				     }
				     case JGE:{
				       code->put(JLE+70);
				       break;
				     }
				     case JLE:{
				       code->put(JGE+70);
				       break;
				     }
				   }
				   code->store(x); 
				   //code->put(y->relaExprOp);
				
				}
				
			} else SynErr(78);
			Expect(_colon);
			break;
		}
		case _if: {
			Get();
			int top = 0;  int adr = 0; int adr2 = 0; 
			top = CodeGenerator::pc; cout<<"if en linea: "<<  CodeGenerator::pc<<endl; 
			if (la->kind == _popen) {
				Get();
			}
			Expr(x);
			if (la->kind == _pclose) {
				Get();
			}
			RelationExpr(x);
			adr = CodeGenerator::pc-1;
			
			
			Statement();
			if (la->kind == _else) {
				Get();
				code->put(JMP); code->put(0);  
				adr2 = CodeGenerator::pc-1; 
				code->fixup(adr);
				
				Statement();
				code->fixup(adr2); 
			} else if (StartOf(5)) {
				code->fixup(adr); 
			} else SynErr(79);
			break;
		}
		case _while: {
			Get();
			int top = 0;  int adr = 0;
			top = CodeGenerator::pc; cout<<"while en linea: "<<  CodeGenerator::pc<<endl; 
			if (la->kind == _popen) {
				Get();
			}
			Expr(x);
			if (la->kind == _pclose) {
				Get();
			}
			cout <<"   ||||||||||||| while statement op expr type: "<<x->fromExprType<<endl; 
			 
			 RelationExpr(x);
			 adr = CodeGenerator::pc-1; cout<<"while after cond en linea: "<<adr<<endl; 
			
			
			Statement();
			code->put(JMP); code->put(top); code->fixup(adr); 
			break;
		}
		case _return: {
			Get();
			if (StartOf(6)) {
				Expr(x);
				code->load(x);
				if(curMethod->type == tab->noType){
				cout<<"void metohd must not return a value"<<endl;
				}
				
			} else if (la->kind == _colon) {
				if(curMethod->type != tab->noType){
				cout<<"return value expected"<<endl;
				
				}
			} else SynErr(80);
			code->put(EXIT); code->put(RET); 
			Expect(_colon);
			break;
		}
		case 51 /* "mcode" */: {
			Mcode();
			break;
		}
		case _print: {
			Get();
			int wlen = 0; Operand * len = new Operand(0);  
			Expect(_popen);
			Expr(x);
			
			if (la->kind == _comma) {
				Get();
				Expr(len);
				wlen = 1; 
			}
			Expect(_pclose);
			if(wlen == 1){
			code->load(len); 
			code->load(x);
			code->put(CWRITE);
			}else{
			     code->load(x); 
			     if(x->type->kind == Struct::Int){
			code->put(IWRITE);
			       
			     }
			     if(x->type->kind == Struct::Float){
			       
			code->put(FWRITE);
			     }
			     
			}
			
			Expect(_colon);
			break;
		}
		case _llopen: {
			Block();
			break;
		}
		case _void: {
			Get();
			Expr(y);
			code->put(POP); 
			break;
		}
		case _colon: {
			Get();
			break;
		}
		default: SynErr(81); break;
		}
}

void Parser::Designator(Operand * x) {
		Operand * y = new Operand(0); string name, f_name,qua;  
		Expect(_ident);
		name = coco_string_create_char(t->val);
		qua = name;
		Obj * obj = tab->find(name); 
		if(obj == SymbolTable::noObj){
		string err = "no se encontro: "+name;
		SemErr(L"");
		cout<<err<<endl;
		errors->Exception(L"");
		//exit(1);
		}
		cout<<"Designator name: "<<name<<" tyoe kind: "<<x->type->kind <<" op kind: "<<x->kind<<endl;
		*x = *new Operand(obj); 
		cout<<"Designator name: "<<name<<" tyoe kind: "<<x->type->kind <<" op kind: "<<x->kind<<endl;
		
		if(obj->kind == Struct::Func){
		//x->kind == Operand::Func;
		//cout<<"----- operan to func"<<endl;
		}
		
		while (la->kind == _bopen || la->kind == 41 /* "." */) {
			if (la->kind == 41 /* "." */) {
				Get();
				Expect(_ident);
				if(x->type->kind == Struct::Class){
				code->load(x);
				
				
				f_name = coco_string_create_char(t->val);
				cout<<"Designator f_name: "<<f_name<<endl;
				Obj * fld = tab->findField(f_name,x->type);
				if(fld == SymbolTable::noObj){
				
				string err = to_string(x->type->nFields)+" No se encontro el campo '"+f_name+"' en el objeto '"+qua+"'";
				SemErr(L"");
				cout<<err<<endl;
				errors->Exception(L"");
				}
				qua.append(".").append(f_name);
				*x = *new Operand(fld);
				if(fld->kind == Obj::Var){
				
				x->kind = Operand::Fld;
				
				}else if(fld->kind == Obj::Meth){
				
				x->kind = Operand::Meth;
				}
				x->adr = fld->adr;
				x->type = fld->type;
				cout<<"Designator f_name: "<<f_name<<" op kind: "<<x->kind<<endl;
				
				} else{
				string err = qua+" no es un objeto.";
				SemErr(L"");
				cout<<err<<endl;
				errors->Exception(L"");
				}
				
			} else {
				Get();
				code->load(x); 
				Expr(y);
				if(y->type->kind != Struct::Int ){
				    string err = "El indice debe ser de tipo int";
				      SemErr(L"");
				      cout<<err<<endl;
				      errors->Exception(L"");
				}else{
				    qua.append("[").append(to_string(y->val)).append("]");
				    cout<<" x name: "<<(x->obj->name )<<" " << Struct::Arr<<endl;
				cout<<qua<<" type  kind : "<<(x->type->kind )<<", expected: " << Struct::Arr<<endl;
				
				    if(x->type->kind == Struct::Arr){
				      cout<<" x->type->elemType->kind: "<<x->type->elemType->kind <<endl;
				      cout<<" index type: "<<y->type->kind<<endl;
				        
				    
				    code->load(y);
				      x->kind = Operand::Elem;
				      x->type = x->type->elemType;
				    
				      
				    }else{
				      cout<<name<<" is not an array"<<endl;
				    }
				}
				
				
				Expect(_bclose);
				while (la->kind == _bopen) {
					Get();
					code->put(IALOAD); 
					Expr(y);
					if(y->type->kind != Struct::Int ){
					   string err = "El indice debe ser de tipo int";
					     SemErr(L"");
					     cout<<err<<endl;
					     errors->Exception(L"");
					}else{
					   qua.append("[").append(to_string(y->val)).append("]");
					   cout<<qua<<" type  kind : "<<(x->type->kind )<<", expected: " << Struct::Arr<<endl;
					   if(x->type->kind == Struct::Arr){
					     //cout<<" x->type->elemType->kind: "<<x->type->elemType->kind <<endl;
					     //cout<<" index type: "<<y->type->kind<<endl;
					       
					     code->load(y);
					     x->kind = Operand::Elem;
					     x->type = x->type->elemType;
					   
					     
					   }else{
					     cout<<name<<" is not an array"<<endl;
					   }
					}
					
					
					Expect(_bclose);
				}
				cout<<"arr "<<qua<<endl; 
			}
		}
}

void Parser::ActPars(Operand * m) {
		Expect(_popen);
		if (StartOf(6)) {
			Operand *ap1 = new Operand(0); 
			Expr(ap1);
			code->load(ap1); 
			while (la->kind == _comma) {
				Get();
				Operand *ap2 = new Operand(0); 
				Expr(ap2);
				code->load(ap2); 
			}
		}
		Expect(_pclose);
}

void Parser::Expr(Operand * x) {
		LogicOr(x);
}

void Parser::Mcode() {
		Expect(51 /* "mcode" */);
		Expect(52 /* "begin" */);
		ASM();
		Expect(53 /* "end" */);
}

void Parser::Relop(int & op, int & s) {
		switch (la->kind) {
		case 30 /* "==" */: {
			Get();
			op=18; 
			break;
		}
		case 31 /* "!=" */: {
			Get();
			op=JE ; s = 0; 
			break;
		}
		case 32 /* ">" */: {
			Get();
			op=JLE; s = 0; 
			break;
		}
		case 33 /* ">=" */: {
			Get();
			op=JLE; s = 1; 
			break;
		}
		case 34 /* "<" */: {
			Get();
			op=JGE; s = 0; 
			break;
		}
		case 35 /* "<=" */: {
			Get();
			op=JG ; s = 0; 
			break;
		}
		default: SynErr(82); break;
		}
}

void Parser::LogicOr(Operand * x) {
		Operand *y = new Operand(0); 
		LogicAnd(x);
		while (la->kind == 36 /* "||" */) {
			Get();
			LogicAnd(x);
		}
}

void Parser::LogicAnd(Operand * x) {
		Operand *y = new Operand(0); 
		RelaExpr(x);
		while (la->kind == 37 /* "&&" */) {
			Get();
			RelaExpr(y);
		}
}

void Parser::RelaExpr(Operand * x) {
		int one_op = 1; int sub=0; Operand *y = new Operand(0); int op = 0;
		AddSub(x);
		if (StartOf(7)) {
			Relop(op,sub);
			x->fromExprType = Operand::RelaExpr; x->relaExprSub=sub;x->relaExprOp=op;
			AddSub(y);
			cout<<"      |||||||||||||||||| exprecion de relacion: = "<< Operand::RelaExpr<<endl;
			code->load(x);
			code->load(y);
			
		}
}

void Parser::AddSub(Operand * x) {
		Operand *y = new Operand(0);  int op = 0; int sub = 0; int invert = 0; 
		if (la->kind == 38 /* "-" */ || la->kind == 39 /* "!" */) {
			if (la->kind == 38 /* "-" */) {
				Get();
				sub = 1; 
			} else {
				Get();
				invert = 1; 
			}
		}
		BitExpr(x);
		if(sub==1){ 
		 code->put(ICONST); code->put(0);
		 code->load(x); 
		 code->put(SUB);
		}else if(invert ==1){
		   code->load(x);
		   code->put(INVERT);
		} 
		while (la->kind == 38 /* "-" */ || la->kind == 42 /* "+" */) {
			Addop(op);
			BitExpr(y);
			cout<<"-------------------- be string x "<<x->type->kind<<endl;
			cout<<"-------------------- be string y "<<y->type->kind<<endl;
			if(
			(x->type == SymbolTable::stringType || x->type->kind == Struct::Arr)
			||
			(x->type == SymbolTable::stringType || y->type->kind == Struct::Arr)
			){
			
			code->load(y);
			if(y->type == SymbolTable::stringType || y->type->kind == Struct::Arr){
			  cout<<"-------------------- be string y elemtype kind "<<y->type->elemType->kind<<endl;
			
			}else{
			  cout<<"--------------------     op y no es de tipo string o char[] "<<y->type->kind<<" anteponeindo itos"<<endl;
			  if(y->type->kind == Struct::Int){
			    code->put(ITOS);
			
			  }else if(y->type->kind == Struct::Float){
			    code->put(FTOS);
			  }else if(y->type->kind == Struct::Double){
			    code->put(DTOS);
			  }else if(false && y->type->kind == Struct::Class){
			    Obj * tmpO = tab->find(y->obj->name);
			    Operand * otmp0 = new Operand(tmpO);
			    Obj * fld = tab->findField("toString",tmpO->type);
			    Operand * otmp1 = new Operand(fld);
			    cout<<"op y: "<< tmpO->name <<", es un objeto de clase: "<<endl;
			    if(fld != SymbolTable::noObj){
			      
			    cout<<"op y contiene el metodo toString: adr "<<fld->adr<<endl;
			    //llamar al metodo toString de la clase
			    code->load(otmp0);
			    FunctionCall(otmp1);
			    }else{
			      
			    
			    SemErr(L"---- op y NO contiene el metodo toString.");
			    }
			    //SemErr(L"---- op y es un objeto de clase .");
			  }else{
			    
			    SemErr(L"---- op y no se puede llevar implicitamente a string.");
			  }
			}
			
			
			code->load(x);
			if(x->type == SymbolTable::stringType || x->type->kind == Struct::Arr){
			  cout<<"-------------------- be string x elemtype kind "<<x->type->elemType->kind<<endl;
			  
			}else{
			  cout<<"--------------------     op x no es de tipo string o char[] "<<y->type->kind<<" anteponeindo itos"<<endl;
			  if(x->type->kind == Struct::Int){
			    code->put(ITOS);
			
			  }else if(x->type->kind == Struct::Float){
			    code->put(FTOS);
			  }else if(x->type->kind == Struct::Double){
			    code->put(DTOS);
			  }else if(false && x->type->kind == Struct::Class){
			    Obj * tmpO = tab->find(x->obj->name);
			    Operand * otmp0 = new Operand(tmpO);
			    Obj * fld = tab->findField("toString",tmpO->type);
			    Operand * otmp1 = new Operand(fld);
			    cout<<"op x: "<< tmpO->name <<", es un objeto de clase: "<<endl;
			    if(fld != SymbolTable::noObj){
			      
			    cout<<"op x contiene el metodo toString: adr "<<fld->adr<<endl;
			    //llamar al metodo toString de la clase
			    code->load(otmp0);
			    FunctionCall(otmp1);
			    }else{
			      
			    
			    SemErr(L"---- op x NO contiene el metodo toString.");
			    }
			    //SemErr(L"---- op x es un objeto de clase .");
			  }else{
			    
			    SemErr(L"---- op x no se puede llevar implicitamente a string.");
			  }
			}
			/*
			if(x->type->elemType->kind == Struct::Char || y->type->elemType->kind == Struct::Char){
			
			}else{
			  SemErr(L"------------- x o y no son string o char[]");
			}*/
			
			
			cout<<"-------------------- string x "<<x->type->kind<<endl;
			cout<<"-------------------- string y "<<y->type->kind<<endl;
			
			
			code->put(STRCAT);
			
			}else  if(
			(x->type == SymbolTable::floatType || x->type->kind == Struct::Float)
			||
			(y->type == SymbolTable::floatType || y->type->kind == Struct::Float)
			){
			
			 code->load(x);
			  cout<<"||||||||||||||||||||| 1 x float: "<<x->type->kind<<" val:"<<x->val<<endl;
			  if(x->type != SymbolTable::floatType && x->type->kind != Struct::Float){
			      cout<<"||||||||||||||||||||| 2 x not float, ITOF"<<x->type->kind<<endl;
			      code->put(ITOF);
			      x->type =SymbolTable::floatType;
			  }
			  code->load(y);
			  cout<<"||||||||||||||||||||| 1 y float: "<<y->type->kind<<" val:"<<x->val<<endl;
			  if(y->type != SymbolTable::floatType && y->type->kind != Struct::Float){
			      cout<<"||||||||||||||||||||| 2 y not float, ITOF"<<y->type->kind<<endl;
			      code->put(ITOF);
			      y->type =SymbolTable::floatType;
			  }
			  
			  switch(op){
			    case ADD:{
			      code->put(FADD);
			      break;
			    }
			    case SUB:{
			      code->put(FSUB);
			      break;
			    }
			    case MUL:{
			      code->put(FMUL);
			      break;
			    }
			    case DIV:{
			      code->put(FDIV);
			      break;
			    }
			    case MOD:{
			      code->put(FMOD);
			      break;
			    }
			  }
			
			
			}else if(
			(x->type == SymbolTable::intType || x->type->kind == Struct::Int)
			||
			(y->type == SymbolTable::intType || y->type->kind == Struct::Int)
			){
			code->load(x);
			code->load(y);
			code->put(op);
			}
			
			
			
		}
}

void Parser::BitExpr(Operand * x) {
		Operand *y = new Operand(0);  int op = 0; 
		BitShiftExpr(x);
		while (la->kind == 46 /* "&" */ || la->kind == 47 /* "|" */ || la->kind == 48 /* "^" */) {
			Bitop(op);
			BitShiftExpr(y);
			code->load(x);
			code->load(y);
			code->put(op);
			
			
		}
}

void Parser::Addop(int & op) {
		if (la->kind == 42 /* "+" */) {
			Get();
			op = 1; 
		} else if (la->kind == 38 /* "-" */) {
			Get();
			op = 2; 
		} else SynErr(83);
}

void Parser::BitShiftExpr(Operand * x) {
		Operand *y = new Operand(0);  int op = 0; 
		Term(x);
		while (la->kind == 45 /* "%" */ || la->kind == 49 /* "<<" */ || la->kind == 50 /* ">>" */) {
			BitShiftop(op);
			Term(y);
			code->load(x);
			code->load(y);
			code->put(op);
			
			
		}
}

void Parser::Bitop(int & op) {
		if (la->kind == 46 /* "&" */) {
			Get();
			op = 3; 
		} else if (la->kind == 47 /* "|" */) {
			Get();
			op = 4; 
		} else if (la->kind == 48 /* "^" */) {
			Get();
			op = 5; 
		} else SynErr(84);
}

void Parser::Term(Operand * x) {
		Operand *y = new Operand(0);  int op = 0; 
		Factor(x);
		while (la->kind == 43 /* "*" */ || la->kind == 44 /* "/" */ || la->kind == 45 /* "%" */) {
			Mulop(op);
			Factor(y);
			code->load(x);
			code->load(y);
			code->put(op);
			
			
		}
}

void Parser::BitShiftop(int & op) {
		if (la->kind == 49 /* "<<" */) {
			Get();
			op = 3; 
		} else if (la->kind == 50 /* ">>" */) {
			Get();
			op = 4; 
		} else if (la->kind == 45 /* "%" */) {
			Get();
			op = 5; 
		} else SynErr(85);
}

void Parser::Factor(Operand * x) {
		Operand * y = new Operand(0);  
		if (la->kind == _ident) {
			Designator(x);
			if(x->kind != Operand::Func){code->load(x);} code->load(x); 
			if (la->kind == _popen) {
				ActPars(x);
				FunctionCall(x); /*code->put(ICONST); code->put(x->obj->adr); code->put(CALL_S);*/ /*code->load(x); */  cout<<"call function: "<<x->obj->adr<<endl; 
			}
		} else if (la->kind == _ident) {
			Get();
			string ident_name = coco_string_create_char(t->val); 
			Obj * obj = tab->find(ident_name);
			*x = *new Operand(obj);
			cout<<"loading"<<endl;
			//code->load(x);
			
		} else if (la->kind == _floatLit) {
			Get();
			int val = 0; string str_val = coco_string_create_char(t->val);
			val = NUMBERS::Numbers::FloatToBitInt(stof(coco_string_create_char(t->val)));
			//Obj * float_type = tab->find("float");
			*x = *new Operand(val);
			x->type =tab->find("float")->type;
			x->val = val;
			
			cout<<"number float: kind "<<x->kind<<" type kind: "<<x->type->kind<<endl;
			//code->load(x);
			cout<<"number float: kind "<<x->kind<<" type kind: "<<x->type->kind<<endl;
			
		} else if (la->kind == _number) {
			Get();
			int val = 0; string str_val = coco_string_create_char(t->val);
			val = stoi(str_val);
			
			*x = *new Operand(val); 
			x->type =tab->find("int")->type;
			cout<<"number: kind "<<x->kind<<" type kind: "<<x->type->kind<<endl;
			//code->load(x);
			cout<<"number: kind "<<x->kind<<" type kind: "<<x->type->kind<<endl;
			
		} else if (la->kind == _stringConst) {
			Get();
			int index = code->string_set->put(onlyContent(string(coco_string_create_char(t->val))));
			//*x = *new Operand(index);
			*x = *new Operand(index);
			x->type = tab->find("string")->type;
			
			//x->type = tab->find("string")->type;
			
			code->put(LDCS);
			code->put(index);
			x->kind = Operand::Stack;
			x->val = index;
			cout<<"regiter: "<<coco_string_create_char(t->val)<<" index: "<<index<<"type: "<<x->type->kind<<endl;
			
		} else if (la->kind == _new) {
			Get();
			Expect(_ident);
			string name = coco_string_create_char(t->val);
			Obj * obj = tab->find(name);
			cout<<"new obj of: "<<obj->name<<" type: "<<obj->type->kind<<endl;
			
			cout <<"      -------- next : "<<la->kind<<endl; 
			if(la->kind == _popen){
			 cout<<"se creara una instacia"<<endl;
			 code->put(NEW); 
			 code->put(obj->type->nFields); 
			}
			
			if (la->kind == _bopen || la->kind == _popen) {
				if (la->kind == _bopen) {
					Get();
					Expr(y);
					Expect(_bclose);
					code->load(y); 
					code->put(ALC); 
					
					Struct * tmp = new Struct(Struct::Arr,obj->type); 
					cout<<"new array: kind : "<<tmp->kind <<", elemType->kind: "<<tmp->elemType->kind<<endl;
					
				} else {
					ActPars(y);
					Obj * fld = tab->findField("construct",obj->type);
					
					if(fld != SymbolTable::noObj){
					
					 *x = *new Operand(fld);
					 x->kind = Operand::Meth;
					
					
					 //code->load(x);
					 code->put(ICONST); 
					 code->put(x->obj->adr); /*code->load(x); */ 
					 code->put(CALL_S);
					
					 cout<<"---------- la clase "<<name<<" tiene constructor"<<endl;
					
					}else{
					 cout<<"---------- la clase "<<name<<" no tiene constructor"<<endl;
					
					}
					cout<<" new constructor"<<endl; 
					
				}
			}
			*x = *new Operand(0); x->obj = obj; x->kind = Operand::Stack; x->type = obj->type;  
		} else if (la->kind == _sinte) {
			Get();
			Expr(x);
			Operand * tmp; int top = 0;  int adr = 0; int adr2 = 0; 
			RelationExpr(x);
			adr = CodeGenerator::pc-1;
			//hacemos q op x sea un operando normal y no de relacion
			tmp = x;
			
			Expr(y);
			*x= *y;
			x->fromExprType = Operand::OneExpr;   
			x->relaExprSub = tmp->relaExprSub;
			x->relaExprOp = tmp->relaExprOp;
			code->load(x);
			Expect(40 /* ":" */);
			code->put(JMP); code->put(0);  
			adr2 = CodeGenerator::pc-1; 
			code->fixup(adr);
			
			Expr(y);
			*x= *y;
			x->fromExprType = Operand::OneExpr;   
			x->relaExprSub = tmp->relaExprSub;
			x->relaExprOp = tmp->relaExprOp;
			code->load(x); 
			code->fixup(adr2); 
		} else if (la->kind == _popen) {
			Get();
			Expr(x);
			Expect(_pclose);
		} else SynErr(86);
}

void Parser::Mulop(int & op) {
		if (la->kind == 43 /* "*" */) {
			Get();
			op = 3; 
		} else if (la->kind == 44 /* "/" */) {
			Get();
			op = 4; 
		} else if (la->kind == 45 /* "%" */) {
			Get();
			op = 5; 
		} else SynErr(87);
}

void Parser::ASM() {
		StatementSequence();
}

void Parser::StatementSequence() {
		while (StartOf(8)) {
			McodeStatement();
		}
}

void Parser::McodeStatement() {
		int addres; 
		if (StartOf(9)) {
			Executable();
		} else if (StartOf(10)) {
			Directive(addres);
		} else SynErr(88);
}

void Parser::Executable() {
		int op,addres; op=0,addres = 0; 
		if (la->kind == _ident) {
			Label();
		}
		if (StartOf(11)) {
			if (StartOf(12)) {
				OneByteOp(op);
				code->put(op); 
			} else {
				TwoByteOp(op);
				Address(addres);
				code->put(op); code->put(addres); 
			}
		}
}

void Parser::Directive(int & addres) {
		if (la->kind == _ident) {
			Label();
			Expect(63 /* "EQU" */);
			KnownAddress(addres);
		} else if (la->kind == _ident || la->kind == 64 /* "DC" */ || la->kind == 65 /* "DS" */) {
			if (la->kind == _ident) {
				Label();
			}
			if (la->kind == 64 /* "DC" */) {
				Get();
				Address(addres);
			} else if (la->kind == 65 /* "DS" */) {
				Get();
				KnownAddress(addres);
			} else SynErr(89);
		} else if (la->kind == 66 /* "ORG" */) {
			Get();
			KnownAddress(addres);
		} else if (la->kind == 67 /* "BEG" */) {
			Get();
		} else SynErr(90);
}

void Parser::Label() {
		Expect(_ident);
}

void Parser::OneByteOp(int & op) {
		if (la->kind == 54 /* "ADD" */) {
			Get();
			op = ADD; 
		} else if (la->kind == 55 /* "PSH" */) {
			Get();
		} else if (la->kind == 56 /* "POP" */) {
			Get();
		} else if (la->kind == 57 /* "CALL" */) {
			Get();
			op = CALL_S; 
		} else if (la->kind == 58 /* "ALEN" */) {
			Get();
			op=ALEN; 
		} else SynErr(91);
}

void Parser::TwoByteOp(int & op) {
		if (la->kind == 59 /* "LDA" */) {
			Get();
			op = ILOADL; 
		} else if (la->kind == 60 /* "STORE" */) {
			Get();
			op = ISTOREL; 
		} else if (la->kind == 61 /* "GOTO" */) {
			Get();
		} else if (la->kind == 62 /* "ICONST" */) {
			Get();
			op = ICONST; 
		} else SynErr(92);
}

void Parser::Address(int & addres) {
		if (la->kind == _ident) {
			Label();
		} else if (la->kind == _number) {
			Get();
			string str_val = coco_string_create_char(t->val);
			addres = stoi(str_val);
			
		} else SynErr(93);
}

void Parser::KnownAddress(int & addres) {
		Address(addres);
}

void Parser::Load() {
		if (la->kind == 68 /* "@>" */) {
			Get();
		} else if (la->kind == 69 /* "@include" */) {
			Get();
		} else SynErr(94);
		Expect(_stringConst);
		string i_file(coco_string_create_char(t->val)); cout<<"include: "<<i_file<<endl; 
		((type_f) _p)(onlyContent(i_file)); 
		Expect(_colon);
}




// If the user declared a method Init and a mehtod Destroy they should
// be called in the contructur and the destructor respctively.
//
// The following templates are used to recognize if the user declared
// the methods Init and Destroy.

template<typename T>
struct ParserInitExistsRecognizer {
	template<typename U, void (U::*)() = &U::Init>
	struct ExistsIfInitIsDefinedMarker{};

	struct InitIsMissingType {
		char dummy1;
	};
	
	struct InitExistsType {
		char dummy1; char dummy2;
	};

	// exists always
	template<typename U>
	static InitIsMissingType is_here(...);

	// exist only if ExistsIfInitIsDefinedMarker is defined
	template<typename U>
	static InitExistsType is_here(ExistsIfInitIsDefinedMarker<U>*);

	enum { InitExists = (sizeof(is_here<T>(NULL)) == sizeof(InitExistsType)) };
};

template<typename T>
struct ParserDestroyExistsRecognizer {
	template<typename U, void (U::*)() = &U::Destroy>
	struct ExistsIfDestroyIsDefinedMarker{};

	struct DestroyIsMissingType {
		char dummy1;
	};
	
	struct DestroyExistsType {
		char dummy1; char dummy2;
	};

	// exists always
	template<typename U>
	static DestroyIsMissingType is_here(...);

	// exist only if ExistsIfDestroyIsDefinedMarker is defined
	template<typename U>
	static DestroyExistsType is_here(ExistsIfDestroyIsDefinedMarker<U>*);

	enum { DestroyExists = (sizeof(is_here<T>(NULL)) == sizeof(DestroyExistsType)) };
};

// The folloing templates are used to call the Init and Destroy methods if they exist.

// Generic case of the ParserInitCaller, gets used if the Init method is missing
template<typename T, bool = ParserInitExistsRecognizer<T>::InitExists>
struct ParserInitCaller {
	static void CallInit(T *t) {
		// nothing to do
	}
};

// True case of the ParserInitCaller, gets used if the Init method exists
template<typename T>
struct ParserInitCaller<T, true> {
	static void CallInit(T *t) {
		t->Init();
	}
};

// Generic case of the ParserDestroyCaller, gets used if the Destroy method is missing
template<typename T, bool = ParserDestroyExistsRecognizer<T>::DestroyExists>
struct ParserDestroyCaller {
	static void CallDestroy(T *t) {
		// nothing to do
	}
};

// True case of the ParserDestroyCaller, gets used if the Destroy method exists
template<typename T>
struct ParserDestroyCaller<T, true> {
	static void CallDestroy(T *t) {
		t->Destroy();
	}
};

void Parser::Parse() {
	t = NULL;
	la = dummyToken = new Token();
	la->val = coco_string_create(L"Dummy Token");
	Get();
	Program();
	Expect(0);
}

Parser::Parser(Scanner *scanner) {
	maxT = 72;

	ParserInitCaller<Parser>::CallInit(this);
	dummyToken = NULL;
	t = la = NULL;
	minErrDist = 2;
	errDist = minErrDist;
	this->scanner = scanner;
	errors = new Errors(this->scanner);
}

bool Parser::StartOf(int s) {
	const bool T = true;
	const bool x = false;

	static bool set[13][74] = {
		{T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,T,x,x, x,x,T,x, x,x,x,x, x,T,x,x, x,x,T,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,T,T, x,x},
		{x,T,x,x, x,x,T,x, x,x,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,T,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,T,x,x, x,x,x,T, x,T,T,T, T,x,x,x, x,T,T,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,T,x,x, x,x,x,T, T,T,T,T, T,x,x,x, x,T,T,T, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,T,T,x, T,T,x,x, x,x,x,x, x,x,T,x, x,x,x,x, x,x,T,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,T,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,T,T,T, T,T,T,x, T,T,T,T, x,x,x,x, x,x},
		{x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,T,T, T,T,T,T, T,T,T,x, T,T,T,T, x,x,x,x, x,x},
		{x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,T,T, x,x,x,x, x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,T,T,T, T,T,T,x, x,x,x,x, x,x,x,x, x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,T,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x}
	};



	return set[s][la->kind];
}

Parser::~Parser() {
	ParserDestroyCaller<Parser>::CallDestroy(this);
	delete errors;
	delete dummyToken;
}

Errors::Errors() {
	count = 0;
}

Errors::Errors(Scanner *scanner)	{ 
	count = 0;
	this->scanner = scanner;
}

void Errors::printMsg(int line, int col, const wchar_t *s){
	string buff = "";
	int b,li;
	li = 1;
	scanner->buffer->SetPos(0);
	while(true){
	
		b =  scanner->buffer->Read(); 
		if(b == scanner->buffer->EoF)
		 	break;
		else{
			buff.append(string((char*)&b));
			if(b==10){

			
				if(li==line){

						printf(" <-- linea %d columna %d: %ls\n", line, col, s);

				}
				if(li >= line-2 && li <= line+2)
					cout<<endl;
				
				li++;
			}else{
				if(li >= line-2 && li <= line+2)
					cout<<(char)b;
			}
		}
	}
	
			
	//printf("%ls\n",	scanner->buffer->GetString(scanner->buffer->GetPos()-(line+col),scanner->buffer->GetPos()+(line+col)));
	
	//cout<<buff<<endl;
}

void Errors::SynErr(int line, int col, int n) {
	wchar_t* s;
	switch (n) {
			case 0: s = coco_string_create(L"EOF expected"); break;
			case 1: s = coco_string_create(L"ident expected"); break;
			case 2: s = coco_string_create(L"number expected"); break;
			case 3: s = coco_string_create(L"charConst expected"); break;
			case 4: s = coco_string_create(L"stringConst expected"); break;
			case 5: s = coco_string_create(L"floatLit expected"); break;
			case 6: s = coco_string_create(L"class expected"); break;
			case 7: s = coco_string_create(L"if expected"); break;
			case 8: s = coco_string_create(L"else expected"); break;
			case 9: s = coco_string_create(L"while expected"); break;
			case 10: s = coco_string_create(L"print expected"); break;
			case 11: s = coco_string_create(L"return expected"); break;
			case 12: s = coco_string_create(L"void expected"); break;
			case 13: s = coco_string_create(L"final expected"); break;
			case 14: s = coco_string_create(L"new expected"); break;
			case 15: s = coco_string_create(L"package expected"); break;
			case 16: s = coco_string_create(L"comma expected"); break;
			case 17: s = coco_string_create(L"colon expected"); break;
			case 18: s = coco_string_create(L"llopen expected"); break;
			case 19: s = coco_string_create(L"llclose expected"); break;
			case 20: s = coco_string_create(L"bopen expected"); break;
			case 21: s = coco_string_create(L"bclose expected"); break;
			case 22: s = coco_string_create(L"popen expected"); break;
			case 23: s = coco_string_create(L"pclose expected"); break;
			case 24: s = coco_string_create(L"sinte expected"); break;
			case 25: s = coco_string_create(L"EOL expected"); break;
			case 26: s = coco_string_create(L"comment expected"); break;
			case 27: s = coco_string_create(L"\"=\" expected"); break;
			case 28: s = coco_string_create(L"\"fun\" expected"); break;
			case 29: s = coco_string_create(L"\"native\" expected"); break;
			case 30: s = coco_string_create(L"\"==\" expected"); break;
			case 31: s = coco_string_create(L"\"!=\" expected"); break;
			case 32: s = coco_string_create(L"\">\" expected"); break;
			case 33: s = coco_string_create(L"\">=\" expected"); break;
			case 34: s = coco_string_create(L"\"<\" expected"); break;
			case 35: s = coco_string_create(L"\"<=\" expected"); break;
			case 36: s = coco_string_create(L"\"||\" expected"); break;
			case 37: s = coco_string_create(L"\"&&\" expected"); break;
			case 38: s = coco_string_create(L"\"-\" expected"); break;
			case 39: s = coco_string_create(L"\"!\" expected"); break;
			case 40: s = coco_string_create(L"\":\" expected"); break;
			case 41: s = coco_string_create(L"\".\" expected"); break;
			case 42: s = coco_string_create(L"\"+\" expected"); break;
			case 43: s = coco_string_create(L"\"*\" expected"); break;
			case 44: s = coco_string_create(L"\"/\" expected"); break;
			case 45: s = coco_string_create(L"\"%\" expected"); break;
			case 46: s = coco_string_create(L"\"&\" expected"); break;
			case 47: s = coco_string_create(L"\"|\" expected"); break;
			case 48: s = coco_string_create(L"\"^\" expected"); break;
			case 49: s = coco_string_create(L"\"<<\" expected"); break;
			case 50: s = coco_string_create(L"\">>\" expected"); break;
			case 51: s = coco_string_create(L"\"mcode\" expected"); break;
			case 52: s = coco_string_create(L"\"begin\" expected"); break;
			case 53: s = coco_string_create(L"\"end\" expected"); break;
			case 54: s = coco_string_create(L"\"ADD\" expected"); break;
			case 55: s = coco_string_create(L"\"PSH\" expected"); break;
			case 56: s = coco_string_create(L"\"POP\" expected"); break;
			case 57: s = coco_string_create(L"\"CALL\" expected"); break;
			case 58: s = coco_string_create(L"\"ALEN\" expected"); break;
			case 59: s = coco_string_create(L"\"LDA\" expected"); break;
			case 60: s = coco_string_create(L"\"STORE\" expected"); break;
			case 61: s = coco_string_create(L"\"GOTO\" expected"); break;
			case 62: s = coco_string_create(L"\"ICONST\" expected"); break;
			case 63: s = coco_string_create(L"\"EQU\" expected"); break;
			case 64: s = coco_string_create(L"\"DC\" expected"); break;
			case 65: s = coco_string_create(L"\"DS\" expected"); break;
			case 66: s = coco_string_create(L"\"ORG\" expected"); break;
			case 67: s = coco_string_create(L"\"BEG\" expected"); break;
			case 68: s = coco_string_create(L"\"@>\" expected"); break;
			case 69: s = coco_string_create(L"\"@include\" expected"); break;
			case 70: s = coco_string_create(L"\"@>l\" expected"); break;
			case 71: s = coco_string_create(L"\"@load_lib\" expected"); break;
			case 72: s = coco_string_create(L"??? expected"); break;
			case 73: s = coco_string_create(L"invalid LibLoad"); break;
			case 74: s = coco_string_create(L"invalid ConstDecl"); break;
			case 75: s = coco_string_create(L"invalid FuncDecl"); break;
			case 76: s = coco_string_create(L"invalid NativeDecl"); break;
			case 77: s = coco_string_create(L"invalid MethodDecl"); break;
			case 78: s = coco_string_create(L"invalid Statement"); break;
			case 79: s = coco_string_create(L"invalid Statement"); break;
			case 80: s = coco_string_create(L"invalid Statement"); break;
			case 81: s = coco_string_create(L"invalid Statement"); break;
			case 82: s = coco_string_create(L"invalid Relop"); break;
			case 83: s = coco_string_create(L"invalid Addop"); break;
			case 84: s = coco_string_create(L"invalid Bitop"); break;
			case 85: s = coco_string_create(L"invalid BitShiftop"); break;
			case 86: s = coco_string_create(L"invalid Factor"); break;
			case 87: s = coco_string_create(L"invalid Mulop"); break;
			case 88: s = coco_string_create(L"invalid McodeStatement"); break;
			case 89: s = coco_string_create(L"invalid Directive"); break;
			case 90: s = coco_string_create(L"invalid Directive"); break;
			case 91: s = coco_string_create(L"invalid OneByteOp"); break;
			case 92: s = coco_string_create(L"invalid TwoByteOp"); break;
			case 93: s = coco_string_create(L"invalid Address"); break;
			case 94: s = coco_string_create(L"invalid Load"); break;

		default:
		{
			wchar_t format[20];
			coco_swprintf(format, 20, L"error %d", n);
			s = coco_string_create(format);
		}
		break;
	}
	printMsg(line, col, s);
	coco_string_delete(s);
	count++;
}

void Errors::Error(int line, int col, const wchar_t *s) {
	
	printMsg(line, col, s);
	count++;
}

void Errors::Warning(int line, int col, const wchar_t *s) {
	printMsg(line, col, s);
}
void Errors::Warning(const wchar_t *s) {
	printf("%ls\n", s);
}

void Errors::Exception(const wchar_t* s) {
	printf("%ls", s); 
	exit(1);
}

} // namespace

