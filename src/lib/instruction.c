#include "instruction.h"
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

Instruction table[] = { // Instruction Set Table
    {.instc = MOV_REGISTER, .type = MOV, .skip = 2, .len = 6},
    {MOV_IM_REG_MEM, MOV, 3, 7},
    {MOV_IM_REGISTER, MOV, 2, 4},
    {MEM_TO_ACC, MOV, 3, 7},
    {ACC_TO_MEM, MOV, 3, 7},
    {MOV_TO_SR, MOV, 2, 8},
    {SR_TO_MEMREG, MOV, 2, 8},

    {PUSH_REG_MEM, PUSH, 2, 8},
    {PUSH_REG, PUSH, 1, 5},
    {PUSH_SGMT_REG, PUSH, 1, 8},

    {POP_REG_MEM, POP, 2, 8}, //10
    {POP_REG, POP, 1, 5},
    {POP_SGMT_REG, POP, 1, 8},

    {ADD_REGMEM_REG, ADD, 2, 6},
    {ADD_IMDT_REG, ADD, 3, 6},
    {ADD_IMDT_ACCUMUL, ADD, 2, 7},

    {XCHG_REGMEM_REG, XCHG, 2, 7},
    {XCHG_ACCUMUL_REG, XCHG, 1, 5},

    {LONG_JUMP, JMP, 3, 8},
    {SHORT_JUMP, JMP, 2, 8},
    {WITHIN_SEGMENT, JMP, 2, 8}, // 20
    {DRCT_INTRSGMT, JMP, 5, 8},
    {INDRCT_INTRSGMT, JMP, 2, 8},

    {IN_FIXED_PORT, IN, 2, 7},
    {IN_VAR_PORT, IN, 1, 7},

    {OUT_FIXED_PORT, OUT, 2, 7},
    {OUT_FIXED_PORT, OUT, 1, 7},

    {BYTE_TO_AL, XLATB, 1, 8},
    {LOAD_EA_TO_REG, LEA, 2, 8},
    {LOAD_PTR_TO_DS, LDS, 2, 8},
    {LOAD_PTR_TO_ES, LES, 2, 8}, //30
    {LOAD_AH_FLAGS, LAHF, 1, 8},
    {SAVE_AH_FLAGS, SAHF, 1, 8},
    {PUSH_FLAGS, PUSHF, 1, 8},
    {POP_FLAGS, POPF, 1, 8},

    {ADDC_REGMEM, ADC, 2, 6},
    {ADDC_IMDT_REGMEM, ADC, 3, 6},
    {ADDC_IMDT_ACCUMUL, ADC, 2, 7},
};

char* instruction_sets[] = { "mov", "push", "pop", "add", "xchg", "jmp", "in", "out", 
    "xlatb", "lea", "lds", "les", "lahf", "sahf", "pushf", "popf", "adc" };
char* _16bit_reg[] = { "ax", "cx", "dx", "bx", "sp", "bp", "si", "di" };
char* _8bit_reg[] = { "al", "cl", "dl", "bl", "ah", "ch", "dh", "bh" };
char* _segment_reg[] = { "es", "cs", "ss", "ds" };

uint64_t ip = 0; // instruction pointer;

#define TABLE_SIZE sizeof(table)/sizeof(Instruction)

uint8_t remove_center(uint8_t value, Instruction ins) {
    uint8_t result = 0;
    switch(ins.instc) {
        case POP_SGMT_REG:
        case PUSH_SGMT_REG:
            result = value & 0xE7;
            return result;
        default:
            return value;
    }
}

Instruction identify(uint8_t firstByte) {
    Instruction ins;
    ins.instc = 0;
    ins.skip = 0;
    ins.type = NONE;
    uint8_t mask = 0;
    uint8_t mesh = 0;

    for(size_t i = 0; i < TABLE_SIZE; i++) {
        mesh = remove_center(firstByte, table[i]);
        mask = (2 << (table[i].len-1)) - 1;
        mask <<= (8-table[i].len);
        mesh &= mask;
        if(mesh == table[i].instc) {
            ins = table[i];
            return ins;
        }
    }

    return ins;
}

