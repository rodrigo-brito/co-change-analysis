
struct FileReader {
    char* content;
    int size;
    int at;
    int line;
    bool error;
};

bool isWhiteSpace(char c) {
    return c == ' ' || c == '\n' || c == '\t' || c == '\r';
}

bool isNumber(char c) {
    return c >= '0' && c <= '9';
}

bool isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool isAlphaNum(char c) {
    return isAlpha(c) || isNumber(c);
}

char charAt(FileReader* reader) {
    return reader->content[reader->at];
}

void advance(FileReader* reader, int count = 1) {
    reader->at = max(0, min(reader->at + count, reader->size));
    if (charAt(reader) == '\n') {
        ++reader->line;
    }
}

bool eof(FileReader reader) {
    return reader.at >= reader.size;
}

void skipWhiteSpace(FileReader* reader) {
    while (isWhiteSpace(reader->content[reader->at])) {
        advance(reader);
        if (reader->at >= reader->size)
            break;
    }
}

void skipEntireLine(FileReader* reader) {
    while (reader->content[reader->at] != '\n') {
        advance(reader);
        if (reader->at >= reader->size)
            break;
    }
    advance(reader);
}

void skipUntilCharIsReached(FileReader* reader, char stopChar) {
    while (reader->content[reader->at] != stopChar) {
        advance(reader);
        if (reader->at >= reader->size)
            break;
    }
    advance(reader);
}

char* at(FileReader* reader) {
    return reader->content + reader->at;
}

char peek(FileReader* reader, int count = 1) {
    return reader->content[reader->at+count];
}

void goback(FileReader* reader, int count = 1) {
    advance(reader, -count);
}

bool compare(FileReader reader, const char* cstr) {
    return (reader.size - reader.at == (int)strlen(cstr))
        && (memcmp(reader.content + reader.at, cstr, reader.size - reader.at) == 0);
}

bool startWithReader(FileReader reader, const char* cstr) {
    FileReader begining = reader;
    begining.size = reader.at + min(reader.size-reader.at, (int)strlen(cstr));
    return compare(begining, cstr);
}

FileReader readUntilCharIsReached(FileReader* reader, char stopChar) {
    FileReader token = {};
    token.content = &reader->content[reader->at];
    token.size = 0;
    while (reader->content[reader->at] != stopChar) {
        advance(reader);
        ++token.size;
        if (reader->at >= reader->size)
            return token;
    }
    return token;
}

void readUntilCharIsReached(char* out, FileReader* reader, char stopChar) {
    FileReader slice = readUntilCharIsReached(reader, stopChar);
    sprintf(out, "%.*s", slice.size, slice.content);
}

FileReader readRemainder(FileReader* reader) {
    return readUntilCharIsReached(reader, '\0');
}

void readRemainder(char* out, FileReader* reader) {
    readUntilCharIsReached(out, reader, '\0');
}

FileReader readNextInteger(FileReader* reader) {
    FileReader token = {};
    while (!isNumber(reader->content[reader->at])) {
        advance(reader);
        if (reader->at >= reader->size) {
            return token;
        }
    }
    
    token.content = &reader->content[reader->at];
    token.size = 1;
    
    advance(reader);
    
    while (isNumber(reader->content[reader->at])) {
        advance(reader);
        ++token.size;
        if (reader->at >= reader->size) {
            return token;
        }
    }

    return token;
}

int getNextInteger(FileReader* reader) {
    FileReader token = readNextInteger(reader);
    assert(token.content);
    return strtol(token.content, 0, 10);
}

int getNextIntegerBinary(FileReader* reader) {
    int integer = *((int*)&reader->content[reader->at]);
    reader->at += 4;
    return integer;
}

void getNextBytes(FileReader* reader, void* buffer, int byteCount) {
    if (buffer) {
        memcpy(buffer, &reader->content[reader->at], byteCount);
    }
    reader->at += byteCount;
}
int getNextStringBinary(FileReader* reader, char* buffer=0) {
    int size = getNextIntegerBinary(reader);
    getNextBytes(reader, buffer, size);
    buffer[size] = 0;
    return size;
}



