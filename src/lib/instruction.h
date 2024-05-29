#ifndef _INSTRUCTION_H
#define _INSTRUCTION_H

// MOV instruction
#define MOV_REGISTER        0b100010  // mov to register
#define MOV_IM_REG_MEM      0b1100011 // immeadiately move to register/memory
#define MOV_IM_REGISTER     0b1011    // Immeadiately move to register
#define MEM_TO_ACC          0b1010000 // Memory to accumulator (A register)
#define ACC_TO_MEM          0b1010001 // Accumulator (A register) to Memory
#define MOV_TO_SR           0b10001110 // Mov to segment register
#define SR_TO_MEMREG        0b10001100 // Segment register to memory register
// ---------------------------------------------

#endif