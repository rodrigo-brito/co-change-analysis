
struct FileWriter {
    char* buffer;
    int at;
};

void writeInFile(FileWriter* fileWriter, void* p, int byteCount) {
    memcpy(&fileWriter->buffer[fileWriter->at], p, byteCount);
    fileWriter->at += byteCount;
}

void writeInFile(FileWriter* fileWriter, int value) {
    memcpy(&fileWriter->buffer[fileWriter->at], &value, sizeof(int));
    fileWriter->at += sizeof(int);
}

void writeInFile(FileWriter* fileWriter, char* string, int length) {
    writeInFile(fileWriter, length);
    memcpy(&fileWriter->buffer[fileWriter->at], string, length);
    fileWriter->at += length;
}

void writeInFile(FileWriter* fileWriter, const char* cstr) {
    writeInFile(fileWriter, (char*) cstr, strlen(cstr));
}

// @note: see prj/1mac
void writeTextInFileVA(FileWriter* writer, const char* formatString, va_list argList) {
    writer->at += vsprintf(&writer->buffer[writer->at], formatString, argList);
}

void writeTextInFile(FileWriter* writer, const char* formatString, ...) {
    va_list argList;
    va_start(argList, formatString);
    writeTextInFileVA(writer, formatString, argList);
    va_end(argList);
}

void saveFile(FileWriter writer, const char* formatString, ...) {
    char path[1024];
    va_list argList;
    va_start(argList, formatString);
    vsprintf(path, formatString, argList);
    va_end(argList);
    FILE* file = fopen(path, "w");
    fwrite(writer.buffer, 1, writer.at, file);
    fclose(file);
}

void appendFile(FileWriter writer, FILE* file) {
    fwrite(writer.buffer, 1, writer.at, file);
}
    
void appendFile(FileWriter writer, const char* formatString, ...) {
    char path[1024];
    va_list argList;
    va_start(argList, formatString);
    vsprintf(path, formatString, argList);
    va_end(argList);
    FILE* file = fopen(path, "a");
    fwrite(writer.buffer, 1, writer.at, file);
    fclose(file);
}
