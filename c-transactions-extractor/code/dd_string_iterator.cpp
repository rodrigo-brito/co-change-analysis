#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

struct DD_StringIterator {
    char* begin;
    int size;
    int at;
    int line;
    int column;
};

typedef bool (*DD_StringIteratorAdvanceConditionCheck)(DD_StringIterator iter);

char& DD_at(DD_StringIterator* iter) {
    return iter->begin[iter->at];
}

char& DD_peek(DD_StringIterator* iter, int count=1) {
    int i = iter->at+count;
    i = i >= 0 ? i:0;
    i = i <= iter->size-1 ? i:iter->size-1;
    return iter->begin[i];
}

bool DDSTRIT_isWhiteSpace(char c) {
    return c == ' ' || c == '\n' || c == '\t' || c == '\r';
}

bool DDSTRIT_isNumber(char c) {
    return c >= '0' && c <= '9';
}

bool DDSTRIT_isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool DDSTRIT_isAlphaNum(char c) {
    return DDSTRIT_isAlpha(c) || DDSTRIT_isNumber(c);
}

bool DD_isWhiteSpace(DD_StringIterator iter) {
    return DDSTRIT_isWhiteSpace(DD_at(&iter));
}

bool DD_isNumber(DD_StringIterator iter) {
    return DDSTRIT_isNumber(DD_at(&iter));
}

bool DD_isAlpha(DD_StringIterator iter) {
    return DDSTRIT_isAlpha(DD_at(&iter));
}

bool DD_isAlphaNum(DD_StringIterator iter) {
    return DDSTRIT_isAlphaNum(DD_at(&iter));
}

bool DD_isAtTheEnd(DD_StringIterator iter) {
    return iter.at >= iter.size-1;
}

bool DD_startsWith(DD_StringIterator* iter, const char* string, int stringSize) {
    return memcmp(&DD_at(iter), string, stringSize) == 0;
}

bool DD_startsWith(DD_StringIterator* iter, const char* string) {
    return DD_startsWith(iter, string, strlen(string));
}

DD_StringIterator DD_createIterator(const char* string) {
    DD_StringIterator result = {};
    result.begin = (char*)string;
    result.size = strlen(string);
    result.line = 1;
    result.column = 1;
    return result;
}

DD_StringIterator DD_createIterator(const char* string, int size) {
    DD_StringIterator result = {};
    result.begin = (char*)string;
    result.size = size;
    result.line = 1;
    result.column = 1;
    return result;
}

int DD_advance(DD_StringIterator* iter, int count=1) {
    if (DD_at(iter) == '\n') {
        ++iter->line;
        iter->column = 1;
    }
    
    int result = count;
    int target = iter->at + count;
    if (target >= iter->size) {
        target = iter->size;
        result = target - iter->at;
    }
    
    iter->at += result;
    iter->column += result;
    return result;
}

int DD_goback(DD_StringIterator* iter, int count=1) {
    for (int i = 0; i < count; ++i) {
        if (DD_at(iter) == '\n') {
            --iter->line;
            DD_StringIterator it = *iter;
            while (it.at > 0 && DD_peek(&it, -1) != '\n') {
                DD_goback(&it);
            }
            iter->column = iter->at - it.at;
        }
    }
    
    int result = count;
    int target = iter->at + count;
    if (target < 0) {
        target = 0;
        result = target - iter->at;
    }
    
    iter->at += result;
    iter->column += result;
    return result;
}

DD_StringIterator DD_advanceWhileCondition(DD_StringIterator* iter, DD_StringIteratorAdvanceConditionCheck advanceCondition, bool include=false) {
    DD_StringIterator result = {};
    result.begin = &DD_at(iter);
    while (advanceCondition(*iter)) {
        ++result.size;
        DD_advance(iter);
        if (DD_isAtTheEnd(*iter))
            return result;
    }
    if (include && result.size) {
        ++result.size;
        DD_advance(iter);
    }
    return result;
}

