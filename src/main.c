#include <stdio.h>
#include "lib/instruction.h"

int main(int argc, char* argv[]) {

    if(argc != 2) {
        printf("Error syntax, ./<app_name> <binary_file>\n");
        return -1;
    }

    (void) disassemble(0b1000101101101011);

    return 0;
}