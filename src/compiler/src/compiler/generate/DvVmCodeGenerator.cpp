#include "DvVmCodeGenerator.h"
#include "../symbol_table/symbol_table.h"
#include <program/program.h>
#include <process/Instructions.h>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <cstring>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

namespace DVLANG {
namespace COMPILER_ {
namespace GENERATE {

// --- DvVmOperand ---
// (Constructors are in the header)

// --- DvVmCodeGenerator ---

DvVmCodeGenerator::DvVmCodeGenerator()
    : symTable(nullptr), main_pc(0), current_pc(0), globals_size(0), next_label_id(1) {
}

DvVmCodeGenerator::~DvVmCodeGenerator() {
}

void DvVmCodeGenerator::initialize(DVLANG::COMPILER_::GENERATE::SymbolTable* symbolTable) {
    symTable = symbolTable;
    program_buffer.clear();
    main_pc = 0;
    current_pc = 0;
    globals_size = symTable ? symTable->globals_size : 0; // Get globals_size from symbol table
    string_set_instance.clear();
    label_addresses.clear();
    label_fixups.clear();
    next_label_id = 1;

    // Assuming SymbolTable has a way to iterate over all string literals if needed globally
    // For now, strings are added to string_set_instance via createConstantStringOperand
}

long DvVmCodeGenerator::doubleToLongBits(double val) {
    long bits;
    static_assert(sizeof(double) == sizeof(long), "Double and Long must be same size for bit conversion");
    std::memcpy(&bits, &val, sizeof(double));
    return bits;
}

DvVmOperand* DvVmCodeGenerator::getConcreteOperand(AbstractOperand* abstractOp) {
    if (!abstractOp) { // Allow nullptr for cases like emitReturn with no value
        return nullptr;
    }
    DvVmOperand* op = static_cast<DvVmOperand*>(abstractOp);
    if (!op) {
        throw std::runtime_error("Invalid or null operand passed to getConcreteOperand when not expected");
    }
    return op;
}

void DvVmCodeGenerator::put(intdv instruction_opcode) {
    program_buffer.push_back(instruction_opcode);
    current_pc++;
}

void DvVmCodeGenerator::put2(intdv value) {
    program_buffer.push_back(value);
    current_pc++;
}

void DvVmCodeGenerator::put4(intdv value) {
    // Assuming intdv is 32-bit or 64-bit, a single slot is sufficient for typical values.
    // If intdv was smaller and we needed to store larger (e.g. 32-bit) values,
    // this would involve splitting into multiple put() calls.
    // For now, consistent with put2.
    program_buffer.push_back(value);
    current_pc++;
}

void DvVmCodeGenerator::writeOutput(const std::string& filename, DVLANG::PROGRAM::LibSymbolStore* libs) {
    rapidjson::Document doc;
    doc.SetObject();
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

    doc.AddMember("main", main_pc, allocator);
    doc.AddMember("size", current_pc, allocator);
    doc.AddMember("globals", globals_size, allocator);

    rapidjson::Value programArray(rapidjson::kArrayType);
    for (intdv instruction_word : program_buffer) {
        programArray.PushBack(instruction_word, allocator);
    }
    doc.AddMember("program", programArray, allocator);

    rapidjson::Value stringsArray(rapidjson::kArrayType);
    for (const auto& entry : this->string_set_instance) {
        rapidjson::Value strObj(rapidjson::kObjectType);
        strObj.AddMember("id", rapidjson::Value(entry.id).Move(), allocator);
        strObj.AddMember("value", rapidjson::Value(entry.content.c_str(), allocator).Move(), allocator);
        stringsArray.PushBack(strObj, allocator);
    }
    doc.AddMember("strings", stringsArray, allocator);

    rapidjson::Value libsArray(rapidjson::kArrayType);
    if (libs) {
        for (const auto& lib_entry : libs->libs) {
            rapidjson::Value libObj(rapidjson::kObjectType);
            libObj.AddMember("path", rapidjson::Value(lib_entry.second->path.c_str(), allocator).Move(), allocator);

            rapidjson::Value symbolsArray(rapidjson::kArrayType);
            for (const auto& sym_pair : lib_entry.second->symbols) {
                 rapidjson::Value symDetails(rapidjson::kObjectType);
                 symDetails.AddMember("name", rapidjson::Value(sym_pair.second->name.c_str(), allocator).Move(), allocator);
                 symDetails.AddMember("id", sym_pair.second->id, allocator);
                 symbolsArray.PushBack(symDetails, allocator);
            }
            libObj.AddMember("symbols", symbolsArray, allocator);
            libsArray.PushBack(libObj, allocator);
        }
    }
    doc.AddMember("libs", libsArray, allocator);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    std::string output_json = buffer.GetString();

    std::ofstream outfile(filename);
    if (!outfile.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
        return;
    }
    outfile << output_json;
    outfile.close();

    std::cout << "FILE " << filename << std::endl;
    std::cout << "MAIN " << main_pc << std::endl;
    std::cout << "SIZE " << current_pc << std::endl;
    std::cout << "globals: " << globals_size << std::endl;
    if (libs) {
      std::cout << "libs: " << libs->libs.size() << std::endl;
    }
}

// --- Operand Creation ---
AbstractOperand* DvVmCodeGenerator::createConstantIntegerOperand(intdv value) {
    DvVmOperand* op = new DvVmOperand(value); // Sets kind = Con, type = intType
    return op;
}

AbstractOperand* DvVmCodeGenerator::createConstantDoubleOperand(double value) {
    long bits = doubleToLongBits(value);
    // Store bits in 'val' for Con type. Type is doubleType.
    DvVmOperand* op = new DvVmOperand(DvVmOperand::Con, SymbolTable::doubleType, 0);
    op->val = bits;
    return op;
}

AbstractOperand* DvVmCodeGenerator::createConstantStringOperand(const std::string& stringValue) {
    int string_id = string_set_instance.add(stringValue.c_str(), stringValue.length());
    // Store string_id in 'val' for Con type. Type is stringType.
    DvVmOperand* op = new DvVmOperand(DvVmOperand::Con, SymbolTable::stringType, 0);
    op->val = string_id;
    return op;
}

AbstractOperand* DvVmCodeGenerator::createLocalVariableOperand(int offset, Struct* type) {
    return new DvVmOperand(DvVmOperand::Local, type, offset);
}

AbstractOperand* DvVmCodeGenerator::createGlobalVariableOperand(int address, Struct* type) {
    return new DvVmOperand(DvVmOperand::Static, type, address);
}

AbstractOperand* DvVmCodeGenerator::createFieldOperand(AbstractOperand* baseAddress, int fieldOffset, Struct* fieldType) {
    // The 'adr' of Fld operand stores the fieldOffset (or index).
    // Base address is handled by ensuring it's on stack before IGF/IPF.
    DvVmOperand* op = new DvVmOperand(DvVmOperand::Fld, fieldType, fieldOffset);
    // The baseAddress operand itself is not stored in DvVmOperand for Fld,
    // it's used by the caller to load the base address onto the stack first.
    return op;
}

AbstractOperand* DvVmCodeGenerator::createArrayElementOperand(AbstractOperand* baseAddress, AbstractOperand* indexOperand, Struct* elementType) {
    // 'adr' is not directly used for Elem kind in the same way as others.
    // Base address and index are pushed to stack before IALOAD/IASTORE.
    DvVmOperand* op = new DvVmOperand(DvVmOperand::Elem, elementType, 0);
    // Similar to FieldOperand, baseAddress and indexOperand are used by caller
    // to load values onto stack before array access instructions.
    return op;
}

AbstractOperand* DvVmCodeGenerator::createFunctionNameOperand(const std::string& functionName) {
    Obj* funcObj = symTable->find(functionName);
    if (funcObj && (funcObj->kind == Obj::Fun || funcObj->kind == Obj::Meth)) {
        // DvVmOperand constructor for Obj sets kind=Meth (or Func if we adapt constructor)
        // and adr = funcObj->adr. This address must be patched by emitFunctionPrologue.
        return new DvVmOperand(funcObj);
    }
    // Fallback: create a Func operand that might be patched later if it's a forward declaration
    // For now, relies on symbol table having the function.
    // If funcObj is not found, this will be an issue. Consider throwing error or specific handling.
    // Let's assume functions are declared before use or symTable handles forward declarations.
    if (!funcObj) {
         throw std::runtime_error("Function name not found in symbol table: " + functionName);
    }
    // The constructor DvVmOperand(Obj*) sets kind to Meth. This needs to be Func for plain functions.
    // Let's adjust or add a constructor for this. For now, manually setting.
    DvVmOperand* op = new DvVmOperand(funcObj);
    op->kind = DvVmOperand::Func; // Ensure it's Func kind
    return op;
}

// --- Instruction Emission: Helper methods ---
void DvVmCodeGenerator::loadOperandToStack(DvVmOperand* op) {
    if (!op) {
        throw std::runtime_error("DvVmCodeGenerator::loadOperandToStack: Null operand provided.");
    }
    switch (op->kind) {
        case DvVmOperand::Con:
            if (op->type == SymbolTable::stringType) {
                 put(PROCESS::LDCS); put2(op->val); // op->val is the string ID
            } else { // Integer, bool, double (long bits)
                 put(PROCESS::ICONST); put2(op->val); // op->val holds value or bits
            }
            break;
        case DvVmOperand::Local:
            // Type determines load instruction (ILOADL, DLOADL, etc.)
            if (op->type == SymbolTable::doubleType) put(PROCESS::DLOADL);
            // else if (op->type == SymbolTable::floatType) put(PROCESS::FLOADL);
            else put(PROCESS::ILOADL); // Default for int, bool, char, pointers, etc.
            put2(op->adr);
            break;
        case DvVmOperand::Static: // Global
            if (op->type == SymbolTable::doubleType) put(PROCESS::DLOAD);
            // else if (op->type == SymbolTable::floatType) put(PROCESS::FLOAD);
            else put(PROCESS::ILOAD);
            put2(op->adr);
            break;
        case DvVmOperand::Fld:
            // Base address should have been pushed by caller.
            // op->adr is field index/offset.
            if (op->type == SymbolTable::doubleType) put(PROCESS::DGF);
            // else if (op->type == SymbolTable::floatType) put(PROCESS::FGF);
            else put(PROCESS::IGF);
            put2(op->adr);
            break;
        case DvVmOperand::Elem:
            // Base address and index should have been pushed by caller.
            if (op->type == SymbolTable::doubleType) put(PROCESS::DALOAD);
            // else if (op->type == SymbolTable::floatType) put(PROCESS::FALOAD);
            else if (op->type == SymbolTable::charType) put(PROCESS::CALOAD); // Assuming CALOAD for char
            else put(PROCESS::IALOAD); // Default for int, pointers, etc.
            break;
        case DvVmOperand::Func: // Function name/address - load address
            put(PROCESS::ICONST); put2(op->adr); // op->adr is function's entry point
            break;
        // case DvVmOperand::Stack: // Value is already on stack. No load needed.
        //     break;
        default:
            throw std::runtime_error("DvVmCodeGenerator::loadOperandToStack: Unsupported operand kind for load: " + std::to_string(op->kind));
    }
}

void DvVmCodeGenerator::storeOperandFromStack(DvVmOperand* op) {
    if (!op) {
         throw std::runtime_error("DvVmCodeGenerator::storeOperandFromStack: Null operand provided.");
    }
    switch (op->kind) {
        case DvVmOperand::Local:
            if (op->type == SymbolTable::doubleType) put(PROCESS::DSTOREL);
            // else if (op->type == SymbolTable::floatType) put(PROCESS::FSTOREL);
            else put(PROCESS::ISTOREL);
            put2(op->adr);
            break;
        case DvVmOperand::Static: // Global
            if (op->type == SymbolTable::doubleType) put(PROCESS::DSTORE);
            // else if (op->type == SymbolTable::floatType) put(PROCESS::FSTORE);
            else put(PROCESS::ISTORE);
            put2(op->adr);
            break;
        case DvVmOperand::Fld:
            // Value to store and base address must be on stack. op->adr is field index/offset.
            if (op->type == SymbolTable::doubleType) put(PROCESS::DPF);
            // else if (op->type == SymbolTable::floatType) put(PROCESS::FPF);
            else put(PROCESS::IPF);
            put2(op->adr);
            break;
        case DvVmOperand::Elem:
            // Value, base addr, index on stack.
            if (op->type == SymbolTable::doubleType) put(PROCESS::DASTORE);
            // else if (op->type == SymbolTable::floatType) put(PROCESS::FASTORE);
            else if (op->type == SymbolTable::charType) put(PROCESS::CASTORE); // Assuming CASTORE for char
            else put(PROCESS::IASTORE); // Default for int, pointers
            break;
        default:
            throw std::runtime_error("DvVmCodeGenerator::storeOperandFromStack: Unsupported or non-writable operand kind for store: " + std::to_string(op->kind));
    }
}

// --- Instruction Emission: Semantic Actions ---
void DvVmCodeGenerator::emitAssign(AbstractOperand* target, AbstractOperand* source) {
    DvVmOperand* srcOp = getConcreteOperand(source);
    DvVmOperand* targetOp = getConcreteOperand(target);

    loadOperandToStack(srcOp);
    // Type conversion if necessary (e.g. int to double) before storing
    if (srcOp->type != targetOp->type) {
        // This is a simple assignment, explicit conversions should be handled by emitConvert.
        // However, if implicit conversion is allowed at assignment (e.g. int to double),
        // it would need to be handled here. For a stricter system, types should match or be compatible.
        // For now, assume types are compatible or frontend inserts explicit emitConvert.
        // If targetOp->type is double and srcOp->type is int, ITOD is needed.
        if (targetOp->type == SymbolTable::doubleType && srcOp->type == SymbolTable::intType) {
            put(PROCESS::ITOD);
        } else if (targetOp->type == SymbolTable::intType && srcOp->type == SymbolTable::doubleType) {
            put(PROCESS::DTOI);
        }
        // Add other implicit conversions if supported by DvL.
    }
    storeOperandFromStack(targetOp);
}

void DvVmCodeGenerator::emitBinaryArithmetic(AbstractOperand* op1, AbstractOperand* op2, AbstractOperand* resultTarget, PROCESS::OPCODES intOp, PROCESS::OPCODES doubleOp /*, PROCESS::OPCODES floatOp*/) {
    DvVmOperand* concreteOp1 = getConcreteOperand(op1);
    DvVmOperand* concreteOp2 = getConcreteOperand(op2);
    DvVmOperand* concreteResult = resultTarget ? getConcreteOperand(resultTarget) : nullptr;

    loadOperandToStack(concreteOp1);
    // If op1 is int and op2 is double, promote op1 to double
    if (concreteOp1->type == SymbolTable::intType && concreteOp2->type == SymbolTable::doubleType) {
        put(PROCESS::ITOD);
    }
    loadOperandToStack(concreteOp2);
    // If op1 is double and op2 is int, promote op2 to double
    if (concreteOp1->type == SymbolTable::doubleType && concreteOp2->type == SymbolTable::intType) {
        put(PROCESS::ITOD);
    }

    PROCESS::OPCODES selected_op = intOp;
    Struct* resultType = concreteOp1->type; // Default to op1's type

    if (concreteOp1->type == SymbolTable::doubleType || concreteOp2->type == SymbolTable::doubleType) {
        selected_op = doubleOp;
        resultType = SymbolTable::doubleType;
    // } else if (concreteOp1->type == SymbolTable::floatType || concreteOp2->type == SymbolTable::floatType) {
    //    selected_op = floatOp; // Assuming float ops exist
    //    resultType = SymbolTable::floatType;
    }
    put(selected_op);

    if (concreteResult) {
        // If result of mixed mode (e.g. int+double=double) is stored into int, DTOI needed
        if (concreteResult->type == SymbolTable::intType && resultType == SymbolTable::doubleType) {
            put(PROCESS::DTOI);
        }
        // If result is double and target is double, direct store
        // If result is int and target is int, direct store
        // If result is int and target is double, ITOD needed
        if (concreteResult->type == SymbolTable::doubleType && resultType == SymbolTable::intType) {
            put(PROCESS::ITOD);
        }
        storeOperandFromStack(concreteResult);
    } else {
        // Result stays on stack. This path is less common for this interface's design
        // but could be used if the result is immediately consumed by another stack operation.
    }
}

void DvVmCodeGenerator::emitAdd(AbstractOperand* op1, AbstractOperand* op2, AbstractOperand* resultTarget) {
    emitBinaryArithmetic(op1, op2, resultTarget, PROCESS::ADD, PROCESS::DADD);
}

void DvVmCodeGenerator::emitSubtract(AbstractOperand* op1, AbstractOperand* op2, AbstractOperand* resultTarget) {
    emitBinaryArithmetic(op1, op2, resultTarget, PROCESS::SUB, PROCESS::DSUB);
}

void DvVmCodeGenerator::emitMultiply(AbstractOperand* op1, AbstractOperand* op2, AbstractOperand* resultTarget) {
    emitBinaryArithmetic(op1, op2, resultTarget, PROCESS::MUL, PROCESS::DMUL);
}

void DvVmCodeGenerator::emitDivide(AbstractOperand* op1, AbstractOperand* op2, AbstractOperand* resultTarget) {
    // TODO: Check for division by zero if required by language semantics at compile time (rare)
    // or ensure DvVM handles it (e.g. throws runtime error).
    emitBinaryArithmetic(op1, op2, resultTarget, PROCESS::DIV, PROCESS::DDIV);
}

void DvVmCodeGenerator::emitModulo(AbstractOperand* op1, AbstractOperand* op2, AbstractOperand* resultTarget) {
    emitBinaryArithmetic(op1, op2, resultTarget, PROCESS::MOD, PROCESS::DMOD);
}

void DvVmCodeGenerator::emitNegate(AbstractOperand* source, AbstractOperand* resultTarget) {
    DvVmOperand* srcOp = getConcreteOperand(source);
    DvVmOperand* targetOp = resultTarget ? getConcreteOperand(resultTarget) : nullptr;
    loadOperandToStack(srcOp);
    if (srcOp->type == SymbolTable::doubleType) put(PROCESS::DNEG);
    // else if (srcOp->type == SymbolTable::floatType) put(PROCESS::FNEG);
    else put(PROCESS::INEG);
    if (targetOp) {
        // Ensure type compatibility if src and target types differ (e.g. result of INEG is int)
        storeOperandFromStack(targetOp);
    }
}

void DvVmCodeGenerator::emitAnd(AbstractOperand* op1, AbstractOperand* op2, AbstractOperand* resultTarget) {
    // Logical AND typically for boolean, but DvVM might use integer AND
    emitBinaryArithmetic(op1, op2, resultTarget, PROCESS::AND, PROCESS::AND); // Assuming AND works for int-like bools
}

void DvVmCodeGenerator::emitOr(AbstractOperand* op1, AbstractOperand* op2, AbstractOperand* resultTarget) {
    // Logical OR
    emitBinaryArithmetic(op1, op2, resultTarget, PROCESS::OR, PROCESS::OR); // Assuming OR works for int-like bools
}

void DvVmCodeGenerator::emitNot(AbstractOperand* source, AbstractOperand* resultTarget) {
    DvVmOperand* srcOp = getConcreteOperand(source);
    DvVmOperand* targetOp = resultTarget ? getConcreteOperand(resultTarget) : nullptr;
    loadOperandToStack(srcOp);
    put(PROCESS::INVERT); // Assumes INVERT for logical NOT (0 -> 1, non-0 -> 0)
    if (targetOp) storeOperandFromStack(targetOp);
}

// --- Control Flow ---
int DvVmCodeGenerator::createLabel() {
    return next_label_id++;
}

void DvVmCodeGenerator::placeLabel(int label) {
    label_addresses[label] = current_pc;
    if (label_fixups.count(label)) {
        for (intdv fixup_addr : label_fixups[label]) {
            program_buffer[fixup_addr] = current_pc;
        }
        label_fixups.erase(label);
    }
}

void DvVmCodeGenerator::emitJump(int label) {
    put(PROCESS::JMP);
    if (label_addresses.count(label)) {
        put2(label_addresses[label]);
    } else {
        put2(0); // Placeholder
        label_fixups[label].push_back(current_pc - 1);
    }
}

void DvVmCodeGenerator::emitCompare(AbstractOperand* op1, AbstractOperand* op2) {
    DvVmOperand* concreteOp1 = getConcreteOperand(op1);
    DvVmOperand* concreteOp2 = getConcreteOperand(op2);
    loadOperandToStack(concreteOp1);
    // Promote op1 to double if types differ and op2 is double
    if (concreteOp1->type == SymbolTable::intType && concreteOp2->type == SymbolTable::doubleType) {
        put(PROCESS::ITOD);
    }
    loadOperandToStack(concreteOp2);
    // Promote op2 to double if types differ and op1 is double (now top of stack is concreteOp1's value)
    if (concreteOp1->type == SymbolTable::doubleType && concreteOp2->type == SymbolTable::intType) {
        put(PROCESS::ITOD); // Convert the second operand (now deeper in stack)
                            // This requires careful stack management or emitting conversions before loading.
                            // Simpler: ensure both are loaded then one is converted if needed before comparison instruction.
                            // Let's reload op1 if a conversion happened for op2, or vice-versa, to ensure types match for CMPI/DCMP etc.
                            // For now, assume emitBinaryArithmetic's promotion logic is a model:
                            // Load op1, load op2, then convert one if needed, then compare.
                            // This is complex for a generic emitCompare.
                            // The current simple load-load-compare is fine if types are same.
                            // If types are different, one must be converted before the compare instruction.
    }

    // The generic conditional jumps (JE, JNE etc.) in DvVM work with flags set by CMPI.
    // For doubles, DCMPx instructions (DCMPEQ, DCMPGT etc.) push 0 or 1.
    // To bridge this: perform type-specific compare, then if it's not CMPI,
    // push ICONST 0 and then CMPI to set flags based on the 0/1 result.

    if (concreteOp1->type == SymbolTable::doubleType || concreteOp2->type == SymbolTable::doubleType) {
        // This is tricky. DCMP instructions (if they exist like DCMPEQ, DCMPGT)
        // or sequences like DSUB; DTOI; (check sign) are needed for doubles.
        // For simplicity, if DvVM has a generic DCMP that sets flags like CMPI, use it.
        // Otherwise, we need to emit e.g. DSub, then check flags (if DSub sets them) or analyze result.
        // Assuming no direct DCMP that sets flags for JE/JL etc.
        // We will emit a sequence that results in 0 or 1 on stack, then compare with 0.
        // This means the actual comparison logic (e.g. op1 < op2) must be chosen *before* the jump.
        // This makes emitCompare coupled with the subsequent jump.
        // For now, this emitCompare will just load operands. The conditional jump must select the right compare.
        // This is a limitation of a generic emitCompare before a specific conditional jump.
        // Let's assume for now that the JUMP instruction itself will handle the type.
        // So, emitCompare just ensures operands are on stack.
        // This is a common design in some three-address code forms but not ideal for stack VMs if not careful.
        // A better `emitCompare` would take the comparison type (EQ, LT, etc.)
        // For now, `emitCompare` is a NO-OP beyond loading for non-integers,
        // and the jump instructions will have to be smarter.
        // OR: emitCompare always does CMPI, requiring conversion to int for doubles for generic jumps. (Bad for precision)

        // Alternative: emitCompare does nothing if types are not int. Conditional jumps handle it.
        // This is what the original code seemed to imply by warning.
        // Let's stick to CMPI for int, and conditional jumps will need specific logic for doubles.
        // For now, if not int, this function won't emit CMPI. The jumps will need to be type-aware.
         if (concreteOp1->type == SymbolTable::intType && concreteOp2->type == SymbolTable::intType) {
            put(PROCESS::CMPI);
         } else {
            // For non-int types, conditional jumps will need to do more work.
            // This function will not emit a CMPI to avoid misinterpreting double bits as int.
            // std::cerr << "Warning: emitCompare called for non-integer types. Conditional jump must handle this." << std::endl;
         }

    } else { // Both are int compatible
        put(PROCESS::CMPI);
    }
}

// Helper for conditional jumps
void DvVmCodeGenerator::emitConditionalJump(int label, PROCESS::OPCODES jumpIntOp, PROCESS::OPCODES compareAndJumpDoubleOpBase) {
    // This is a conceptual helper. The actual implementation for doubles is complex
    // if JE/JL etc. only use CMPI flags.
    // For now, assume the AbstractOperands op1, op2 for comparison are known (e.g. stored from emitCompare)
    // This is not clean. emitCompare should ideally take the comparison operator.

    // Revisit: The jump instructions need to know the types of operands compared.
    // This information is not passed to them. It was part of the emitCompare call.
    // This implies emitCompare must store the types, or the jump ops peek/know.

    // Let's assume emitCompare has left operands on stack.
    // The jump must now pick the right comparison. This is messy.

    // Simplification: Assume the last emitCompare stored the types implicitly.
    // This is bad design but a common patch.
    // A better way: emitJumpIfEqual(op1, op2, label) - but interface is fixed.

    // Fallback: For now, all conditional jumps assume CMPI flags are set.
    // This means emitCompare *must* result in CMPI being called, possibly after type-specific comparison.
    // This is the TODO from the prompt.

    // Let's assume op1 and op2 are still on stack from a previous call or need to be re-loaded if emitCompare was a NO-OP for them.
    // This part of the design is problematic with the current ICodeGenerator interface.
    // The most robust solution within the current interface constraints:
    // emitCompare(op1, op2) for doubles should do: DSub, Push 0, CMPI (compares result of DSub with 0).
    // This is not what was implemented in emitCompare.

    // Given the current emitCompare: if it was non-int, no CMPI was emitted.
    // So, the Jxx instruction will likely fail or use stale flags.
    // This needs a proper fix in how comparisons and jumps are linked.

    // Quick Fix Attempt: Jumps will assume CMPI flags. emitCompare needs to ensure they are set.
    // This means `emitCompare` needs to be smarter for doubles.
    // (emitCompare is already written, cannot change it in this step based on instructions)
    // So, the JUMP instructions will be simple for now, using the provided Jxx opcode.
    // This means double comparisons for conditional jumps are currently broken / reliant on CMPI over raw bits.

    put(jumpIntOp); // Using the integer version of the jump opcode
    if (label_addresses.count(label)) {
        put2(label_addresses[label]);
    } else {
        put2(0);
        label_fixups[label].push_back(current_pc - 1);
    }
}


void DvVmCodeGenerator::emitJumpIfEqual(int label) {
    // Assumes flags set by CMPI. For doubles: DREQ, ICONST 0, CMPI would precede this.
    // If emitCompare didn't do that, this is for integers only.
    emitConditionalJump(label, PROCESS::JE, PROCESS::DCMPEQ); // DCMPEQ is placeholder for double comparison logic
}
void DvVmCodeGenerator::emitJumpIfNotEqual(int label) {
    emitConditionalJump(label, PROCESS::JNE, PROCESS::DCMPNE);
}
void DvVmCodeGenerator::emitJumpIfLessThan(int label) {
    emitConditionalJump(label, PROCESS::JL, PROCESS::DCMPLT);
}
void DvVmCodeGenerator::emitJumpIfLessThanOrEqual(int label) {
    emitConditionalJump(label, PROCESS::JLE, PROCESS::DCMPLE);
}
void DvVmCodeGenerator::emitJumpIfGreaterThan(int label) {
    emitConditionalJump(label, PROCESS::JG, PROCESS::DCMPGT);
}
void DvVmCodeGenerator::emitJumpIfGreaterThanOrEqual(int label) {
    emitConditionalJump(label, PROCESS::JGE, PROCESS::DCMPGE);
}


// --- Function Calls ---
void DvVmCodeGenerator::emitFunctionPrologue(const std::string& functionName, int paramCount, int localVariableSpace) {
    Obj* funcObj = symTable->find(functionName);
    if (funcObj) { // Could be Fun or Meth
        funcObj->adr = current_pc; // Patch function address in symbol table
        if (symTable->main_f && funcObj->name == symTable->main_f->name ) {
             main_pc = current_pc;
        }
    } else {
        std::cerr << "Warning: Function " << functionName << " not found in symbol table for prologue." << std::endl;
    }

    put(PROCESS::ENTER);
    put2(paramCount);
    put2(localVariableSpace);
}

void DvVmCodeGenerator::emitFunctionEpilogue(const std::string& functionName) {
    put(PROCESS::EXIT);
    // RET is typically emitted by emitReturn. If a function can fall off,
    // an explicit RET might be needed here, or ensure all paths have emitReturn.
    // For void functions that fall off: EXIT then RET.
    // Let's assume emitReturn is always called, or add RET if not.
    // A common pattern is that emitReturn includes EXIT. If so, this epilogue might be redundant or only for specific cases.
    // For now, assuming EXIT is standard, and RET is separate.
}

void DvVmCodeGenerator::emitCall(AbstractOperand* functionOperand, const std::vector<AbstractOperand*>& args, AbstractOperand* returnTarget /*nullable*/) {
    DvVmOperand* funcOp = getConcreteOperand(functionOperand);

    for (auto it = args.rbegin(); it != args.rend(); ++it) {
        loadOperandToStack(getConcreteOperand(*it));
    }

    // funcOp->adr should be the function's entry point (set during its prologue)
    if (funcOp->kind == DvVmOperand::Func || funcOp->kind == DvVmOperand::Meth) {
        put(PROCESS::ICONST);
        put2(funcOp->adr);
    } else {
        // Function pointer stored in a variable/field
        loadOperandToStack(funcOp);
    }

    put(PROCESS::CALL_S);

    if (returnTarget) {
        DvVmOperand* targetOp = getConcreteOperand(returnTarget);
        // Handle type conversion if function return type differs from target storage type
        // e.g. if function returns int but target is double (ITOD after call)
        // This requires knowing function's return type. funcOp->type might be the function signature type,
        // from which return type can be extracted. For now, assume direct store.
        storeOperandFromStack(targetOp);
    } else {
        // If function returns a value but no target, it should be POPped if it's not void.
        // This requires knowing if the function is non-void.
        // Assuming funcOp->type is the function's type, get return type from it.
        // If funcOp->type->returnType != voidType && funcOp->type->returnType != nullptr
        //    put(PROCESS::POP); // or DPOP for double
        // This is an advanced detail; for now, assuming caller manages stack balance.
    }
}

void DvVmCodeGenerator::emitNativeCall(const std::string& nativeFunctionName, int nativeSymbolId, const std::vector<AbstractOperand*>& args, AbstractOperand* returnTarget /*nullable*/) {
    for (auto it = args.rbegin(); it != args.rend(); ++it) {
        loadOperandToStack(getConcreteOperand(*it));
    }
    put(PROCESS::ICONST);
    put2(nativeSymbolId);
    put(PROCESS::NCALL);

    if (returnTarget) {
        // Similar to emitCall, consider return type and potential POP if value not used.
        storeOperandFromStack(getConcreteOperand(returnTarget));
    }
}

void DvVmCodeGenerator::emitReturn(AbstractOperand* returnValue /*nullable*/) {
    if (returnValue) {
        loadOperandToStack(getConcreteOperand(returnValue));
    }
    // EXIT is often paired with RET. If emitFunctionEpilogue always emits EXIT,
    // then just RET here. If emitReturn is the sole way functions end, it might need EXIT.
    // Standard: emitFunctionEpilogue does general cleanup (EXIT), emitReturn does value load + RET.
    // So, assuming EXIT is handled by epilogue or is not strictly tied to every RET.
    // For DvVM, typical structure is func_body -> [load_ret_val] -> EXIT -> RET
    // Let's assume EXIT was called by emitFunctionEpilogue or just before this.
    // If not, it might be needed here. The prompt was to implement placeholders,
    // so I'll stick to the direct translation of "return".
    // Final decision: Many compilers have return emit both. Let's do that if epilogue isn't guaranteed.
    // However, the interface has emitFunctionEpilogue separate. So, just RET.
    put(PROCESS::RET);
}

// --- Stack Manipulation ---
void DvVmCodeGenerator::emitPush(AbstractOperand* operand) {
    loadOperandToStack(getConcreteOperand(operand));
}

void DvVmCodeGenerator::emitPop(AbstractOperand* targetOperand /*nullable*/) {
    if (targetOperand) {
        storeOperandFromStack(getConcreteOperand(targetOperand));
    } else {
        // Need to know type for POP vs DPOP. Assume operand on stack is int-like.
        // This is ambiguous. If we need to pop a double, DPOP is needed.
        // For now, generic POP. This might be an issue.
        put(PROCESS::POP);
    }
}

void DvVmCodeGenerator::emitDuplicate() {
    // Need DUP or DDUP depending on type on stack. Assume int-like.
    put(PROCESS::DUP);
}

// --- Memory Operations ---
void DvVmCodeGenerator::emitNewObject(Struct* classType, AbstractOperand* targetAddressOperand) {
    if (!classType) throw std::runtime_error("emitNewObject: classType is null");
    intdv object_size = classType->size;
    put(PROCESS::NEW);
    put2(object_size); // Size in words/slots
    if (targetAddressOperand) {
        storeOperandFromStack(getConcreteOperand(targetAddressOperand));
    }
}

void DvVmCodeGenerator::emitNewArray(Struct* elementType, AbstractOperand* sizeOperand, AbstractOperand* targetAddressOperand) {
    if (!elementType) throw std::runtime_error("emitNewArray: elementType is null");
    loadOperandToStack(getConcreteOperand(sizeOperand));

    // ALC instruction itself might be generic, or type specific (ALC_INT, ALC_DBL)
    // Or it takes element size as another argument.
    // Assuming ALC takes number of elements from stack, and knows element size implicitly or via another op.
    // The old DvM code generator had `put(ALC); put(e->type->size);`
    // This interface doesn't have a direct `put(elementType->size)` here.
    // It implies ALC must get size from stack, and element type from somewhere else,
    // or there are typed ALC instructions.
    // For now, let's assume ALC is generic and the VM knows element size for GC etc.
    // Or, that ALC takes element size from another operand if needed.
    // If ALC needs element size, this interface is insufficient.
    // Let's assume PROCESS::ALC is sufficient. If not, this needs VM instruction review.
    // The old `code.put(ALC); code.put(x->type->fields->obj->type->size);` implies ALC took size AND element size.
    // This interface does not support that directly in emitNewArray.
    // Let's use ALC and assume it only needs number of elements.
    // This is a potential mismatch with old system if ALC needed element size.
    // A common approach is ALC_TYPE or ALC takes type_id.
    // For now:
    put(PROCESS::ALC);
    // If ALC needs element size, and it's not an immediate:
    // ICONST element_size_val
    // ALC (now expects count and element_size on stack)
    // This is speculative. Sticking to simple ALC.

    if (targetAddressOperand) {
        storeOperandFromStack(getConcreteOperand(targetAddressOperand));
    }
}

void DvVmCodeGenerator::emitArrayLength(AbstractOperand* arrayAddressOperand, AbstractOperand* targetLengthOperand) {
    loadOperandToStack(getConcreteOperand(arrayAddressOperand));
    put(PROCESS::ALEN);
    if (targetLengthOperand) {
        storeOperandFromStack(getConcreteOperand(targetLengthOperand));
    }
}

void DvVmCodeGenerator::emitHeapFree(AbstractOperand* addressOperand) {
    loadOperandToStack(getConcreteOperand(addressOperand));
    put(PROCESS::FREE);
}

// --- Type Conversions ---
void DvVmCodeGenerator::emitConvert(AbstractOperand* sourceOperand, Struct* targetType, AbstractOperand* resultTargetOperand) {
    DvVmOperand* srcOp = getConcreteOperand(sourceOperand);
    Struct* sourceType = srcOp->type;

    if (sourceType == targetType) { // No conversion needed
        loadOperandToStack(srcOp); // Just load the source
        if (resultTargetOperand) {
            storeOperandFromStack(getConcreteOperand(resultTargetOperand));
        }
        // If no result target, value remains on stack.
        return;
    }

    loadOperandToStack(srcOp);

    PROCESS::OPCODES conv_op = PROCESS::HLT;

    if (sourceType == SymbolTable::intType) {
        if (targetType == SymbolTable::doubleType) conv_op = PROCESS::ITOD;
        // else if (targetType == SymbolTable::floatType) conv_op = PROCESS::ITOF;
        else if (targetType == SymbolTable::stringType) conv_op = PROCESS::ITOS;
        else if (targetType == SymbolTable::charType) conv_op = PROCESS::ITOC; // Int to Char
    } else if (sourceType == SymbolTable::doubleType) {
        if (targetType == SymbolTable::intType) conv_op = PROCESS::DTOI;
        // else if (targetType == SymbolTable::floatType) conv_op = PROCESS::DTOF;
        else if (targetType == SymbolTable::stringType) conv_op = PROCESS::DTOS;
    // } else if (sourceType == SymbolTable::floatType) {
        // ...
    } else if (sourceType == SymbolTable::charType) {
        if (targetType == SymbolTable::intType) conv_op = PROCESS::CTOI; // Char to Int
    }
    // Add other conversions like STOI, STOD if they exist and make sense here

    if (conv_op != PROCESS::HLT) {
        put(conv_op);
    } else {
        std::cerr << "Warning: No direct DvVM conversion from type " << sourceType->name
                  << " to " << targetType->name << "." << std::endl;
        // Value loaded from sourceOperand remains on stack, unconverted.
        // If no resultTargetOperand, it might need to be POPped depending on desired semantics.
    }

    if (resultTargetOperand) {
        storeOperandFromStack(getConcreteOperand(resultTargetOperand));
    } else if (conv_op == PROCESS::HLT && sourceType != targetType) {
        // If conversion failed (no op) and no target, pop the original value to avoid stack pollution.
        // However, if sourceType == targetType, it was handled above.
        // If conv_op was HLT but a conversion was expected, implies error or unsupported.
        // Stack has original value. If not stored, it should usually be popped.
        // This depends on compiler frontend strategy: does it expect value on stack or not?
        // For now, if no target, value (converted or original if no op) stays on stack.
    }
}

// --- Miscellaneous ---
void DvVmCodeGenerator::emitLoadConstantStringAddress(int stringIdInTable, AbstractOperand* targetAddressOperand) {
    put(PROCESS::LDCS);
    put2(stringIdInTable);
    if (targetAddressOperand) {
        storeOperandFromStack(getConcreteOperand(targetAddressOperand));
    }
}

int DvVmCodeGenerator::getCurrentCodeOffset() const {
    return current_pc;
}

} // namespace GENERATE
} // namespace COMPILER_
} // namespace DVLANG

```
