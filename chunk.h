#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"
#include "value.h"

typedef struct {
    int line;
    int count;
} LineEntry;

typedef struct {
    LineEntry* entries;
    int count;
    int capacity;
} LineArray;

typedef enum {
    OP_CONSTANT,
    OP_CONSTANT_LONG,
    OP_RETURN,
} OpCode;

typedef struct {
    int count;
    int capacity;
    uint8_t* code;
    LineArray lines;
    ValueArray constants;
} Chunk;

void initChunk(Chunk* chunk);
void freeChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t byte, int line);
int getLine(Chunk* chunk, int instructionIndex);
void writeConstant(Chunk* chunk, Value value, int line);
int addConstant(Chunk* chunk, Value value);

#endif