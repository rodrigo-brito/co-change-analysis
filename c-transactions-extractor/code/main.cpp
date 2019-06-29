#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define assert(x)

#define STB_LEAKCHECK_IMPLEMENTATION
#include "stb_leakcheck.h"
#include "dd_array.cpp"

#define min(a, b) (a < b ? a:b)
#define max(a, b) (a > b ? a:b)

#include "Memory.h"
#include "Array.cpp"
#include "EntireFile.cpp"
#include "FileReader.cpp"
#include "FileWriter.cpp"
#include "String.cpp"
#include "string_utils.cpp"

#define ANSI_COLOR_YELLOW(text) "\x1b[33m" text "\x1b[0m"
#define LOOP_SIZE 50

enum TokenType {
    TokenTypeNone,
    TokenTypeLeftBrace,
    TokenTypeRightBrace,
    TokenTypeLeftParentesis,
    TokenTypeRightParentesis,
    TokenTypeLineComment,
    TokenTypeBlockComment,
    TokenTypeEOF,
    TokenTypeUnknown,
    TokenTypeCount
};

struct Token {
    TokenType type;
    char* string;
    int charIndex;
    int line;
};

struct ChangeInfo {
    int oldFileLine;
    int oldFileCount;
    int newFileLine;
    int newFileCount;
};

struct DiffChunk {
    char oldFile[256];
    char newFile[256];
    ChangeInfo* changes;
    //char** functionsChanged;
};

struct CommitInfo {
    char hash[256];
    DiffChunk* chunks;
    char** functionsChanged;
};

struct FunctionInFile {
    char name[256];
    int firstLine;
    int lastLine;
    bool changed;
};

Token readNextToken(FileReader& reader) {
    //while (isWhiteSpace(reader.content[reader.at]) || startWithReader(reader, "//") || startWithReader(reader, "#define") || startWithReader(reader, "/*")) {
    while (isWhiteSpace(reader.content[reader.at])
        || startWithReader(reader, "/*")
        || startWithReader(reader, "//")
        || startWithReader(reader, "#define")
        || (startWithReader(reader, "\"") && *(reader.content+reader.at-1) != '\\')) {
        while (isWhiteSpace(reader.content[reader.at])) {
            skipWhiteSpace(&reader);
        }

        while (startWithReader(reader, "//")) {
            //printf("line comment\n");
            readUntilCharIsReached(&reader, '\n');
        }

        while (startWithReader(reader, "\"") && *(reader.content+reader.at-1) != '\\') {
            //printf("string literal\n");
            advance(&reader);
            FileReader r = readUntilCharIsReached(&reader, '\"');
            while (*(reader.content+reader.at-1) == '\\') {
                advance(&reader);
                readUntilCharIsReached(&reader, '\"');
            }
            advance(&reader);
        }

        while (startWithReader(reader, "#define")) {
            //printf("define start %d\n", reader.line);
            FileReader defineLine = readUntilCharIsReached(&reader, '\n');
            advance(&reader);
            char* c = at(&reader);
            while (isWhiteSpace(*c)) --c;

            while (*c == '\\') {
                defineLine = readUntilCharIsReached(&reader, '\n');
                advance(&reader);
                c = at(&reader);
                while (isWhiteSpace(*c)) --c;
            }
            //printf("define end %d\n", reader.line);
        }

        while (startWithReader(reader, "/*")) {
            //printf("block comment %d\n", reader.line);
            advance(&reader, 2);
            FileReader r = readUntilCharIsReached(&reader, '*');
            //printf("%.*s\n", r.size, at(&r));
            while (peek(&reader) != '/') {
                advance(&reader);
                readUntilCharIsReached(&reader, '*');
                if (eof(reader)) break;
            }
            if (eof(reader)) break;
            //printf("end %d\n", reader.line);
            advance(&reader, 2);
        }
        if (eof(reader)) break;
    }

    if (reader.at >= reader.size || reader.content[reader.at] == 0)
        return {TokenTypeEOF, 0, reader.at, reader.line};

    Token token = {};
    token.line = reader.line;

    if (reader.content[reader.at] == '{') {
        token.type = TokenTypeLeftBrace;
    } else if (reader.content[reader.at] == '}') {
        token.type = TokenTypeRightBrace;
    } else if (reader.content[reader.at] == '(') {
        token.type = TokenTypeLeftParentesis;
    } else if (reader.content[reader.at] == ')') {
        token.type = TokenTypeRightParentesis;
    } else {
        advance(&reader);
        return {};
        //printf("Unexpected character %c\n", reader.content[reader.at]);
        //assert(0);
    }

    token.charIndex = reader.at;

    advance(&reader);

    return token;
}

