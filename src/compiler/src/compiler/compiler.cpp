

#include "compiler.h"
//#include <native.cpp>

using namespace PARSER;
using namespace std;

static DVLANG::COMPILER_::Compiler *c;

namespace DVLANG
{
namespace COMPILER_
{
Compiler::Compiler(){

};

Compiler::Compiler(int argc, char **args)
{
    c = this;
    string all = "";
    for (int x = 0; x < argc - 1; x++)
    {

        string p(args[x + 1]);
        if (p.substr(0, 1) == string("-"))
        {
            cout << "append arg: " << args[x + 1] << endl;
            all.append(" ").append(string(args[x + 1])).append(" ");
        }
        else
        {
            cout << "append arg: \"" << args[x + 1] << "\"" << endl;
            all.append("\"").append(string(args[x + 1])).append("\" ");
        }
    }

    cout << "all: " << all << endl;

    PARSER_PARAMS::Scanner *s_params = new PARSER_PARAMS::Scanner((const unsigned char *)all.c_str(), all.length());
    PARSER_PARAMS::Parser *p_params = new PARSER_PARAMS::Parser(s_params);

    p_params->params = new Params("nofile", "out.dvc");
    p_params->Parse();

    if (p_params->errors->count == 0)
    {
        //compile
        if (p_params->params->input == "nofile")
        {
            //no compile
            cout << "Falta el programa de entrada.";
        }
        else
        {
            //compile
            cout << "input: " << p_params->params->input << endl;
            cout << "output: " << p_params->params->output << endl;

            /* wchar_t *fileName = coco_string_create(p_params->params->input.c_str());
                    Scanner *scanner = new Scanner(fileName);
                    Parser *parser = new Parser(scanner); */

            tab = new DVLANG::COMPILER_::GENERATE::SymbolTable();
            code = new DVLANG::COMPILER_::GENERATE::CodeGenerator();

            PARSER::Parser * p = parse(p_params->params->input);

            CodeGenerator::globals = SymbolTable::globals;

            p->code->write(p_params->params->output,tab->lib_symbols);
           
            //tab->lib_symbols->dump();
        }
    }
}

void _parse(string fn) throw()
{
    c->parse(fn);
}

PARSER::Parser *Compiler::parse(string filename)
{
    wchar_t *fileName = coco_string_create(filename.c_str());
    Scanner *scanner = new Scanner(fileName);
    Parser *parser = new Parser(scanner);

    parser->tab = tab;
    parser->code = code;

    parser->_p = (void *)_parse;

    parser->Parse();

    if (parser->errors->count > 0)
    {
        cout << "errores de compilacion: " << parser->errors->count << " en: " << filename << endl;
        string err = string("errores de compiacion: ").append(to_string(parser->errors->count));
        throw err.c_str();
    }
   
    return parser;
}

Compiler::Compiler(char *filename)
{

    wchar_t *fileName = coco_string_create(filename);
    Scanner *scanner = new Scanner(fileName);
    Parser *parser = new Parser(scanner);
    parser->tab = new DVLANG::COMPILER_::GENERATE::SymbolTable();

    parser->Parse();
    cout << "errores: " << parser->errors->count << endl;
};
} // namespace COMPILER_
} // namespace DVLANG
