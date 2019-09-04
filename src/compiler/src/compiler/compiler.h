

#ifndef COMPILER_H
#define COMPILER_H

#include "../parser/Parser.h"
#include "../parser/Scanner.h"
#include "../parser_params/params.h"
#include "../parser_params/Scanner.h"
#include "../parser_params/Parser.h"
#include "generate/symbol_table/symbol_table.h"
#include "generate/code_generator/code_generator.h"

namespace DVLANG
{
namespace COMPILER_
{
class Compiler
{
  public:
    Compiler();
    Compiler(char *filename);
    Compiler(int argc,char **args);

    //
    PARSER::Parser * parse(string filename);

  private:
    DVLANG::COMPILER_::GENERATE::SymbolTable * tab; 
    DVLANG::COMPILER_::GENERATE::CodeGenerator * code; 
};
}; // namespace COMPILER
} // namespace DVLANG

#endif