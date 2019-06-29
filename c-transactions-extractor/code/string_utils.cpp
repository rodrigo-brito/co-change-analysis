
int concatenate(char* buffer, char** strings, const char* separator) {
    int k = 0;
    int separatorLength = strlen(separator);
    int i = 0;
    for (i = 0; i < ArraySize(strings)-1; ++i) {
        int length = strlen(strings[i]);
        for (int c = 0; c < length; ++c) {
            buffer[k++] = strings[i][c];
        }
        memcpy(&buffer[k], separator, separatorLength);
        k += separatorLength;
    }

    int length = strlen(strings[i]);
    for (int c = 0; c < length; ++c) {
        buffer[k++] = strings[i][c];
    }
    buffer[k] = 0;
    return k;
}

char** splitString(char* string, int len, char separator) {
    char* currentString = Str();
    char** strings = Array(char*, 0);
    char* at = string;
    for (int i = 0; i < len; ++i) {
        if (*at == separator && currentString) {
            ArrayAdd(strings, currentString);
            currentString = Str();
        } else {
            StrAddChar(currentString, *at);
        }
        ++at;
    }

    if (currentString) {
        ArrayAdd(strings, currentString);
    } else {
        StrFree(currentString);
    }

    return strings;
}

bool startsWith(char* testString, const char* prefix) {
    if (strlen(testString) < strlen(prefix)) {
        return false;
    }
    
    int i = 0;
    while (testString[i] && prefix[i]) {
        if (testString[i] != prefix[i])
            return false;
        ++i;
    }

    return true;
}

bool endsWith(char* testString, const char* suffix) {
    int i = strlen(testString) - strlen(suffix);
    return startsWith(testString+i, suffix);
}
