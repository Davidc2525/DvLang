

#include "compiler.h"
#include "generate/DvVmCodeGenerator.h" // Added
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
            // code = new DVLANG::COMPILER_::GENERATE::CodeGenerator(); // Old
            code_generator_ = std::make_unique<DVLANG::COMPILER_::GENERATE::DvVmCodeGenerator>();
            code_generator_->initialize(tab);


            PARSER::Parser * p = parse(p_params->params->input); // This will set p->code via Compiler::parse

            // CodeGenerator::globals = SymbolTable::globals; // Removed - handled by DvVmCodeGenerator internally if needed

            // p->code points to code_generator_ (if Compiler::parse is updated correctly)
            // So, using code_generator_ directly is clearer.
            code_generator_->writeOutput(p_params->params->output, tab->lib_symbols);
           
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
    // Parser constructor will need to accept ICodeGenerator*
    Parser *parser = new Parser(scanner);

    parser->tab = tab; // Assign compiler's tab
    parser->code = code_generator_.get(); // Assign compiler's code generator instance

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
    // Initialize Compiler's own symbol table and code generator
    this->tab = new DVLANG::COMPILER_::GENERATE::SymbolTable();
    this->code_generator_ = std::make_unique<DVLANG::COMPILER_::GENERATE::DvVmCodeGenerator>();
    this->code_generator_->initialize(this->tab);

    // The Scanner now takes const char* filename directly in its constructor typically.
    // If Scanner still needs wchar_t*, coco_string_create is fine.
    // Assuming Scanner can handle `filename` directly or via coco_string_create if necessary.
    wchar_t *wc_fileName = coco_string_create(filename); // Keep if Scanner needs wchar_t*
    Scanner *scanner = new Scanner(wc_fileName); // Or new Scanner(filename) if API changed

    Parser *parser = new Parser(scanner);
    parser->tab = this->tab; // Parser uses Compiler's symbol table
    parser->code = this->code_generator_.get(); // Parser uses Compiler's code generator

    // It seems _parse assignment might be missing here compared to the other parse method
    // parser->_p = (void *)_parse; // If this constructor also supports include directives

    parser->Parse();
    cout << "errores: " << parser->errors->count << endl;

    if (parser->errors->count == 0) {
        // Determine output filename, e.g., input filename + ".dvc"
        string output_filename = string(filename) + ".dvc";
        this->code_generator_->writeOutput(output_filename, this->tab->lib_symbols);
        cout << "Compilation successful. Output: " << output_filename << endl;
    } else {
        cout << "Compilation failed." << endl;
    }
    // coco_string_delete(wc_fileName); // Clean up wchar_t string if created
};
} // namespace COMPILER_
} // namespace DVLANG
