#ifndef I_CODE_GENERATOR_H
#define I_CODE_GENERATOR_H

#include <string>
#include <vector>
#include <memory> // For std::unique_ptr if we decide to return AbstractOperands by unique_ptr

// Forward declarations to minimize direct dependencies in the interface
namespace DVLANG {
    namespace PROGRAM {
        class LibSymbolStore; // Used in writeOutput
    }
    namespace COMPILER_ {
        namespace GENERATE {
            class SymbolTable; // Used in initialize
            class Struct;      // Used for type information
            // class Obj;      // From existing SymbolTable, might be used by concrete generator
        }
    }
}


namespace DVLANG {
namespace COMPILER_ {
namespace GENERATE {

// Abstract representation of an operand.
// Concrete generators will create their own specific operand types derived from this.
// It primarily serves as a type-safe marker for operands passed to/from the interface.
class AbstractOperand {
public:
    virtual ~AbstractOperand() = default;
    // It's up to the concrete generator to know how to cast this back to its specific type
    // and extract information. For the interface, it's an opaque handle.
    // We could add a virtual `getType()` or `getKind()` if universally needed by interface logic,
    // but often the methods themselves imply the type of operand expected.
};

class ICodeGenerator {
public:
    virtual ~ICodeGenerator() = default;

    // --- Initialization and Output ---
    // SymbolTable is passed to give context (e.g., for string literals, types)
    virtual void initialize(DVLANG::COMPILER_::GENERATE::SymbolTable* symbolTable) = 0;
    virtual void writeOutput(const std::string& filename, DVLANG::PROGRAM::LibSymbolStore* libs) = 0;

    // --- Operand Creation (managed by concrete generator) ---
    // The compiler front-end requests the creation of operands.
    // The concrete generator returns an opaque pointer to its internal operand representation.
    virtual AbstractOperand* createConstantIntegerOperand(intdv value) = 0;
    virtual AbstractOperand* createConstantDoubleOperand(double value) = 0;
    virtual AbstractOperand* createConstantStringOperand(const std::string& stringValue) = 0; // Might involve adding to a string table

    // Operands representing memory locations. `type` helps the generator if it needs to know size/type.
    virtual AbstractOperand* createLocalVariableOperand(int offset, DVLANG::COMPILER_::GENERATE::Struct* type) = 0;
    virtual AbstractOperand* createGlobalVariableOperand(int address, DVLANG::COMPILER_::GENERATE::Struct* type) = 0;
    // `baseAddress` is an operand yielding the base address of an object or array.
    virtual AbstractOperand* createFieldOperand(AbstractOperand* baseAddress, int fieldOffset, DVLANG::COMPILER_::GENERATE::Struct* fieldType) = 0;
    virtual AbstractOperand* createArrayElementOperand(AbstractOperand* baseAddress, AbstractOperand* indexOperand, DVLANG::COMPILER_::GENERATE::Struct* elementType) = 0;
    virtual AbstractOperand* createFunctionNameOperand(const std::string& functionName) = 0; // For direct calls

    // --- Instruction Emission: Semantic Actions ---

    // Data movement
    virtual void emitAssign(AbstractOperand* target, AbstractOperand* source) = 0;
    // virtual void emitLoadEffectiveAddress(AbstractOperand* addressOperand, AbstractOperand* target) = 0; // Example: LEA

    // Arithmetic operations (typically result in modifying a target operand or pushing to stack)
    virtual void emitAdd(AbstractOperand* op1, AbstractOperand* op2, AbstractOperand* resultTarget) = 0;
    virtual void emitSubtract(AbstractOperand* op1, AbstractOperand* op2, AbstractOperand* resultTarget) = 0;
    virtual void emitMultiply(AbstractOperand* op1, AbstractOperand* op2, AbstractOperand* resultTarget) = 0;
    virtual void emitDivide(AbstractOperand* op1, AbstractOperand* op2, AbstractOperand* resultTarget) = 0;
    virtual void emitModulo(AbstractOperand* op1, AbstractOperand* op2, AbstractOperand* resultTarget) = 0;
    virtual void emitNegate(AbstractOperand* source, AbstractOperand* resultTarget) = 0;

