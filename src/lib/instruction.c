#include "instruction.h"
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

Instruction table[] = { // Instruction Set Table
    {.instc = MOV_REGISTER, .type = MOV, .skip = 2},
    {MOV_IM_REG_MEM, MOV, 3},
    {MOV_IM_REGISTER, MOV, 2},
    {MEM_TO_ACC, MOV, 3},
    {ACC_TO_MEM, MOV, 3},
    {MOV_TO_SR, MOV, 2},
    {SR_TO_MEMREG, MOV, 2},
    {PUSH_REG_MEM, PUSH, 2},
    {PUSH_REG, PUSH, 1},
    {PUSH_SGMT_REG, PUSH, 1}
};

char* instruction_sets[] = { "mov", "push", "pop" };
char* _16bit_reg[] = { "ax", "cx", "dx", "bx", "sp", "bp", "si", "di" };
char* _8bit_reg[] = { "al", "cl", "dl", "bl", "ah", "ch", "dh", "bh", "es", "cs", "ss", "ds" };

#define TABLE_SIZE sizeof(table)/sizeof(Instruction)

Instruction identify(uint8_t firstByte) {
    Instruction ins;
    ins.instc = 0;
    ins.skip = 0;
    ins.type = NONE;

    for(size_t i = 0; i < TABLE_SIZE; i++) {
        if((firstByte & table[i].instc) == table[i].instc) {
            ins = table[i];
            return ins;
        }
    }

    return ins;
}

uint32_t analyse(uint8_t* buffer, size_t BUFFER_SIZE) {
    size_t position = 0;
    uint8_t delta = 0;

    Instruction ins = identify(buffer[position]);
    #ifdef DEBUG
    printf("First opcode 0x%02X\n", ins.instc);
    #endif

    char* instruction_string;
    Arch a;

    while(position < BUFFER_SIZE) {
        printf("%zX\t", position);
        delta = 0;
        switch(ins.instc) {
            case MOV_REGISTER:

                a.w = buffer[position] & 0x01;
                a.w = !!a.w;
                a.d = buffer[position] & 0x02;
                a.d = !!a.d;
                a.immediate = false;
                a.mod = (buffer[position+1] >> 6) & 0x03;
                a.reg = (buffer[position+1] >> 3) & 0x07;
                a.rm = buffer[position+1] & 0x07;

                instruction_string = build_string(&ins, a);
                printf("%s\n", instruction_string);

                free(instruction_string);

                break;

            case MOV_IM_REG_MEM:

                a.w = buffer[position] & 0x01;
                a.w = !!a.w;
                a.d = 0;
                a.immediate = true;
                a.mod = (buffer[position+1] >> 6) & 0x03;
                a.rm = buffer[position+1] & 0x07;
                a.data = buffer[position+2];
                if(a.w) {
                    a.data_ifw = buffer[position+3];
                    delta = 1;
                }

                instruction_string = build_string(&ins, a);
                printf("%s\n", instruction_string);

                free(instruction_string);

                break;
            default:
                delta = 1;
                break;
        }

        position += ins.skip + delta;
    }
    
    return 0;
}

