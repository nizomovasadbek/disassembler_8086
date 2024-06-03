#include "instruction.h"
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>

Instruction table[] = { // Instruction Set Table
    {.instc = MOV_REGISTER, .type = MOV},
    {MOV_IM_REG_MEM, MOV},
    {MOV_IM_REGISTER, MOV},
    {MEM_TO_ACC, MOV},
    {ACC_TO_MEM, MOV},
    {MOV_TO_SR, MOV},
    {SR_TO_MEMREG, MOV},
    {PUSH_REG_MEM, PUSH},
    {PUSH_REG, PUSH},
    {SGMT_REG, PUSH}
};

#define TABLE_SIZE sizeof(table)/sizeof(Instruction)

uint32_t disassemble(uint32_t __attribute__((unused)) instruction) {
    
    uint8_t header = 0xFF & instruction;
    Instruction ins;
    for(size_t i = 0; i < TABLE_SIZE; i++) {
        if((header & table[i].instc) == header) {
            ins = table[i];
            break;
        }
    }

    uint8_t sec = (instruction & 0x00FF0000) << 8;
    uint8_t th = (instruction & 0x0000FF00) << 16;
    uint8_t fo = (instruction & 0x000000FF) << 24;
    uint8_t w;
    uint8_t d;
    uint8_t mod;
    uint8_t rm;
    uint8_t reg;

    char* instruction_sets[] = { "mov", "push" };
    char* _16bit_reg[] = { "ax", "cx", "dx", "bx", "sp", "bp", "si", "di" };
    char* _8bit_reg[] = { "al", "cl", "dl", "bl", "ah", "ch", "dh", "bh", "es", "cs", "ss", "ds" };

    switch(ins.instc) {
        case MOV_REGISTER:
            printf("%s ", instruction_sets[0]);
            w = header & 0x01;
            w = !!w;
            d = header & 0x02;
            d = !!d;
            mod = sec & 0x03;
            reg = (sec & 0x1C) >> 2;
            rm = (sec & 0xE0) >> 5;

            break;
    }
    
    return 0;
}