void pointer_wrapper(char a[]) {
    uint8_t zero_position = 0;
    while(a[zero_position]) {
        zero_position++;
    }
    for(uint8_t i = zero_position; i > 0; i--) {
        a[zero_position+1] = a[zero_position];
    }

    a[0] = '[';
    a[zero_position+1] = ']';
    a[zero_position+2] = 0;
    
}

uint32_t analyse(uint8_t* buffer, size_t BUFFER_SIZE) {
    size_t position = 0;
    uint8_t delta = 0;

    Instruction ins;
    #ifdef DEBUG
    printf("First opcode 0x%02X\n", buffer[0]);
    #endif

    char* instruction_string;
    Arch a;

    while(position < BUFFER_SIZE) {
        printf("0x%04zX\t", position);
        memset(&a, 0, sizeof(Arch)); // clear structure every cycle
        delta = 0;
        ins = identify(buffer[position]);
        switch(ins.instc) {

            case PUSH_FLAGS:
            case POP_FLAGS:
            case LOAD_AH_FLAGS:
            case BYTE_TO_AL:
            case SAVE_AH_FLAGS:
                printf("%s\n", instruction_sets[ins.type]);
                break;

            case MOV_REGISTER:

                a.w = buffer[position] & 0x01;
                a.w = !!a.w;
                a.d = buffer[position] & 0x02;
                a.d = !!a.d;
                a.mod = (buffer[position+1] >> 6) & 0x03;
                a.reg = (buffer[position+1] >> 3) & 0x07;
                a.rm = buffer[position+1] & 0x07;
                a.immediate = false;

                a.config |= W | D | MOD | REG | RM;

                instruction_string = build_string(&ins, a);
                printf("%s\n", instruction_string);

                free(instruction_string);

                break;

            case MOV_IM_REG_MEM:

                a.w = buffer[position] & 0x01;
                a.w = !!a.w;
                a.immediate = true;
                a.mod = (buffer[position+1] >> 6) & 0x03;
                a.rm = buffer[position+1] & 0x07;
                a.data = buffer[position+2];
                if(a.w) {
                    a.data_ifw = buffer[position+3];
                    delta = 1;
                }
                a.config |= W | D | MOD | RM | DATA;

                instruction_string = build_string(&ins, a);
                printf("%s\n", instruction_string);

                free(instruction_string);

                break;

            case MOV_IM_REGISTER:

                a.w = buffer[position] & 0x08;
                a.w = !!a.w;
                a.reg = buffer[position] & 0x07;
                a.data = buffer[position+1];
                a.immediate = true;
                if(a.w) {
                    a.data_ifw = buffer[position+2];
                    delta = 1;
                }
                a.config |= W | D | REG | DATA;

                instruction_string = build_string(&ins, a);
                printf("%s\n", instruction_string);

                free(instruction_string);
                break;

            case MEM_TO_ACC:

                a.w = buffer[position] & 0x01;
                a.addrlow = buffer[position+1];
                a.addrhigh = buffer[position+2];

                a.reg = 0;
                a.d = 1;
                
                a.config |= W | ADDR | REG | D;

                instruction_string = build_string(&ins, a);
                printf("%s\n", instruction_string);

                free(instruction_string);
                break;

            case ACC_TO_MEM:

                a.w = buffer[position] & 0x01;
                a.addrlow = buffer[position+1];
                a.addrhigh = buffer[position+2];

                a.reg = 0;

                a.config |= W | ADDR | REG;

                instruction_string = build_string(&ins, a);
                printf("%s\n", instruction_string);

                free(instruction_string);
                break;

            case MOV_TO_SR:
                a.d = 1;

            case SR_TO_MEMREG:

                a.w = 1;
                a.mod = (buffer[position+1] & 0xC0) >> 6;
                a.reg = (buffer[position+1] & 0x18) >> 3;
                a.rm = buffer[position+1] & 0b111;
                a.segment = true;

                a.config |= MOD | REG | RM;

                instruction_string = build_string(&ins, a);
                printf("%s\n", instruction_string);

                free(instruction_string);
                break;


            // ADD Instruction
            case ADD_REGMEM_REG:

                a.d = buffer[position] & 0x02;
                a.d = !!a.d;
                a.w = buffer[position] & 0x01;

                a.mod = (buffer[position+1] & 0xC0) >> 6;
                a.reg = (buffer[position+1] & 0x38) >> 3;
                a.rm = buffer[position+1] & 0x07;

                a.config |= W | D | MOD | REG | RM;

                instruction_string = build_string(&ins, a);
                printf("%s\n", instruction_string);

                free(instruction_string);
                break;

            case ADD_IMDT_REG:

                a.sw = buffer[position] & 0x03;
                a.mod = (buffer[position+1] & 0xC0) >> 6;
                a.rm = buffer[position+1] & 0x07;
                a.data = buffer[position+2];
                a.reg = (buffer[position+1] & 0x38) >> 3;
                if(a.reg == 2) {
                    ins = table[36];
                    a.w = 1;
                }
                a.reg = 0;

                if(a.sw == 1) {
                    a.w = 1;
                    a.data_ifw = buffer[position+3];
                    delta = 1; //16 bit
                }

                a.config |= MOD | RM | DATA | SW;

                instruction_string = build_string(&ins, a);
                printf("%s\n", instruction_string);

                free(instruction_string);
                break;

            case ADD_IMDT_ACCUMUL:

                a.w = buffer[position] & 0x01;
                a.data = buffer[position+1];
                a.reg = 0;

                if(a.w) {
                    a.data_ifw = buffer[position+2];
                    delta = 1;
                    a.config |= W;
                }

                a.config |= REG | DATA;

                instruction_string = build_string(&ins, a);
                printf("%s\n", instruction_string);

                free(instruction_string);
                break;

            case PUSH_REG_MEM:

                a.mod = (buffer[position+1] & 0xC0) >> 6;
                a.reg = (buffer[position+1] & 0x38) >> 3;
                if(a.reg == 4) {
                    a.w = 1;
                    ins = table[20];
                }
                if(a.reg == 5) {
                    ins = table[22];
                }
                a.reg = 0;
                a.rm = buffer[position+1] & 0x07;

                a.config |= MOD | RM;

                instruction_string = build_string(&ins, a);
                printf("%s\n", instruction_string);

                free(instruction_string);
                break;

            case PUSH_REG:

                a.reg = buffer[position] & 0x07;
                a.w = 1;

                a.config |= REG;

                instruction_string = build_string(&ins, a);
                printf("%s\n", instruction_string);

                free(instruction_string);
                break;

            case PUSH_SGMT_REG:

                a.reg = (buffer[position] & 0x18) >> 3;
                a.segment = true;

                a.config |= REG;

                instruction_string = build_string(&ins, a);
                printf("%s\n", instruction_string);

                free(instruction_string);
                break;

            case POP_REG_MEM:

                a.mod = (buffer[position+1] & 0xC0) >> 6;
                a.rm = buffer[position+1] & 0x07;
                a.w = 1;

                a.config |= MOD | RM;

                instruction_string = build_string(&ins, a);
                printf("%s\n", instruction_string);

                free(instruction_string);
                break;

            case POP_REG:

                a.reg = buffer[position] & 0x07;
                a.w = 1;

                a.config |= REG;

                instruction_string = build_string(&ins, a);
                printf("%s\n", instruction_string);

                free(instruction_string);
                break;

            case POP_SGMT_REG:

                a.reg = (buffer[position] & 0x18) >> 3;
                a.segment = true;

                a.config |= REG;

                instruction_string = build_string(&ins, a);
                printf("%s\n", instruction_string);

                free(instruction_string);
                break;

            case XCHG_REGMEM_REG:
                //mod reg rm
                a.w = buffer[position] & 0x01;
                a.mod = (buffer[position+1] & 0xC0) >> 6;
                a.reg = (buffer[position+1] & 0x38) >> 3;
                a.rm = buffer[position+1] & 0x07;

                a.config |= MOD | REG | RM;

                instruction_string = build_string(&ins, a);
                printf("%s\n", instruction_string);

                free(instruction_string);
                break;

            case XCHG_ACCUMUL_REG:

                a.w = 1;
                a.d = 1;
                a.rm = buffer[position] & 0x07;
                a.mod = 0x03;
                a.reg = 0;
                
                a.config |= REG | RM | MOD;

                instruction_string = build_string(&ins, a);
                printf("%s\n", instruction_string);

                free(instruction_string);
                break;

            case LONG_JUMP:

                a.displow = buffer[position+1];
                a.disphigh = buffer[position+2];

                instruction_string = build_string(&ins, a);
                printf("%s\n", instruction_string);

                free(instruction_string);
                break;

            case SHORT_JUMP:

                a.displow = buffer[position+1];

                instruction_string = build_string(&ins, a);
                printf("%s\n", instruction_string);

                free(instruction_string);
                break;

            case DRCT_INTRSGMT:

                a.displow = buffer[position+1];
                a.disphigh = buffer[position+2];
                a.addrlow = buffer[position+3];
                a.addrhigh = buffer[position+4];

                a.config |= DIRECT_INTERSEGMENT;

                instruction_string = build_string(&ins, a);
                printf("%s\n", instruction_string);

                free(instruction_string);
                break;

            case IN_FIXED_PORT:

                a.w = buffer[position] & 0x01;
                a.d = 1;
                a.data = buffer[position + 1];

                a.config |= PORT | REG;

                instruction_string = build_string(&ins, a);
                printf("%s\n", instruction_string);

                free(instruction_string);
                break;

            case IN_VAR_PORT:

                a.w = buffer[position] & 0x01;
                a.mod = 3;
                a.d = 1;
                a.rm = 2;
                a.reg = 0;

                a.config |= RM | MOD | REG;

                instruction_string = build_string(&ins, a);
                printf("%s\n", instruction_string);

                free(instruction_string);
                break;

            case OUT_FIXED_PORT:

                a.w = buffer[position] & 0x01;
                a.d = 1;
                a.data = buffer[position + 1];

                a.config |= PORT | REG;

                instruction_string = build_string(&ins, a);
                printf("%s\n", instruction_string);

                free(instruction_string);
                break;

            case OUT_VAR_PORT:

                a.w = buffer[position] & 0x01;
                a.mod = 3;
                a.d = 1;
                a.rm = 2;
                a.reg = 0;

                a.config |= RM | MOD | REG;

                instruction_string = build_string(&ins, a);
                printf("%s\n", instruction_string);

                free(instruction_string);
                break;
            
            case LOAD_PTR_TO_ES:
            case LOAD_PTR_TO_DS:
            case LOAD_EA_TO_REG:

                //mod reg rm
                a.mod = (buffer[position+1] & 0xC0) >> 6;
                a.reg = (buffer[position+1] & 0x38) >> 3;
                a.rm = buffer[position+1] & 0x07;
                a.w = 1;
                a.d = 1;

                a.config |= RM | REG | MOD;

                instruction_string = build_string(&ins, a);
                printf("%s\n", instruction_string);

                free(instruction_string);
                break;

            case ADDC_REGMEM:

                a.w = buffer[position] & 0x01;
                a.d = buffer[position] & 0x02;
                a.d = !!a.d;
                a.mod = (buffer[position+1] & 0xC0) >> 6;
                a.reg = (buffer[position+1] & 0x38) >> 3;
                a.rm = buffer[position+1] & 0x07;

                a.config |= MOD | REG | RM;

                instruction_string = build_string(&ins, a);
                printf("%s\n", instruction_string);

                free(instruction_string);
                break;

            case ADDC_IMDT_ACCUMUL:

                a.w = buffer[position] & 0x01;
                a.data = buffer[position+1];
                if(a.w) {
                    a.data_ifw = buffer[position+2];
                    delta = 1;
                }

                a.config |= REG | DATA;

                instruction_string = build_string(&ins, a);
                printf("%s\n", instruction_string);

                free(instruction_string);
                break;

            default:
                delta = 1;
                break;
        }

        position += ins.skip + delta;
        ip = position;
    }
    
    return 0;
}

