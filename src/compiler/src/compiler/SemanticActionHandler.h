#ifndef SEMANTIC_ACTION_HANDLER_H
#define SEMANTIC_ACTION_HANDLER_H

#include <string>
#include <vector>
#include <memory>
#include "generate/ICodeGenerator.h"
#include "generate/symbol_table/symbol_table.h"
// Forward declare Obj if needed, or include specific types

namespace DVLANG {
namespace COMPILER_ {

// Forward declare Parser if SemanticActionHandler needs to interact with it beyond cg and tab
// namespace PARSER { class Parser; }

class SemanticActionHandler {
public:
    SemanticActionHandler(GENERATE::ICodeGenerator* code_gen, GENERATE::SymbolTable* symbol_tab);

    // Placeholder for methods that will be called from grammar actions.
    // These methods will encapsulate the logic previously in .atg files.

    // Example: Handling a numeric literal found in Factor
    // GENERATE::AbstractOperand* handleNumericLiteral(const std::string& value_str, bool is_float);

    // Example: Handling an assignment statement
    // void handleAssignment(GENERATE::AbstractOperand* target, GENERATE::AbstractOperand* source);

    // Example: Start of an IF statement condition evaluation
    // void handleIfConditionStart();
    // void handleIfConditionEnd(int jump_to_else_label_id);

    // More methods will be added here corresponding to grammar rules
    // like Factor, Term, Expr, Statement types, Designator resolution, etc.

private:
    GENERATE::ICodeGenerator* cg_; // The code generator instance
    GENERATE::SymbolTable*  tab_; // The symbol table instance
    // Potentially keep track of current method, scope, etc., if needed by actions
    // GENERATE::Obj* current_method_;
};

} // namespace COMPILER_
} // namespace DVLANG

#endif // SEMANTIC_ACTION_HANDLER_H
