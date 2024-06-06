#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(void) {
    FILE* file = fopen("test/main.bin", "w+");
    if(!file) {
        printf("Failed to open file\n");
        return -1;
    }

    // 0x8800~0x8BFF
    // 0xC600(00)~0xC7FF(FFFF)
    uint16_t op = 0xC600;
    uint16_t data;
    for(data = 0x00; op <= 0xC7FF; data++) {
        fwrite(&op, 2, 1, file);
        fwrite(&data, 2, 1, file);
        if(data == 0xFFFF) {
            data = 0;
            op++;
        }
    }

    fclose(file);
    return 0;
}

int dpr(void) {
    FILE* file = fopen("test/main.bin", "r");
    if(!file) {
        printf("Failed opened file\n");
        return -1;
    }

    uint8_t buffer[4];
    fread(buffer, 2, 1, file);
    
    printf("0x%02X 0x%02X\n", buffer[0], buffer[1]);

    fclose(file);
    return 0;
}