char* build_string(Instruction* ins, Arch arch) {
    
    EA ea;

    char source[15]; memset(source, 0, 15);
    char destination[15]; memset(destination, 0, 15);

    char* result = (char*) malloc(40);
    char rm[15]; memset(rm, 0, 15);
    char reg[10]; memset(reg, 0, 10);
    char disp[10]; memset(disp, 0, 10);

    
    if(arch.config & REG){
        if(arch.segment)
            sprintf(reg, "%s", _segment_reg[arch.reg]);
        else
            sprintf(reg, "%s", (arch.w)?_16bit_reg[arch.reg]:_8bit_reg[arch.reg]);
    }

    if(arch.config & ADDR) {
        uint16_t addr = arch.addrhigh;
        addr <<= 8;
        addr |= arch.addrlow;
        sprintf(rm, "[0x%X]", addr);
    }

    if(arch.w && arch.config & DATA) {
        uint16_t flow = arch.data_ifw;
        flow <<= 8;
        flow |= arch.data;

        sprintf(source, "0x%X", flow);
    } else {
        sprintf(source, "0x%X", arch.data);
    }

    // if(arch.sw == 3 && arch.config & SW) {
    //     sprintf(source, "%d", (int8_t) arch.data);
    // } uncomment for decimal representing only

    if(arch.config & MOD)
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
    if(arch.config & RM)
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

    if(arch.config & DATA) {
        if(arch.config & RM) {
            strcpy(destination, rm);
        } else if(arch.config & REG) {
            strcpy(destination, reg);
        }
    } else {
        strcpy(destination, rm);
        strcpy(source, reg);
    }

    if(arch.config & DST_MEM) {
        pointer_wrapper(destination);
    }

    if(arch.config & PORT) {
        sprintf(destination, "0x%02X", arch.data);
        
    }

    uint16_t jumping_addr = 0x00;
    uint16_t jumping_segment = 0x00;
    if(ins->type == JMP) {
        switch(ins->instc) {
            case LONG_JUMP:

                jumping_addr = arch.disphigh;
                jumping_addr <<= 8;
                jumping_addr |= arch.displow;

                jumping_addr = ip + ins->skip + jumping_addr;

                sprintf(destination, "0x%04X", jumping_addr);

                break;

            case SHORT_JUMP:

                jumping_addr = arch.displow;
                jumping_addr = ip + ins->skip + (int16_t) jumping_addr;

                sprintf(destination, "0x%02X", jumping_addr);

                break;

            case DRCT_INTRSGMT:

                jumping_addr = arch.displow;
                jumping_addr |= arch.disphigh << 8;

                jumping_segment = arch.addrlow;
                jumping_segment |= arch.addrhigh << 8;

                sprintf(destination, "0x%04X:0x%04X", jumping_segment, jumping_addr);

                break;

            default:
                break;
        }
    }

    switch(ins->type) {
        
        case LES:
        case LDS:
        case LEA:
        case OUT:
        case IN:
        case XCHG:
        case ADD:
        case ADC:
        case MOV:
            if(arch.d) {
                sprintf(result, "%s %s, %s", instruction_sets[ins->type], source, destination);
            } else {
                sprintf(result, "%s %s, %s", instruction_sets[ins->type], destination, source);
            }

            break;

        case POP:
        case PUSH:

            if(arch.config & RM)
                sprintf(result, "%s %s", instruction_sets[ins->type], destination);
            else
                sprintf(result, "%s %s", instruction_sets[ins->type], source);

            break;

        case JMP:

            sprintf(result, "%s%s %s", instruction_sets[ins->type], 
                (ins->instc==SHORT_JUMP)?" short":"", destination);

            break;

        default:
            sprintf(result, "");
            break;
    }
    
    return result;
}

void my_itoa(uint16_t num, char* str) {
    sprintf(str, "%X", num);
}