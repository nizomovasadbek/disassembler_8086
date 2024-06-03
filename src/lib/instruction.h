#ifndef _INSTRUCTION_H
#define _INSTRUCTION_H

#include <stdint.h>

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
    SGMT_REG =          0b00011110, // Segment register 0b000(reg)110 真ん中のregは必ず1です
// ---------------------------------------------

// POP INSTRUCTION
} IST;


typedef enum {
    NONE,
    MOV,
    PUSH
} Type;

typedef struct {
    IST instc;
    Type type;
} Instruction;

uint32_t disassemble(uint32_t);

#endif