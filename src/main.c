#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "lib/instruction.h"

#define CHUNK_SIZE 1024

long getFileSize(FILE* f) {
    fseek(f, 0, SEEK_END);
    long result = ftell(f);
    fseek(f, 0, SEEK_SET);
    return result;
}

int main(int argc, char* argv[]) {

    if(argc != 2) {
        printf("Error syntax, ./<app_name> <binary_file>\n");
        return -1;
    }

    FILE* file = fopen(argv[1], "r");
    if(!file) {
        printf("Opening file failed! try again\n");
        return -1;
    }

    const size_t BUFFER_SIZE = getFileSize(file);
    uint8_t buffer[BUFFER_SIZE];
    
    #ifdef DEBUG
    printf("Reading %lu bytes\n", BUFFER_SIZE);
    #endif
    
    for(size_t i = 0; i < BUFFER_SIZE; i++) {
        
    }

    size_t n = 0;
    size_t bytesRead = 0;
    while((n = fread(buffer, 1, BUFFER_SIZE, file)) > 0) bytesRead += n;
    #ifdef DEBUG
    printf("Bytes read %lu\nFile length: %lu\n", bytesRead, BUFFER_SIZE);
    #endif
    if(bytesRead != BUFFER_SIZE) {
        printf("Error reading file content\n");
        fclose(file);
        return -1;
    }
    (void) analyse(buffer, BUFFER_SIZE);

    fclose(file);
    return 0;
}