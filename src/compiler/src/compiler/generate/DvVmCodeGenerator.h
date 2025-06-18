#ifndef DV_VM_CODE_GENERATOR_H
#define DV_VM_CODE_GENERATOR_H

#include "ICodeGenerator.h"
#include <vector>
#include <string>
#include <map>
#include <process/Instructions.h>
#include <program/program.h>
#include <process/strings_sets.h>
#include "../symbol_table/symbol_table.h" // For SymbolTable, Struct, Obj

// Forward declare DVLANG::PROGRAM::LibSymbolStore if full definition isn't needed here
// However, it's used in writeOutput, so a full include might be necessary in the .cpp
// For now, a forward declaration is fine for the header.
namespace DVLANG {
    namespace PROGRAM {
        class LibSymbolStore;
    }
}

namespace DVLANG {
namespace COMPILER_ {
namespace GENERATE {

// Concrete Operand for DvVm
class DvVmOperand : public AbstractOperand {
public:
    // Using the same kind definitions as the old Operand class for now
    // This might be refactored if these kinds are too DvVM specific
    enum DvVmOperandKind {
        Con = 0, Local = 1, Static = 2, Stack = 3,
        Fld = 4, Elem = 5, Meth = 6, Func = 7
    };

    DvVmOperandKind kind; // Con, Local, Static, ...
    Struct* type;         // Type of the operand
    intdv val;            // Con: constant value
    intdv adr;            // Local, Static, Fld, Meth: address
    Obj* obj;             // Meth: method object
    int co_call;          // TODO: Understand and integrate this properly

    // Expression related fields from old Operand - might be needed for complex emit logic
    // int fromExprType;
    // int relaExprSub;
    // int relaExprOp;

    explicit DvVmOperand(Obj* o) : kind(Meth), type(o->type), val(0), adr(o->adr), obj(o), co_call(0) {}
    explicit DvVmOperand(intdv v) : kind(Con), type(SymbolTable::intType), val(v), adr(0), obj(nullptr), co_call(0) {}
    DvVmOperand(DvVmOperandKind k, Struct* t, intdv address) : kind(k), type(t), val(0), adr(address), obj(nullptr), co_call(0) {}
    // Add more constructors as needed for other kinds
};

class DvVmCodeGenerator : public ICodeGenerator {
public:
    DvVmCodeGenerator();
    ~DvVmCodeGenerator() override;

    // --- Initialization and Output ---
    void initialize(DVLANG::COMPILER_::GENERATE::SymbolTable* symbolTable) override;
    void writeOutput(const std::string& filename, DVLANG::PROGRAM::LibSymbolStore* libs) override;

    // --- Operand Creation ---
    AbstractOperand* createConstantIntegerOperand(intdv value) override;
    AbstractOperand* createConstantDoubleOperand(double value) override; // Requires handling double representation
    AbstractOperand* createConstantStringOperand(const std::string& stringValue) override;
    AbstractOperand* createLocalVariableOperand(int offset, Struct* type) override;
    AbstractOperand* createGlobalVariableOperand(int address, Struct* type) override;
    AbstractOperand* createFieldOperand(AbstractOperand* baseAddress, int fieldOffset, Struct* fieldType) override;
    AbstractOperand* createArrayElementOperand(AbstractOperand* baseAddress, AbstractOperand* indexOperand, Struct* elementType) override;
    AbstractOperand* createFunctionNameOperand(const std::string& functionName) override; // This might just store the name or an address

    // --- Instruction Emission: Semantic Actions ---
    void emitAssign(AbstractOperand* target, AbstractOperand* source) override;
    void emitAdd(AbstractOperand* op1, AbstractOperand* op2, AbstractOperand* resultTarget) override;
    void emitSubtract(AbstractOperand* op1, AbstractOperand* op2, AbstractOperand* resultTarget) override;
    void emitMultiply(AbstractOperand* op1, AbstractOperand* op2, AbstractOperand* resultTarget) override;
    void emitDivide(AbstractOperand* op1, AbstractOperand* op2, AbstractOperand* resultTarget) override;
    void emitModulo(AbstractOperand* op1, AbstractOperand* op2, AbstractOperand* resultTarget) override;
    void emitNegate(AbstractOperand* source, AbstractOperand* resultTarget) override;