DD_StringIterator DD_advanceWhileNotCondition(DD_StringIterator* iter, DD_StringIteratorAdvanceConditionCheck advanceCondition, bool include=false) {
    DD_StringIterator result = {};
    result.begin = &DD_at(iter);
    while (!advanceCondition(*iter)) {
        ++result.size;
        DD_advance(iter);
        if (DD_isAtTheEnd(*iter))
            return result;
    }
    if (include && result.size) {
        ++result.size;
        DD_advance(iter);
    }
    return result;
}

DD_StringIterator DD_advanceWhileChar(DD_StringIterator* iter, char stopChar, bool include=false) {
    DD_StringIterator result = {};
    result.begin = &DD_at(iter);
    while (DD_at(iter) == stopChar) {
        ++result.size;
        DD_advance(iter);
        if (DD_isAtTheEnd(*iter))
            return result;
    }
    if (include && result.size) {
        ++result.size;
        DD_advance(iter);
    }
    return result;
}

DD_StringIterator DD_advanceWhileNotChar(DD_StringIterator* iter, char stopChar, bool include=false) {
    DD_StringIterator result = {};
    result.begin = &DD_at(iter);
    while (DD_at(iter) != stopChar) {
        DD_advance(iter);
        ++result.size;
        if (DD_isAtTheEnd(*iter))
            return result;
    }
    if (include && result.size) {
        ++result.size;
        DD_advance(iter);
    }
    return result;
}

DD_StringIterator DD_advanceWhileNotString(DD_StringIterator* iter, const char* delim, bool include=false) {
    DD_StringIterator result = {};
    result.begin = &DD_at(iter);
    int delimSize = strlen(delim);
    while (DD_startsWith(iter, delim, delimSize)) {
        DD_advance(iter);
        ++result.size;
        if (DD_isAtTheEnd(*iter))
            return result;
    }
    if (include && result.size) {
        result.size += delimSize;
        DD_advance(iter, delimSize);
    }
    return result;
}

DD_StringIterator DD_advanceWhileAlpha(DD_StringIterator* iter, bool include=false) {
    return DD_advanceWhileCondition(iter, DD_isAlpha, include);
}

DD_StringIterator DD_advanceWhileAlphaNum(DD_StringIterator* iter, bool include=false) {
    return DD_advanceWhileCondition(iter, DD_isAlphaNum, include);
}

DD_StringIterator DD_advanceWhileNumber(DD_StringIterator* iter, bool include=false) {
    return DD_advanceWhileCondition(iter, DD_isNumber, include);
}

DD_StringIterator DD_advanceWhileNotNumber(DD_StringIterator* iter, bool include=false) {
    return DD_advanceWhileNotCondition(iter, DD_isNumber, include);
}

DD_StringIterator DD_advanceWhileWhiteSpace(DD_StringIterator* iter, bool include=false) {
    return DD_advanceWhileCondition(iter, DD_isWhiteSpace, include);
}

DD_StringIterator DD_advanceWhileNotWhiteSpace(DD_StringIterator* iter, bool include=false) {
    return DD_advanceWhileNotCondition(iter, DD_isWhiteSpace, include);
}

DD_StringIterator DD_advanceWhileNotNewLine(DD_StringIterator* iter, bool include=false) {
    return DD_advanceWhileNotChar(iter, '\n', include);
}

DD_StringIterator DD_advanceWhileNotNull(DD_StringIterator* iter, bool include=false) {
    return DD_advanceWhileNotChar(iter, '\0', include);
}

bool DD_equals(DD_StringIterator iter, const char* cstr) {
    return (iter.size - iter.at == (int)strlen(cstr))
        && (memcmp(iter.begin + iter.at, cstr, iter.size - iter.at) == 0);
}

int DD_toInt(DD_StringIterator iter) {
    char buf[32];
    memcpy(buf, iter.begin, iter.size);
    buf[iter.size] = 0;
    return strtol(buf, 0, 10);
}

