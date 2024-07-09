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

} IST;


typedef enum {
    MOV,
    PUSH,
    POP,
    ADD,
    XCHG,
    JMP,


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
    uint16_t config; // d,w,reg,rm,mod,data,src_mem,dst_mem 8 bits reserved.
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
#define RESERVED3   0x400

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