int loadProcessOutput(char* buffer, const char* formatString, ...) {
    char path[1024];
    va_list argList;
    va_start(argList, formatString);
    vsprintf(path, formatString, argList);
    va_end(argList);

    FILE* f = popen(path, "r");

    if (f == nullptr) {
        printf("Failed to load process %s\n", path);
        return false;
    }

    char* at = buffer;
    char lineBuffer[2048];
    while (fgets(lineBuffer, 2048, f)) {
        int len = strlen(lineBuffer);
        memcpy(at, lineBuffer, len);
        at += len;
    }

    pclose(f);

    return at-buffer;
}

FunctionInFile* getFunctionsInFile(EntireFile entireFile) {
    FunctionInFile* functions = arralloc(FunctionInFile, 50);

    FileReader reader = {entireFile.content, entireFile.size, 0, 1};

    Token token1 = readNextToken(reader);
    while (!eof(reader)) {
        Token token2 = readNextToken(reader);

        if (token1.type == TokenTypeRightParentesis && token2.type == TokenTypeLeftBrace) {
            FunctionInFile function = {};

            char* c = &entireFile.content[token1.charIndex];

            int scope = 0;
            while (1) {
                --c;
                if (*c == ')') ++scope;
                else if (*c == '(' && scope > 0) --scope;
                else if (*c == '(' && scope == 0) break;
            }

            char* nameEnd = --c;
            while (isAlphaNum(*c) || *c == '_') --c;
            char* nameBegin = ++c;

            sprintf(function.name, "%.*s", (int) (nameEnd-nameBegin+1), nameBegin);

            function.firstLine = reader.line;

            int scopeLevel = 0;
            while (!eof(reader)) {
                Token tk = readNextToken(reader);
                if (tk.type == TokenTypeRightBrace) {
                    if (scopeLevel == 0) {
                        break;
                    } else {
                        --scopeLevel;
                    }
                } else if (tk.type == TokenTypeLeftBrace) {
                    ++scopeLevel;
                }
            }

            function.lastLine = reader.line;
            arradd(functions, function);
            //printf("%s{%d, %d}\n", function.name, function.firstLine, function.lastLine);
        }

        token1 = token2;
    }

    return functions;
}

void writeCommit(FileWriter* writer, CommitInfo commit) {
    writeTextInFile(writer, "  [");
    writeTextInFile(writer, "\"%s\"", commit.functionsChanged[0]);
    for (int f = 1; f < arrcount(commit.functionsChanged); ++f) {
        writeTextInFile(writer, ", \"%s\"", commit.functionsChanged[f]);
    }
    writeTextInFile(writer, "]");
}