    void emitAnd(AbstractOperand* op1, AbstractOperand* op2, AbstractOperand* resultTarget) override;
    void emitOr(AbstractOperand* op1, AbstractOperand* op2, AbstractOperand* resultTarget) override;
    void emitNot(AbstractOperand* source, AbstractOperand* resultTarget) override;

    int  createLabel() override;
    void placeLabel(int label) override;
    void emitJump(int label) override;
    void emitCompare(AbstractOperand* op1, AbstractOperand* op2) override;
    void emitJumpIfEqual(int label) override;
    void emitJumpIfNotEqual(int label) override;
    void emitJumpIfLessThan(int label) override;
    void emitJumpIfLessThanOrEqual(int label) override;
    void emitJumpIfGreaterThan(int label) override;
    void emitJumpIfGreaterThanOrEqual(int label) override;

    void emitFunctionPrologue(const std::string& functionName, int paramCount, int localVariableSpace) override;
    void emitFunctionEpilogue(const std::string& functionName) override;
    void emitCall(AbstractOperand* functionOperand, const std::vector<AbstractOperand*>& args, AbstractOperand* returnTarget) override;
    void emitNativeCall(const std::string& nativeFunctionName, int nativeSymbolId, const std::vector<AbstractOperand*>& args, AbstractOperand* returnTarget) override;
    void emitReturn(AbstractOperand* returnValue) override;

    void emitPush(AbstractOperand* operand) override;
    void emitPop(AbstractOperand* targetOperand) override;
    void emitDuplicate() override;

    void emitNewObject(Struct* classType, AbstractOperand* targetAddressOperand) override;
    void emitNewArray(Struct* elementType, AbstractOperand* sizeOperand, AbstractOperand* targetAddressOperand) override;
    void emitArrayLength(AbstractOperand* arrayAddressOperand, AbstractOperand* targetLengthOperand) override;
    void emitHeapFree(AbstractOperand* addressOperand) override;

    void emitConvert(AbstractOperand* sourceOperand, Struct* targetType, AbstractOperand* resultTargetOperand) override;
    void emitLoadConstantStringAddress(int stringIdInTable, AbstractOperand* targetAddressOperand) override;

    int getCurrentCodeOffset() const override;

private:
    // --- Internal Helper Methods (migrated from static CodeGenerator or new) ---
    void put(intdv instruction_opcode);
    void put2(intdv value); // For word-sized operands or addresses
    void put4(intdv value); // For double-word sized operands or addresses (if any)

    void loadOperandToStack(DvVmOperand* op); // Helper to emit instructions to load operand's value onto VM stack
    void storeOperandFromStack(DvVmOperand* op); // Helper to emit instructions to store stack's top value to operand's location

    // --- Data Members (migrated from static CodeGenerator) ---
    DVLANG::COMPILER_::GENERATE::SymbolTable* symTable; // For context, like type info
    STRING_SET::Strings string_set_instance; // Own instance of string table
    std::vector<intdv> program_buffer; // Internal buffer for bytecode
    intdv main_pc;        // Entry point of the main function
    intdv current_pc;     // Current position in program_buffer (Program Counter for generation)
    intdv globals_size;   // Size of global variable area

    int next_label_id;                     // For generating unique label IDs
    std::map<int, intdv> label_addresses;  // Maps label ID to bytecode address
    std::map<int, std::vector<intdv>> label_fixups; // Tracks places where labels need to be patched

    // Helper to get concrete operand type
    DvVmOperand* getConcreteOperand(AbstractOperand* abstractOp);

    // Handling double to long conversion for DvVM
    long doubleToLongBits(double val);
    // Add other type conversion helpers as needed
};

} // namespace GENERATE
} // namespace COMPILER_
} // namespace DVLANG

#endif // DV_VM_CODE_GENERATOR_H