char* DD_toString(DD_StringIterator iter) {
    char* string = (char*) malloc(iter.size+1);
    memcpy(string, iter.begin, iter.size);
    string[iter.size] = 0;
    return string;
}

bool DD_consume(DD_StringIterator* strit, char c) {
    if (DD_at(strit) == c) {
        DD_advance(strit);
        return true;
    }
    return false;
}

bool DD_require(DD_StringIterator* strit, char c) {
    if (DD_at(strit) != c) {
        return false;
    }
    return true;
}

bool DD_requireAndConsume(DD_StringIterator* strit, char c) {
    if (!DD_consume(strit, c)) {
        return false;
    }
    return true;
}

void DD_error(DD_StringIterator* strit, char* out, bool colored, const char* formatString, va_list arglist) {
    const char* ANSIColorReset = "";
    const char* ANSIColorRed   = "";
    const char* ANSIColorBlue  = "";
    
#ifndef DD_STRING_ITERATOR_DISABLE_COLORED_OUTPUT
    if (colored) {
        ANSIColorReset = "\x1b[0m";
        ANSIColorRed   = "\x1b[31m";
        ANSIColorBlue  = "\x1b[34m";
    }
#endif
    
    int o = 0;
    
    o += sprintf(out+o, "%s", ANSIColorRed);
    o += sprintf(out+o, "Error at line %d: ", strit->line);
    o += vsprintf(out+o, formatString, arglist);
    o += sprintf(out+o, "%s\n", ANSIColorReset);
    
    DD_StringIterator iter = *strit;
    
    int contextLines = 1;
    
    int linesBack = 0;
    while (linesBack <= contextLines) {
        if (iter.at > 0) {
            if (DD_peek(&iter, -1) == '\n') {
                ++linesBack;
            }
            DD_goback(&iter, -1);
        } else {
            break;
        }
    }
    
    if (DD_at(&iter) == '\n')
        DD_advance(&iter);
    
    DD_StringIterator iter2 = *strit;
    iter2 = *strit;
    DD_advanceWhileNotNewLine(&iter2);
    
    int lineNumber = strit->line - contextLines;
    while (lineNumber < strit->line) {
        DD_StringIterator line = DD_advanceWhileNotNewLine(&iter);
        o += sprintf(out+o, "%s%3d | %s%.*s\n", ANSIColorBlue, lineNumber, ANSIColorReset, line.size, line.begin);
        DD_advance(&iter);
        ++lineNumber;
    }
    
    o += sprintf(out+o, "%s%3d | %s", ANSIColorBlue, lineNumber, ANSIColorReset);
    o += sprintf(out+o, "%.*s", strit->at - iter.at, &DD_at(&iter));
    DD_advance(&iter, strit->at - iter.at);
    
    o += sprintf(out+o, "%s%c%s", ANSIColorRed, DD_at(&iter), ANSIColorReset);
    DD_advance(&iter);
    
    DD_StringIterator line = DD_advanceWhileNotNewLine(&iter);
    o += sprintf(out+o, "%.*s\n", line.size, line.begin);
}

void DD_error(char* out, DD_StringIterator* strit, const char* formatString, ...) {
    va_list arglist;
    va_start(arglist, formatString);
    DD_error(strit, out, false, formatString, arglist);
    va_end(arglist);
}

void DD_error(DD_StringIterator* strit, bool colored, const char* formatString, ...) {
    char buffer[1024];
    va_list arglist;
    va_start(arglist, formatString);
    DD_error(strit, buffer, colored, formatString, arglist);
    va_end(arglist);
    printf("%s", buffer);
}

void DD_error(DD_StringIterator* strit, const char* formatString, ...) {
    char buffer[1024];
    va_list arglist;
    va_start(arglist, formatString);
    DD_error(strit, buffer, true, formatString, arglist);
    va_end(arglist);
    printf("%s", buffer);
}