int main(int argc, char* argv[]) {
#if 1
    if (argc < 4) {
        printf("Usage: %s <git_repo_path> <output_path> <num_commits>", argv[0]);
        return 0;
    }

    char repositoryPath[512];
    sprintf(repositoryPath, "%s/.git", argv[1]);

    char outputPath[512];
    sprintf(outputPath, "%s", argv[2]);

    int maxNumberOfCommitsToAnalyse = atoi(argv[3]);

    FILE* outputFile = fopen(outputPath, "w");
    fwrite("[\n", 1, 2, outputFile);

    char totalCommitCountString[12];
    loadProcessOutput(totalCommitCountString, "git --git-dir=\"%s\" rev-list --count master", repositoryPath);
    int totalCommitCount = atoi(totalCommitCountString);

    FileReader commitHashesOutput = {(char*) malloc(MEGA_BYTES(40))};
    commitHashesOutput.size = loadProcessOutput(commitHashesOutput.content, "git --git-dir=\"%s\" log --oneline --pretty=tformat:\"%%H\"", repositoryPath);

    char** commitHashes = arralloc(char*, totalCommitCount);
    while (!eof(commitHashesOutput)) {
        char* commitHash = (char*) malloc(64);
        readUntilCharIsReached(commitHash, &commitHashesOutput, '\n');
        advance(&commitHashesOutput);
        arradd(commitHashes, commitHash);
    }

    free(commitHashesOutput.content);

    int numberOfCommitsToAnalyse = min(totalCommitCount, maxNumberOfCommitsToAnalyse);

    EntireFile ef = {};
    ef.content = (char*) malloc(MEGA_BYTES(500));

    CommitInfo* commits = (CommitInfo*) malloc(numberOfCommitsToAnalyse*sizeof(CommitInfo));
    int commitCount = 0;

    int upToCommit = 0;
    int sinceCommit = min(numberOfCommitsToAnalyse-1, LOOP_SIZE);


    while (sinceCommit <= numberOfCommitsToAnalyse) {
        ef.size = loadProcessOutput(
            ef.content,
            "git --git-dir=\"%s\" log ^%s^..^%s^ --diff-filter=M -p --unified=0",
            repositoryPath,
            commitHashes[sinceCommit],
            commitHashes[upToCommit]
        );

        printf(ANSI_COLOR_YELLOW("Analysing log from %.*s to %.*s (size: %.2fmb)\n"),
               6, commitHashes[sinceCommit],
               6, commitHashes[upToCommit],
               (float)ef.size/MEGA_BYTES(1)
        );

        CommitInfo* currentCommit = 0;
        DiffChunk* currentChunk = 0;
        int newCommits = 0;

        FileReader reader = {ef.content, ef.size, 0, 1};
        while (!eof(reader)) {
            FileReader line = readUntilCharIsReached(&reader, '\n');

            if (startsWith(line.content, "commit")) {
                advance(&line, sizeof("commit"));
                currentCommit = &commits[commitCount++];
                readUntilCharIsReached(currentCommit->hash, &line, '\n');
                printf("%6d: %s\n", commitCount, currentCommit->hash);
                currentCommit->chunks = arralloc(DiffChunk, 10);
                ++newCommits;
            } else if (startsWith(line.content, "---")) {
                advance(&line, sizeof("--- a"));
                DiffChunk chunk = {};
                readUntilCharIsReached(chunk.oldFile, &line, '\n');
                chunk.changes = arralloc(ChangeInfo, 10);
                //chunk.functionsChanged = 0;
                arradd(currentCommit->chunks, chunk);
                currentChunk = &arrlast(currentCommit->chunks);
            } else if (startsWith(line.content, "+++")) {
                advance(&line, sizeof("+++ b"));
                readUntilCharIsReached(currentChunk->newFile, &line, '\n');
            } else if (startsWith(line.content, "@@")) {
                advance(&line, sizeof("@@ "));

                char sourceLine[32];
                char sourceCount[32];
                char targetLine[32];
                char targetCount[32];

                FileReader sourceInfo = readUntilCharIsReached(&line, ' ');

                readUntilCharIsReached(sourceLine, &sourceInfo, ',');
                advance(&sourceInfo);
                if (!eof(sourceInfo)) {
                    readRemainder(sourceCount, &sourceInfo);
                }

                advance(&line, 2);

                FileReader targetInfo = readUntilCharIsReached(&line, ' ');

                readUntilCharIsReached(targetLine, &targetInfo, ',');
                advance(&targetInfo);
                if (!eof(targetInfo)) {
                    readRemainder(targetCount, &targetInfo);
                }

                ChangeInfo change = {};
                change.oldFileLine = atoi(sourceLine);
                change.oldFileCount = atoi(sourceCount);
                change.newFileLine = atoi(targetLine);
                change.newFileCount = atoi(targetCount);

                arradd(currentChunk->changes, change);
            }

            advance(&reader);
        }

        for (int i = commitCount - newCommits; i < commitCount; ++i) {
            //printf("%s\n", commits[i].hash);
            char** functionsChanged = arralloc(char*, 10);

            for (int j = 0; j < arrcount(commits[i].chunks); ++j) {
                DiffChunk& chunk = commits[i].chunks[j];

                if (endsWith(chunk.newFile, ".cpp")
                || endsWith(chunk.newFile, ".hpp")
                || endsWith(chunk.newFile, ".c")
                || endsWith(chunk.newFile, ".h")) {

                    EntireFile file = {};
                    file.content = (char*) malloc(MEGA_BYTES(64));
                    file.size = loadProcessOutput(file.content, "git --git-dir=%s/.git show %s:%s", argv[1], commits[i].hash, chunk.newFile);

                    FunctionInFile* functions = getFunctionsInFile(file);

                    for (int k = 0; k < arrcount(chunk.changes); ++k) {
                        ChangeInfo change = chunk.changes[k];

                        for (int f = 0; f < arrcount(functions); ++f) {
                            FunctionInFile& function = functions[f];
                            if (change.newFileLine >= function.firstLine && change.newFileLine <= function.lastLine) {
                                function.changed = true;
                            }
                        }
                    }

                    for (int f = 0; f < arrcount(functions); ++f) {
                        FunctionInFile& function = functions[f];
                        if (function.changed) {
                            char* functionName = (char*) malloc(256);
                            sprintf(functionName, "[%s] %s", chunk.newFile, function.name);
                            arradd(functionsChanged, functionName);
                        }
                    }

                    arrfree(functions);
                    free(file.content);
                }
            }

            commits[i].functionsChanged = functionsChanged;
        }

        FileWriter writer = {(char*) malloc(MEGA_BYTES(100))};

        if (newCommits) {
            for (int i = commitCount - newCommits; i < commitCount; ++i) {
                writeCommit(&writer, commits[i]);
                writeTextInFile(&writer, ",\n");
            }
        }

        appendFile(writer, outputFile);

        free(writer.buffer);

        for (int i = commitCount-newCommits; i < commitCount; ++i) {
            for (int j = 0; j < arrcount(commits[i].chunks); ++j) {
                arrfree(commits[i].chunks[j].changes);
            }
            for (int f = 0; f < arrcount(commits[i].functionsChanged); ++f) {
                free(commits[i].functionsChanged[f]);
            }
            arrfree(commits[i].functionsChanged);
            arrfree(commits[i].chunks);
        }

        upToCommit = sinceCommit;
        sinceCommit += LOOP_SIZE;
        sinceCommit = min(sinceCommit, numberOfCommitsToAnalyse);
        if (sinceCommit == upToCommit) break;
    }

    fseek(outputFile, -3, SEEK_CUR);
    fwrite("\n]", 1, 2, outputFile);
    printf("Done!\n");

    free(ef.content);

    for (int i = 0; i < arrcount(commitHashes); ++i) {
        free(commitHashes[i]);
    }

    arrfree(commitHashes);
    arrfree(commits);

    stb_leakcheck_dumpmem();

    return 0;
#elif 0

    const char* repos[] = {
    "php/php-src"
    };
    
    for (unsigned int repoIndex = 0; repoIndex < sizeof(repos); ++repoIndex) {
        system("rd /s /q \"../../temp-clone\"");
        system("mkdir \"../../temp-clone\"");
        
        printf("%s\n", repos[repoIndex]);

        char commandLine[512];
        sprintf(commandLine, "git clone https://github.com/%s.git ../../temp-clone", repos[repoIndex]);
        system(commandLine);
        
        char repositoryPath[512];
        sprintf(repositoryPath, "../../temp-clone/.git");

        char outputPath[512];
        sprintf(outputPath, "../../transactions/");
        FileReader rd = {};
        rd.content = (char*) repos[repoIndex];
        rd.size = strlen(repos[repoIndex]);
        readUntilCharIsReached(outputPath + strlen(outputPath), &rd, '/');
        advance(&rd);
        rd = readRemainder(&rd);
        sprintf(outputPath + strlen(outputPath), "_%.*s.txt", rd.size, rd.content);

        printf("Output path: %s\n", outputPath);

        int maxNumberOfCommitsToAnalyse = 10000;

        FILE* outputFile = fopen(outputPath, "w");
        fwrite("[\n", 1, 2, outputFile);
        
        char branchName[128];
        loadProcessOutput(branchName, "git --git-dir=\"%s\" branch | grep \\* | cut -d ' ' -f2", repositoryPath);
        branchName[strlen(branchName)-1] = 0;
        
        char totalCommitCountString[12];
        loadProcessOutput(totalCommitCountString, "git --git-dir=\"%s\" rev-list --count %s", repositoryPath, branchName);
        int totalCommitCount = atoi(totalCommitCountString);

        FileReader commitHashesOutput = {(char*) malloc(MEGA_BYTES(40))};
        commitHashesOutput.size = loadProcessOutput(commitHashesOutput.content, "git --git-dir=\"%s\" log --oneline --pretty=tformat:\"%%H\"", repositoryPath);

        char** commitHashes = arralloc(char*, totalCommitCount);
        while (!eof(commitHashesOutput)) {
            char* commitHash = (char*) malloc(64);
            readUntilCharIsReached(commitHash, &commitHashesOutput, '\n');
            advance(&commitHashesOutput);
            arradd(commitHashes, commitHash);
        }

        free(commitHashesOutput.content);

        int numberOfCommitsToAnalyse = min(totalCommitCount, maxNumberOfCommitsToAnalyse);

        EntireFile ef = {};
        ef.content = (char*) malloc(MEGA_BYTES(500));

        CommitInfo* commits = arralloc(CommitInfo, numberOfCommitsToAnalyse);
        int commitCount = 0;

        int upToCommit = 0;
        int sinceCommit = min(numberOfCommitsToAnalyse-1, LOOP_SIZE);


        for (int commitIndex = 0; commitIndex < numberOfCommitsToAnalyse; ++commitIndex) {
            ef.size = loadProcessOutput(
                ef.content,
                "git --git-dir=\"%s\" log %s --diff-filter=M -p --unified=0 -1",
                repositoryPath,
                commitHashes[commitIndex]
            );

            CommitInfo* currentCommit = 0;
            DiffChunk* currentChunk = 0;
            int newCommits = 0;

            FileReader reader = {ef.content, ef.size, 0, 1};
            while (!eof(reader)) {
                FileReader line = readUntilCharIsReached(&reader, '\n');

                if (startsWith(line.content, "commit")) {
                    advance(&line, sizeof("commit"));
                    currentCommit = &commits[commitCount++];
                    readUntilCharIsReached(currentCommit->hash, &line, '\n');
                    printf(ANSI_COLOR_YELLOW("%6d: ") "%s\n", commitCount, currentCommit->hash);
                    currentCommit->chunks = arralloc(DiffChunk, 10);
                    ++newCommits;
                } else if (startsWith(line.content, "---")) {
                    advance(&line, sizeof("--- a"));
                    DiffChunk chunk = {};
                    readUntilCharIsReached(chunk.oldFile, &line, '\n');
                    chunk.changes = arralloc(ChangeInfo, 10);
                    //chunk.functionsChanged = 0;
                    arradd(currentCommit->chunks, chunk);
                    currentChunk = &arrlast(currentCommit->chunks);
                } else if (startsWith(line.content, "+++")) {
                    advance(&line, sizeof("+++ b"));
                    readUntilCharIsReached(currentChunk->newFile, &line, '\n');
                } else if (startsWith(line.content, "@@")) {
                    advance(&line, sizeof("@@ "));

                    char sourceLine[32];
                    char sourceCount[32];
                    char targetLine[32];
                    char targetCount[32];

                    FileReader sourceInfo = readUntilCharIsReached(&line, ' ');

                    readUntilCharIsReached(sourceLine, &sourceInfo, ',');
                    advance(&sourceInfo);
                    if (!eof(sourceInfo)) {
                        readRemainder(sourceCount, &sourceInfo);
                    }

                    advance(&line, 2);

                    FileReader targetInfo = readUntilCharIsReached(&line, ' ');

                    readUntilCharIsReached(targetLine, &targetInfo, ',');
                    advance(&targetInfo);
                    if (!eof(targetInfo)) {
                        readRemainder(targetCount, &targetInfo);
                    }

                    ChangeInfo change = {};
                    change.oldFileLine = atoi(sourceLine);
                    change.oldFileCount = atoi(sourceCount);
                    change.newFileLine = atoi(targetLine);
                    change.newFileCount = atoi(targetCount);

                    arradd(currentChunk->changes, change);
                }

                advance(&reader);
            }

            for (int i = commitCount - newCommits; i < commitCount; ++i) {
                //printf("%s\n", commits[i].hash);
                char** functionsChanged = arralloc(char*, 10);

                for (int j = 0; j < arrcount(commits[i].chunks); ++j) {
                    DiffChunk& chunk = commits[i].chunks[j];

                    if (endsWith(chunk.newFile, ".cpp")
                    || endsWith(chunk.newFile, ".hpp")
                    || endsWith(chunk.newFile, ".c")
                    || endsWith(chunk.newFile, ".h")) {

                        EntireFile file = {};
                        file.content = (char*) malloc(MEGA_BYTES(64));
                        file.size = loadProcessOutput(file.content, "git --git-dir=../../temp-clone/.git show %s:%s", commits[i].hash, chunk.newFile);

                        FunctionInFile* functions = getFunctionsInFile(file);

                        for (int k = 0; k < arrcount(chunk.changes); ++k) {
                            ChangeInfo change = chunk.changes[k];

                            for (int f = 0; f < arrcount(functions); ++f) {
                                FunctionInFile& function = functions[f];
                                if (change.newFileLine >= function.firstLine && change.newFileLine <= function.lastLine) {
                                    function.changed = true;
                                }
                            }
                        }

                        for (int f = 0; f < arrcount(functions); ++f) {
                            FunctionInFile& function = functions[f];
                            if (function.changed) {
                                char* functionName = (char*) malloc(256);
                                sprintf(functionName, "[%s] %s", chunk.newFile, function.name);
                                arradd(functionsChanged, functionName);
                            }
                        }

                        arrfree(functions);
                        free(file.content);
                    }
                }

                commits[i].functionsChanged = functionsChanged;
            }

            FileWriter writer = {(char*) malloc(MEGA_BYTES(100))};

            if (newCommits) {
                for (int i = commitCount - newCommits; i < commitCount; ++i) {
                    if (arrcount(commits[i].functionsChanged)) {
                        writeCommit(&writer, commits[i]);
                        writeTextInFile(&writer, ",\n");
                    }
                }
            }

            appendFile(writer, outputFile);

            free(writer.buffer);

            for (int i = commitCount-newCommits; i < commitCount; ++i) {
                for (int j = 0; j < arrcount(commits[i].chunks); ++j) {
                    arrfree(commits[i].chunks[j].changes);
                }
                for (int f = 0; f < arrcount(commits[i].functionsChanged); ++f) {
                    free(commits[i].functionsChanged[f]);
                }
                arrfree(commits[i].functionsChanged);
                arrfree(commits[i].chunks);
            }
        }

        fseek(outputFile, -3, SEEK_CUR);
        fwrite("\n]", 1, 2, outputFile);
        fclose(outputFile);
        printf("Done!\n");

        free(ef.content);

        for (int i = 0; i < arrcount(commitHashes); ++i) {
            free(commitHashes[i]);
        }

        arrfree(commitHashes);
        arrfree(commits);

        //stb_leakcheck_dumpmem();
    }
    return 0;
#endif
}

