#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "RLEList.h"


#define UNDEFINED -1
#define STR_FORMAT_BASE_LEN 2
#define NEW_LINE_ASCII '\n'
#define NULL_TERMINATION '\0'
#define ZERO_ASCII '0'
#define DECIMAL_BASE 10

struct RLEList_t {
    char value;
    int len;
    struct RLEList_t* next;
};

static int RLEListNodeNumber(RLEList list) {
    int nodesInList = 0;

    RLEList tempList = list;

    while(tempList) {
        nodesInList++;
        tempList = tempList->next;
    }
    return nodesInList;
}

static int getNumDigits(int num) {
    int counter = 1;
    int base = DECIMAL_BASE;
    
    while(num/base) {
        counter++;
        base*=DECIMAL_BASE;
    }
    
    return counter;
}

static int getRLEStringLength(RLEList list) {
    int RLEStrLength = (RLEListNodeNumber(list) * STR_FORMAT_BASE_LEN);
    RLEList tmpList = list;
    while (tmpList) {
        RLEStrLength += getNumDigits(tmpList->len);
        tmpList = tmpList->next;
    }
    return RLEStrLength;
}

static void RLENodeToString(RLEList list, char* outputStr) {
    int nodeNumOfDigits = getNumDigits(list->len);
    int strSize = nodeNumOfDigits + STR_FORMAT_BASE_LEN;
    outputStr[0] = list->value;
    outputStr[strSize - 1] = NEW_LINE_ASCII;
    int nodeCurrentDigit = list->len;

    for (int i = 1; i <= nodeNumOfDigits; i++) {
        outputStr[strSize - 1 - i] = (nodeCurrentDigit % DECIMAL_BASE) + ZERO_ASCII;
        nodeCurrentDigit /= DECIMAL_BASE;
    }
}

static void compressDoubleAppearances(RLEList lastNode, RLEList currentNode) {
    assert(lastNode);
    assert(currentNode);
    
    lastNode->len += currentNode->len;
    lastNode->next = currentNode->next;
    free(currentNode);
}

RLEList RLEListCreate() {
    RLEList list = malloc(sizeof(*list));
    
    if(list == NULL) {
        return NULL;
    }
    
    list->value = 0;
    list->len = 0;
    list->next = NULL;
    
    return list;
}

void RLEListDestroy(RLEList list) {
    RLEList currentNodeToDestroy;
    while(list) {
        currentNodeToDestroy = list;
        list = list->next;
        free(currentNodeToDestroy);
    }
}

RLEListResult RLEListAppend(RLEList list, char value) {
    if(!list || !value) {
        return RLE_LIST_NULL_ARGUMENT;
    }
    
    RLEList tempList = list;
    
    while(tempList->next) {
        tempList = tempList->next;
    }
    
    if(tempList->value && tempList->value == value) {
        tempList->value = value;
        tempList->len++;
        return RLE_LIST_SUCCESS;
    }
    
    RLEList newList = RLEListCreate();
    
    if(!newList) {
        return RLE_LIST_OUT_OF_MEMORY;
    }
    
    newList->value = value;
    newList->len = 1;
    newList->next = NULL;
    tempList->next = newList;
    
    return RLE_LIST_SUCCESS;
}

int RLEListSize(RLEList list) {
    if(!list) {
        return UNDEFINED;
    }
        
    RLEList tempList = list;
    int listCharacterNumber = 0;

    while (tempList) {
        listCharacterNumber += tempList->len;
        tempList = tempList->next;
    }

    return listCharacterNumber;
}


RLEListResult RLEListRemove(RLEList list, int index) {
    if(!list) {
        return RLE_LIST_NULL_ARGUMENT;
    }

    if(index < 0 || index >= RLEListSize(list)) {
        return RLE_LIST_INDEX_OUT_OF_BOUNDS;
    }

    assert(!list->value);

    RLEList lastNode = list;
    RLEList tempList = list->next;

    int lastNum = 0;
    for (int i = 0; i + tempList->len <= index; i += lastNum) {
        lastNode = tempList;
        lastNum = tempList->len;
        tempList = tempList->next;
    }

    if(tempList->len == 1) {
        lastNode->next = tempList->next;
        free(tempList);
        
        tempList = lastNode->next;
        if(tempList && lastNode->value == tempList->value) {
            compressDoubleAppearances(lastNode, tempList);
        }
    }
    else {
        tempList->len--;
    }

    return RLE_LIST_SUCCESS;
}



char RLEListGet(RLEList list, int index, RLEListResult *result) {
    if(!list) {
        if(result) {
            *result = RLE_LIST_NULL_ARGUMENT;
        }
        
        return 0;
    }

    if(index < 0 || index >= RLEListSize(list)) {
        if(result) {
            *result = RLE_LIST_INDEX_OUT_OF_BOUNDS;
        }

        return 0;
    }

    RLEList tempList = list;
    int lastNum = 0;
    for (int i = 0; i + tempList->len <= index; i += lastNum) {
        lastNum = tempList->len;
        tempList = tempList->next;
    }
    
    if(result) {
        *result = RLE_LIST_SUCCESS;
    }

    return tempList->value;
}

RLEListResult RLEListMap(RLEList list, MapFunction map_function) {
    if(!list || !map_function) {
        return RLE_LIST_NULL_ARGUMENT;
    }
    
    RLEList tempList = list->next;
    RLEList lastNode = list;
    char mappedValue = 0;
    
    while(tempList) {
        mappedValue = map_function(tempList->value);
        
        if(lastNode->value == mappedValue) {
            compressDoubleAppearances(lastNode, tempList);
        }
        else {
            tempList->value = mappedValue;
            lastNode = tempList;
        }
        
        tempList = lastNode->next;
    }
  
    return RLE_LIST_SUCCESS;
}

char* RLEListExportToString(RLEList list, RLEListResult* result) {
    if(!list) {
        if(result) {
            *result = RLE_LIST_NULL_ARGUMENT;
        }
        
        return NULL;
    }

    assert(!list->value);
    
    RLEList tempList = list->next;
    
    int listLen = getRLEStringLength(tempList);
    char* exportedString = malloc(listLen + 1);
    
    if(!exportedString) {
        if (result) {
            *result = RLE_LIST_OUT_OF_MEMORY;
        }
        
        return NULL;
    }

    exportedString[listLen] = NULL_TERMINATION;
    char* tmpStr = exportedString;
    
    

    while(tempList) {
        RLENodeToString(tempList, tmpStr);
        tmpStr += getNumDigits(tempList->len) + STR_FORMAT_BASE_LEN;
        tempList = tempList->next;
    }

    if (result) {
        *result = RLE_LIST_SUCCESS;
    }

    return exportedString;
}
