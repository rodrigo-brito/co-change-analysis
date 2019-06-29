
#ifndef EntireFileMAlloc
#include "memory.h"
#define EntireFileMAlloc(size) malloc(size)
#endif

#ifndef EntireFileMFree
#include "memory.h"
#define EntireFileMFree(file) free(file)
#endif

struct EntireFile {
    char* content;
    int size;
};

void loadEntireFile(EntireFile* entireFile, FILE* file) {
    fseek(file, 0, SEEK_END);
    int fsize = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    entireFile->content = (char*) EntireFileMAlloc(fsize+1);
    fread(entireFile->content, fsize, 1, file);
    fclose(file);

    entireFile->content[fsize] = 0;
    entireFile->size = fsize;
}

bool loadEntireFile(EntireFile* entireFile, const char* formatString, ...) {
    char path[1024];
    va_list argList;
    va_start(argList, formatString);
    vsprintf(path, formatString, argList);
    va_end(argList);
    
    FILE* f = fopen(path, "rb");
    
    if (f == nullptr) {
        printf("Failed to load file %s\n", path);
        return false;
    }
    
    loadEntireFile(entireFile, f);
    return true;
}

void freeEntireFile(EntireFile* entireFile) {
    EntireFileMFree(entireFile->content);
    entireFile->content = nullptr;
    entireFile->size = 0;
}
