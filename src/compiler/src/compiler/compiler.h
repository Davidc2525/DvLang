

#ifndef COMPILER_H
#define COMPILER_H

#include "../parser/Parser.h"
#include "../parser/Scanner.h"
#include "../parser_params/params.h"
#include "../parser_params/Scanner.h"
#include "../parser_params/Parser.h"
#include "generate/symbol_table/symbol_table.h"
// #include "generate/code_generator/code_generator.h" // Removed
#include "generate/ICodeGenerator.h" // Added
#include <memory> // Added

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
    std::unique_ptr<DVLANG::COMPILER_::GENERATE::ICodeGenerator> code_generator_; // Changed member
};
}; // namespace COMPILER_
} // namespace DVLANG

#endif