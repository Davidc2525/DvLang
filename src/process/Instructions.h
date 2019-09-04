#ifndef INST_H
#define INST_H
namespace PROCESS
{

typedef enum
{
    ICONST = 0,
    ADD = 1,
    SUB = 2,
    MUL = 3,
    DIV = 4,
    MOD = 5,
    CWRITE = 6,
    IWRITE = 7,
    SET = 8,
    ISTORE = 9,
    ILOAD = 10,
    ILOADL = 11,
    ISTOREL = 12,
    AND = 13,
    OR = 14,
    JMP = 15, //eq,ne,lt,le,gt,ge
    CMPI = 16, //cmp
    JE = 17, //je
    JNE = 18,//jne
    JG = 19,//jg
    JL = 20,//jl
    JGE = 21,//jge
    JLE = 22,//jle
    //RGE = 23,
    CALL = 24,
    CO_CALL = 25, //TODO
    ENTER = 26,
    EXIT = 27,
    RET = 28,
    HLT = 29,
    INEG = 30,
    NCALL = 31, //TODO
   
    //arrays
    ALC = 32,
    IALOAD = 33,
    IASTORE = 34,
    ALEN = 35,
    
    //objects
    NEW = 36,
    IGF = 37,
    IPF = 38,

    LDCS = 39,
    DUP = 40,

    CALL_S = 41,

    FWRITE = 42,
    DWRITE = 43,

    DTOF = 44, //DOUBLE TO FLOAT
    FTOD = 45, //FLOAT TO DOUBLE
    DTOI = 46, // DOUBLE TO INT
    FTOI = 47, // FLOAT TO INT

    ITOF = 48, // int to float
    ITOD = 49, //int to double

    //double arithmety
    DADD = 50,
    DSUB = 51,
    DMUL = 52,
    DDIV = 53,
    DMOD = 54,
    DNEG = 55,

    //float arithmety
    FADD = 56,
    FSUB = 57,
    FMUL = 58,
    FDIV = 59,
    FMOD = 60,
    FNEG = 61,

    //relation float TODO
    FREQ = 62,
    FRNE = 63,
    FRLT = 64,
    FRLE = 65,
    FRGT = 67,
    FRGE = 68,

    //relation double TODO
    DREQ = 69,
    DRNE = 70,
    DRLT = 71,
    DRLE = 72,
    DRGT = 73,
    DRGE = 74,

    //heap
    FREE = 75,
    REALLOC = 76,

    //pasar a nativas
    ITOS = 77,
    FTOS = 78,
    DTOS = 79,
    STRCAT = 80,
    //pasar a nativas

    POP = 81,
    PUSH = 82,

    //direccion de memoria
    ADR = 83,
    ADRL = 84,

    ILOAD_S = 85,
    ISTORE_S = 86,

    SETE = 87, //je
    SETNE = 88,//jne
    SETG = 89,//jg
    SETL = 90,//jl
    SETGE = 91,//jge
    SETLE = 92,//jle

    INVERT = 93,
} OPCODES;

} // namespace PROCESS

#endif