char* build_string(Instruction* ins, Arch arch) {
    
    EA ea;

    char* result = (char*) malloc(40);
    char rm[15];
    char reg[10];
    char disp[10];

    if(!arch.immediate)
        sprintf(reg, "%s", (arch.w)?_16bit_reg[arch.reg]:_8bit_reg[arch.reg]);
    else {

        if(arch.w) {
            uint16_t flow = arch.data_ifw;
            flow <<= 8;
            flow |= arch.data;
            sprintf(reg, "0x%X", flow);
        } else {
            sprintf(reg, "0x%X", arch.data);
        }
    }
    switch(arch.mod) {
        case 0:

            ea.disp = 0; // if mod = 0, then DISP = 0; disp-high and disp-low are absent.

            break;

        case 1:

            ea.disp = (uint16_t) arch.displow; // if mod = 0b01 (1), then disp-low is sign-extended to 16-bit and disp-high is absent.
            break;

        case 2:

            ea.disp = 0; // if mod=0b10 (2), then disp-high and disp-low combined
            ea.disp |= arch.disphigh;
            ea.disp <<= 8;
            ea.disp |= arch.displow;

            break;

        case 3:
                // if mod=0b11  (3), then R/M is treated as REG field
            sprintf(rm, "%s", (arch.w)?_16bit_reg[arch.rm]:_8bit_reg[arch.rm]);
            break;

    }

    if(arch.mod != 3) 
    // if mod = 3, then R/M is same as REG, so there is no need to build effective address.
    switch(arch.rm) {
        case 0:

            sprintf(rm, "[%s+%s", _16bit_reg[BX], _16bit_reg[SI]);
            if(ea.disp) {
                strncat(rm, "+" , 1);
                my_itoa(ea.disp, disp);
                strncat(rm, "0x", 3);
                strncat(rm, disp, 10);
            }
            strncat(rm, "]", 1);

            break;

        case 1:

            sprintf(rm, "[%s+%s", _16bit_reg[BX], _16bit_reg[DI]);
            if(ea.disp) {
                strncat(rm, "+" , 1);
                my_itoa(ea.disp, disp);
                strncat(rm, "0x", 3);
                strncat(rm, disp, 10);
            }
            strncat(rm, "]", 1);

            break;

        case 2:

            sprintf(rm, "[%s+%s", _16bit_reg[BP], _16bit_reg[SI]);
            if(ea.disp) {
                strncat(rm, "+" , 1);
                my_itoa(ea.disp, disp);
                strncat(rm, "0x", 3);
                strncat(rm, disp, 10);
            }
            strncat(rm, "]", 1);

            break;

        case 3:

            sprintf(rm, "[%s+%s", _16bit_reg[BP], _16bit_reg[DI]);
            if(ea.disp) {
                strncat(rm, "+" , 1);
                my_itoa(ea.disp, disp);
                strncat(rm, "0x", 3);
                strncat(rm, disp, 10);
            }
            strncat(rm, "]", 1);

            break;

        case 4:

            sprintf(rm, "[%s", _16bit_reg[SI]);
            if(ea.disp) {
                strncat(rm, "+", 1);
                my_itoa(ea.disp, disp);
                strncat(rm, "0x", 3);
                strncat(rm, disp, 10);
            }
            strncat(rm, "]", 1);

            break;

        case 5:

            sprintf(rm, "[%s", _16bit_reg[DI]);
            if(ea.disp) {
                strncat(rm, "+", 1);
                my_itoa(ea.disp, disp);
                strncat(rm, "0x", 3);
                strncat(rm, disp, 10);
            }
            strncat(rm, "]", 1);

            break;

        case 6:

            if(!arch.mod) {
                sprintf(rm, "[0x%X]", ea.disp);
            } else {

                sprintf(rm, "[%s", _16bit_reg[DI]);
                if(ea.disp) {
                    strncat(rm, "+", 1);
                    my_itoa(ea.disp, disp);
                    strncat(rm, "0x", 3);
                    strncat(rm, disp, 10);
                }

                strncat(rm, "]", 1);
            }

            break;

        case 7:

            sprintf(rm, "[%s", _16bit_reg[BX]);
            if(ea.disp) {
                strncat(rm, "+", 1);
                my_itoa(ea.disp, disp);
                strncat(rm, "0x", 3);
                strncat(rm, disp, 10);
            }
            strncat(rm, "]", 1);

            break;
    }

    if(arch.d) {
        sprintf(result, "%s %s, %s", instruction_sets[ins->type], reg, rm);
    } else {
        sprintf(result, "%s %s, %s", instruction_sets[ins->type], rm, reg);
    }
    
    return result;
}

void my_itoa(uint16_t num, char* str) {
    sprintf(str, "%X", num);
}