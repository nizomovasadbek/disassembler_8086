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
    {PUSH_SGMT_REG, PUSH}
};

#define TABLE_SIZE sizeof(table)/sizeof(Instruction)

uint32_t disassemble(uint32_t instruction) {

    uint8_t header = 0, sec = 0, th = 0, fo = 0;

    //decompose
    if(!(instruction & 0xFF000000)) {
        if(!(instruction & 0x00FF0000)) {
            if(!(instruction & 0x0000FF00)) {
                if(!(instruction & 0x000000FF)) {
                    return 0;
                } else {
                    header = instruction;
                }
            } else {
                header = instruction >> 8;
                sec = instruction;
            }
        } else {
            header = instruction >> 16;
            sec = instruction >> 8;
            th = instruction;
        }
    } else {
        header = instruction >> 24;
        sec = instruction >> 16;
        th = instruction >> 8;
        fo = instruction;
    }

    Instruction ins;
    for(size_t i = 0; i < TABLE_SIZE; i++) {
        if((header & table[i].instc) == table[i].instc) {
            ins = table[i];
            break;
        }
    }

    uint8_t w, d, mod, rm, reg;

    char* instruction_sets[] = { "mov", "push", "pop" };
    char* _16bit_reg[] = { "ax", "cx", "dx", "bx", "sp", "bp", "si", "di" };
    char* _8bit_reg[] = { "al", "cl", "dl", "bl", "ah", "ch", "dh", "bh", "es", "cs", "ss", "ds" };

    switch(ins.instc) {
        case MOV_REGISTER:
            w = header & 0x01;
            w = !!w;
            d = header & 0x02;
            d = !!d;
            mod = (sec >> 6) & 0x03;
            reg = (sec >> 3) & 0x07;
            rm = sec & 0x07;

            printf("Mov instruction triggered\nD=%d, Word=%d, MOD=%d, R/M=%d, register=%s\n", d, w, mod, rm, 
                (w)?_16bit_reg[reg]:_8bit_reg[reg]);

            break;
    }
    
    return 0;
}