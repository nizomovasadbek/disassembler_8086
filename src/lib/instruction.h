#ifndef _INSTRUCTION_H
#define _INSTRUCTION_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// MOV instruction
typedef enum {
    MOV_REGISTER =      0b10001000,  // mov to register
    MOV_IM_REG_MEM =    0b11000110, // immeadiately move to register/memory
    MOV_IM_REGISTER =   0b10110000,    // Immeadiately move to register
    MEM_TO_ACC =        0b10100000, // Memory to accumulator (A register)
    ACC_TO_MEM =        0b10100010, // Accumulator (A register) to Memory
    MOV_TO_SR =         0b10001110, // Mov to segment register
    SR_TO_MEMREG =      0b10001100, // Segment register to memory register
// Fit them into 8 bit.
// ---------------------------------------------

// PUSH INSTRUCTION
    PUSH_REG_MEM =      0b11111111, // PUSH register/memory
    PUSH_REG =          0b01010000, // PUSH register
    PUSH_SGMT_REG =     0b00000110, // Segment register 0b000(reg)110 真ん中のregは必ず0です
// ---------------------------------------------

    POP_REG_MEM =       0b10001111, // POP register/memory (effective address)
    POP_REG =           0b01011000, // POP register
    POP_SGMT_REG =      0b00000111, // POP segment register (MIDDLE)
// ---------------------------------------------

    ADD_REGMEM_REG =    0b00000000, // Add reg/mem with register
    ADD_IMDT_REG =      0b10000000, // Immeadiate to register
    ADD_IMDT_ACCUMUL =  0b00000100, // Immeadiate to accumulator
// ---------------------------------------------

    XCHG_REGMEM_REG =   0b10000110, // Exchange register/memory with register
    XCHG_ACCUMUL_REG =  0b10010000, //Exchange register with accumulator

// ---------------------------------------------

    LONG_JUMP =         0b11101001, // Unconditional near jump
    SHORT_JUMP =        0b11101011, // Unconditional short jump
    WITHIN_SEGMENT =    0b11111111, // Jump within a segment
    DRCT_INTRSGMT =     0b11101010, // Direct intersegment
    INDRCT_INTRSGMT =   0b11111111, // indirect intersegment

// ---------------------------------------------

    IN_FIXED_PORT =     0b11100100, // In fixed port
    IN_VAR_PORT =       0b11101100, // In variable port

// ---------------------------------------------

    OUT_FIXED_PORT =    0b11100110, // out fixed port
    OUT_VAR_PORT =      0b11101110, // out variable port

// ---------------------------------------------

    BYTE_TO_AL =        0b11010111, // Translate byte to Al

// ---------------------------------------------

    LOAD_EA_TO_REG =    0b10001101, // Load EA to register

// ---------------------------------------------

    LOAD_PTR_TO_DS =    0b11000101, // Load Pointer to ds

// ---------------------------------------------

    LOAD_PTR_TO_ES =    0b11000100, // Load Pointer to es

// ---------------------------------------------

    LOAD_AH_FLAGS =     0b10011111, // Load AH with flags

// ---------------------------------------------

    SAVE_AH_FLAGS =     0b10011110, // Save AH with flags

// ---------------------------------------------

    PUSH_FLAGS =        0b10011100, // Push flags
    POP_FLAGS =         0b10011101, // Pop flags

// ---------------------------------------------

    ADDC_REGMEM =       0b00010000, // Add to register/memory to register
    ADDC_IMDT_REGMEM =  0b10000000, // Immidiate register to memory
    ADDC_IMDT_ACCUMUL = 0b00010100, // Immidiate to accumulator

// ---------------------------------------------

    INC_REGMEM =        0b11111110, // Increment register/memory
    INC_REG =           0b01000000, // Increment register

// ---------------------------------------------

    ASCII_ADJUST =      0b00110111, // ASCII adjust for Add
    DECIMAL_ADJUST =    0b00100111, // Decimal adjust for Add

// ---------------------------------------------

    SUB_REGMEM_MEM =    0b00101000, // Subtract register/memory from register
    SUB_IMDT_REG =      0b10000000, // Subtract immidiate from memory
    SUB_IMDT_ACCUMUL =  0b00101100, // Subtract immidiate from accumulator

// ---------------------------------------------

    SSB_REGMEM_MEM =    0b00011000, // Subtract with borrow
    SSB_IMDT_REG =      0b10000000, // Subtract with borrow immidiate from register/memory
    SSB_IMDT_ACCUMUL =  0b00011100, // Subtract with borrow immidiate from 

// ---------------------------------------------

    DEC_REGMEM =        0b11111110, // Decrement register memory
    DEC_REG =           0b01001000, // decrement register

} IST;


typedef enum {
    MOV,
    PUSH,
    POP,
    ADD,
    XCHG,
    JMP,
    IN,
    OUT,
    XLATB,
    LEA,
    LDS,
    LES,
    LAHF,
    SAHF,
    PUSHF,
    POPF,
    ADC,
    INC,
    AAA,
    BAA,
    SUB,
    SBB,
    DEC,

    NONE
} Type;

typedef struct {
    IST instc;
    Type type;
    uint8_t skip;
    size_t len;
} Instruction;

typedef struct {
    uint8_t d;
    uint8_t w;
    uint8_t sw;
    uint8_t reg;
    uint8_t rm;
    uint8_t mod;
    uint8_t displow;
    uint8_t disphigh;
    uint8_t data;
    uint8_t data_ifw;
    uint8_t addrlow;
    uint8_t addrhigh;
    bool immediate;
    bool segment;
    uint32_t config; // d,w,reg,rm,mod,data,src_mem,dst_mem 8 bits reserved.
} Arch;

#define D           0x01
#define W           0x02
#define REG         0x04
#define RM          0x08
#define MOD         0x10
#define DATA        0x20
#define SRC_MEM     0x40
#define DST_MEM     0x80
#define ADDR        0x100
#define SW          0x200
#define DIRECT_INTERSEGMENT   0x400
#define PORT        0x800

typedef struct {
    uint8_t reg;
    uint8_t reg2_enabled;
    uint8_t reg2;
    uint16_t disp;
} EA;

#define OPCODE_MOV  0
#define OPCODE_PUSH 1
#define OPCODE_POP  2

#define AX 0
#define CX 1
#define DX 2
#define BX 3
#define SP 4
#define BP 5
#define SI 6
#define DI 7

#define AL 0
#define CL 1
#define DL 2
#define BL 3
#define AH 4
#define CH 5
#define DH 6
#define BH 7

#define ES 8
#define CS 9
#define SS 10
#define DS 11

Instruction identify(uint8_t firstByte);
uint32_t analyse(uint8_t*, size_t);
char* build_string(Instruction*, Arch);
void rm_modification(uint8_t* mod, uint8_t* rm);
void my_itoa(uint16_t, char*);
void pointer_wrapper(char[]);
uint8_t remove_center(uint8_t value, Instruction ins);

#endif