    // Logical operations
    virtual void emitAnd(AbstractOperand* op1, AbstractOperand* op2, AbstractOperand* resultTarget) = 0;
    virtual void emitOr(AbstractOperand* op1, AbstractOperand* op2, AbstractOperand* resultTarget) = 0;
    virtual void emitNot(AbstractOperand* source, AbstractOperand* resultTarget) = 0; // Logical NOT

    // Control Flow
    virtual int  createLabel() = 0; // Returns an opaque label identifier for future reference
    virtual void placeLabel(int label) = 0; // Marks the current code position with this label
    virtual void emitJump(int label) = 0; // Unconditional jump

    // Comparisons and conditional jumps: result of comparison is often implicit (e.g., flags)
    virtual void emitCompare(AbstractOperand* op1, AbstractOperand* op2) = 0; // Sets internal flags for subsequent conditional jumps
    virtual void emitJumpIfEqual(int label) = 0;
    virtual void emitJumpIfNotEqual(int label) = 0;
    virtual void emitJumpIfLessThan(int label) = 0;
    virtual void emitJumpIfLessThanOrEqual(int label) = 0;
    virtual void emitJumpIfGreaterThan(int label) = 0;
    virtual void emitJumpIfGreaterThanOrEqual(int label) = 0;

    // Function calls
    virtual void emitFunctionPrologue(const std::string& functionName, int paramCount, int localVariableSpace) = 0;
    virtual void emitFunctionEpilogue(const std::string& functionName) = 0;
    // `functionOperand` could be created via `createFunctionNameOperand` or be an address
    virtual void emitCall(AbstractOperand* functionOperand, const std::vector<AbstractOperand*>& args, AbstractOperand* returnTarget /*nullable*/) = 0;
    virtual void emitNativeCall(const std::string& nativeFunctionName, int nativeSymbolId, const std::vector<AbstractOperand*>& args, AbstractOperand* returnTarget /*nullable*/) = 0;
    virtual void emitReturn(AbstractOperand* returnValue /*nullable*/) = 0;

    // Stack manipulation (if target is stack-based or for intermediate calcs)
    virtual void emitPush(AbstractOperand* operand) = 0;
    virtual void emitPop(AbstractOperand* targetOperand /*nullable, if just popping value*/) = 0;
    virtual void emitDuplicate() = 0; // Duplicates top of stack, if applicable

    // Memory operations (for objects, arrays)
    virtual void emitNewObject(DVLANG::COMPILER_::GENERATE::Struct* classType, AbstractOperand* targetAddressOperand) = 0;
    virtual void emitNewArray(DVLANG::COMPILER_::GENERATE::Struct* elementType, AbstractOperand* sizeOperand, AbstractOperand* targetAddressOperand) = 0;
    virtual void emitArrayLength(AbstractOperand* arrayAddressOperand, AbstractOperand* targetLengthOperand) = 0;
    virtual void emitHeapFree(AbstractOperand* addressOperand) = 0; // Free memory allocated on heap

    // Type conversions
    virtual void emitConvert(AbstractOperand* sourceOperand, DVLANG::COMPILER_::GENERATE::Struct* targetType, AbstractOperand* resultTargetOperand) = 0;

    // Miscellaneous
    // For an instruction like LDCS (Load Constant String from string_set)
    virtual void emitLoadConstantStringAddress(int stringIdInTable, AbstractOperand* targetAddressOperand) = 0;

    // Get current code generation offset (useful for debugging or complex address calculations by frontend)
    virtual int getCurrentCodeOffset() const = 0;
};

} // namespace GENERATE
} // namespace COMPILER_
} // namespace DVLANG

#endif // I_CODE_GENERATOR_H
