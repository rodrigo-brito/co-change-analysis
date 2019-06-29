
// @todo: implement small string optimization

#ifndef StringMAlloc
#include "memory.h"
#define StringMAlloc(size) malloc(size)
#endif

#ifndef StringMFree
#include "memory.h"
#define StringMFree(pointer) free(pointer)
#endif

#include "string.h"

#define Str()                         _mallocString(0)
#define StrAlloc(size)                _mallocString(size)
#define StrSize(string)               (*((int*) ((string) - sizeof(int))))
#define StrCopy(dest, source, len)    _copyString((char*)dest, (char*)(source), len)
#define StrCreateCopy(source)         _createStringCopy((char*)source)
#define StrCreateCCopy(c_str)         _createCStringCopy((char*)c_str)
#define StrClear(string)              _clearString(&(string))
#define StrFree(string)               _freeString(&(string))
#define StrAddChar(string, c)         _addCharToString(&(string), c)
#define StrAppend(string, toAppend)   _appendToString(&(string), toAppend)
#define StrCompare(str1, str2)        _compareStrings(str1, str2)
#define StrCCompare(str1, str2)       _compareStringAndCString(str1, (char*) str2)
#define StrCmp(str1, str2)            strcmp(str1, str2) == 0
#define StrReplace(outBuffer, inBuffer, inLength, searchedString, searchedLength, replacementString, replacementLength) \
    _replace(outBuffer, (char*) inBuffer, inLength, (char*) searchedString, searchedLength, (char*) replacementString, replacementLength)

char* _mallocString(int size) {
    char* stringHeader = (char*) malloc(sizeof(int) + size*sizeof(char) + 1);
    *((int*) stringHeader) = size;
    (stringHeader + sizeof(int))[size] = 0;
    return (stringHeader + sizeof(int));
}

void _copyString(char* string, char* source, int sourceLength) {
    assert(StrSize(string) >= sourceLength);
    memcpy(string, source, sourceLength * sizeof(char));
    string[sourceLength] = 0;
    StrSize(string) = sourceLength;
}

char* _createStringCopy(char* string) {
    int length = StrSize(string);
    char* stringCopy = _mallocString(length);
    memcpy(stringCopy, string, length * sizeof(char));
    stringCopy[length] = 0;
    return stringCopy;
}

char* _createCStringCopy(char* c_str) {
    int length = 0;
    char* c = c_str;
    while (*c != 0) {
        c++; 
        length++;
    }
    char* string = _mallocString(length);
    memcpy(string, c_str, length * sizeof(char));
    string[length] = 0;
    return string;
}

void _freeString(char** string) {
    StringMFree((int*)(*string) - 1);
    *string = 0;
}

void _clearString(char** string) {
    char* newString = Str();
    _freeString(string);
    *string = newString;
}

void _addCharToString(char** string, char c) {
    int newSize = StrSize(*string) + 1;
    char* newString = _mallocString(newSize);
    memcpy(newString, *string, StrSize(*string) * sizeof(char));
    newString[newSize-1] = c;
    newString[newSize] = 0;
    _freeString(string);
    *string = newString;
}

void _appendToString(char** string, const char* toAppend) {
    int currentSize = StrSize(*string);
    //int newSize = StrSize(*string) + StrSize(toAppend);
    int newSize = StrSize(*string) + strlen(toAppend);
    char* newString = _mallocString(newSize);
    memcpy(newString, *string, StrSize(*string) * sizeof(char));
    //memcpy(&newString[currentSize], toAppend, StrSize(toAppend) * sizeof(char));
    memcpy(&newString[currentSize], toAppend, strlen(toAppend) * sizeof(char));
    newString[newSize] = 0;
    _freeString(string);
    *string = newString;
}

bool _compareStrings(const char* str1, const char* str2) {
    if (StrSize(str1) != StrSize(str2))
        return false;
    
    while (*str1 && *str2 && *str1 == *str2) {
        str1++;
        str2++;
    }
    
    return *str1 == *str2;
}

bool _compareStringAndCString(char* str1, char* str2) {
    int str2Size = 0;
    char* _str2 = str2;
    while (*_str2) {
        str2Size++;
        _str2++;
    }
    
    if (StrSize(str1) != str2Size)
        return false;
    
    while (*str1 && *str2 && *str1 == *str2) {
        str1++;
        str2++;
    }
    
    return *str1 == *str2;
}

int _replace(char* outBuffer, char* inBuffer, int inLength, char* searchedString, int searchedLength, char* replacementString, int replacementLength) {
    int replaceCount = 0;
    int i = 0;
    int j = 0;
    while (i < inLength) {
        if (memcmp(&inBuffer[i], searchedString, searchedLength) == 0) {
            memcpy(&outBuffer[j], replacementString, replacementLength);
            i += searchedLength;
            j += replacementLength;
            ++replaceCount;
        } else {
            outBuffer[j] = inBuffer[i];
            ++j;
            ++i;
        }
    }
    outBuffer[j] = 0;
    return j;
}





