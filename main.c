#include <stdio.h>
#include "RLEList.h"
#include "AsciiArtTool.h"

#define FLAG_DASH '-'
#define ENCODED_FLAG 'e'
#define INVERTED_FLAG 'i'
#define EXPECTED_ARGS 3

int main(int argc, char** argv) {
    if(argc != EXPECTED_ARGS+1) {
        // TODO: Should we raise another error here?
        return 0;
    }
    
    char flag = argv[1][1];
    
    if(argv[1][0] != FLAG_DASH || (flag != ENCODED_FLAG && flag != INVERTED_FLAG)) {
        
        return 0;
    }
    
    FILE* source = fopen(argv[2], "r");
    if(!source) {
        return 0;
    }
    
    FILE* target = fopen(argv[3], "w");
    if(!target) {
        fclose(source);
        return 0;
    }
    
    RLEList list = asciiArtRead(source);
    RLEListResult result = RLE_LIST_ERROR;
    
    if(flag == ENCODED_FLAG) {
        result = asciiArtPrintEncoded(list, target);
    }
    else {
        result = asciiArtPrint(list, target);
    }
    
    RLEListDestroy(list);
    
    fclose(source);
    fclose(target);
    
    return result;
}
