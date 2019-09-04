

#include <wchar.h>
#include "Parser.h"
#include "Scanner.h"


namespace PARSER_PARAMS {


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

void Parser::_Params_() {
		if (la->kind == 16 /* "-run" */) {
			Run();
		}
		Input();
		while (StartOf(1)) {
			Expr();
		}
}

void Parser::Run() {
		Expect(16 /* "-run" */);
		params->run = true; 
}

void Parser::Input() {
		if (la->kind == 6 /* "-i" */) {
			Get();
		}
		Expect(_stringConst);
		params->input = onlyContent(coco_string_create_char(t->val)); 
}

void Parser::Expr() {
		if (la->kind == 7 /* "-o" */) {
			Out();
		} else if (la->kind == 12 /* "-type" */) {
			Format();
		} else if (la->kind == 8 /* "-d" */) {
			Debug();
		} else if (la->kind == 16 /* "-run" */) {
			Run();
		} else SynErr(18);
}

void Parser::Out() {
		Expect(7 /* "-o" */);
		if (la->kind == _stringConst) {
			Get();
			params->output = onlyContent(coco_string_create_char(t->val));
			
		}
}

void Parser::Format() {
		Expect(12 /* "-type" */);
		if (la->kind == 13 /* "json" */) {
			Get();
		} else if (la->kind == 14 /* "protobuf" */) {
			Get();
		} else if (la->kind == 15 /* "plaint" */) {
			Get();
		} else SynErr(19);
}

void Parser::Debug() {
		Expect(8 /* "-d" */);
		if (la->kind == 9 /* "ALL" */) {
			Get();
		} else if (la->kind == 10 /* "WARN" */) {
			Get();
		} else if (la->kind == 11 /* "ERR" */) {
			Get();
		} else SynErr(20);
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
	_Params_();
	Expect(0);
}

Parser::Parser(Scanner *scanner) {
	maxT = 17;

	ParserInitCaller<Parser>::CallInit(this);
	dummyToken = NULL;
	t = la = NULL;
	minErrDist = 2;
	errDist = minErrDist;
	this->scanner = scanner;
	errors = new Errors();
}

bool Parser::StartOf(int s) {
	const bool T = true;
	const bool x = false;

	static bool set[2][19] = {
		{T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x},
		{x,x,x,x, x,x,x,T, T,x,x,x, T,x,x,x, T,x,x}
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

void Errors::SynErr(int line, int col, int n) {
	wchar_t* s;
	switch (n) {
			case 0: s = coco_string_create(L"EOF expected"); break;
			case 1: s = coco_string_create(L"ident expected"); break;
			case 2: s = coco_string_create(L"number expected"); break;
			case 3: s = coco_string_create(L"charConst expected"); break;
			case 4: s = coco_string_create(L"stringConst expected"); break;
			case 5: s = coco_string_create(L"floatLit expected"); break;
			case 6: s = coco_string_create(L"\"-i\" expected"); break;
			case 7: s = coco_string_create(L"\"-o\" expected"); break;
			case 8: s = coco_string_create(L"\"-d\" expected"); break;
			case 9: s = coco_string_create(L"\"ALL\" expected"); break;
			case 10: s = coco_string_create(L"\"WARN\" expected"); break;
			case 11: s = coco_string_create(L"\"ERR\" expected"); break;
			case 12: s = coco_string_create(L"\"-type\" expected"); break;
			case 13: s = coco_string_create(L"\"json\" expected"); break;
			case 14: s = coco_string_create(L"\"protobuf\" expected"); break;
			case 15: s = coco_string_create(L"\"plaint\" expected"); break;
			case 16: s = coco_string_create(L"\"-run\" expected"); break;
			case 17: s = coco_string_create(L"??? expected"); break;
			case 18: s = coco_string_create(L"invalid Expr"); break;
			case 19: s = coco_string_create(L"invalid Format"); break;
			case 20: s = coco_string_create(L"invalid Debug"); break;

		default:
		{
			wchar_t format[20];
			coco_swprintf(format, 20, L"error %d", n);
			s = coco_string_create(format);
		}
		break;
	}
	printf("-- line %d col %d: %ls\n", line, col, s);
	coco_string_delete(s);
	count++;
}

void Errors::Error(int line, int col, const wchar_t *s) {
	printf("-- line %d col %d: %ls\n", line, col, s);
	count++;
}

void Errors::Warning(int line, int col, const wchar_t *s) {
	printf("-- line %d col %d: %ls\n", line, col, s);
}

void Errors::Warning(const wchar_t *s) {
	printf("%ls\n", s);
}

void Errors::Exception(const wchar_t* s) {
	printf("%ls", s); 
	exit(1);
}

} // namespace

