

#if !defined(PARSER_PARAMS_COCO_PARSER_H__)
#define PARSER_PARAMS_COCO_PARSER_H__

#include<iostream>
#include "params.h"
using namespace std;
using namespace DVLANG::COMPILER_;


#include "Scanner.h"

namespace PARSER_PARAMS {


class Errors {
public:
	int count;			// number of errors detected

	Errors();
	void SynErr(int line, int col, int n);
	void Error(int line, int col, const wchar_t *s);
	void Warning(int line, int col, const wchar_t *s);
	void Warning(const wchar_t *s);
	void Exception(const wchar_t *s);

}; // Errors

class Parser {
private:
	enum {
		_EOF=0,
		_ident=1,
		_number=2,
		_charConst=3,
		_stringConst=4,
		_floatLit=5
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

string onlyContent(string str){
    return str.substr(1,str.length()-2);
  }


  Params * params;



	Parser(Scanner *scanner);
	~Parser();
	void SemErr(const wchar_t* msg);

	void _Params_();
	void Run();
	void Input();
	void Expr();
	void Out();
	void Format();
	void Debug();

	void Parse();

}; // end Parser

} // namespace


#endif

