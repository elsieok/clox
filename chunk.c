#include <stdlib.h>

#include "chunk.h"
#include "memory.h"

void initLineArray(LineArray* array) {
    array->entries = NULL;
    array->count = 0;
    array->capacity = 0;
}

void initChunk(Chunk* chunk) {
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;

    initLineArray(&chunk->lines);

    initValueArray(&chunk->constants);
}

void freeChunk(Chunk* chunk) {
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    FREE_ARRAY(LineEntry, chunk->lines.entries, chunk->lines.capacity);
    freeValueArray(&chunk->constants);
    initChunk(chunk);
}

void writeChunk(Chunk* chunk, uint8_t byte, int line) {
    if (chunk->capacity < chunk->count + 1) {
        int oldCapacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(oldCapacity);
        chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
    }

    chunk->code[chunk->count] = byte;

    if (chunk->lines.count > 0 &&
        chunk->lines.entries[chunk->lines.count - 1].line == line) {

        // Same line as previous instruction → extend run
        chunk->lines.entries[chunk->lines.count - 1].count++;

    } else {
        // Different line → start new run
        LineEntry entry;
        entry.line = line;
        entry.count = 1;

        // Grow line array if needed
        if (chunk->lines.capacity < chunk->lines.count + 1) {
            int oldCapacity = chunk->lines.capacity;
            chunk->lines.capacity = GROW_CAPACITY(oldCapacity);
            chunk->lines.entries = GROW_ARRAY(LineEntry, chunk->lines.entries, oldCapacity, chunk->lines.capacity);
        }

        chunk->lines.entries[chunk->lines.count] = entry;
        chunk->lines.count++;
    }

    chunk->count++;
}

int getLine(Chunk* chunk, int instructionIndex) {
    int i = 0;
    for (int entry = 0; entry < chunk->lines.count; entry++) {
        i += chunk->lines.entries[entry].count;

        if (instructionIndex < i) {
            return chunk->lines.entries[entry].line;
        }
    }

    return -1;
}

void writeConstant(Chunk* chunk, Value value, int line) {
    int constantIndex = addConstant(chunk, value);

    if (constantIndex <= 0xFF) { // Fits in one byte
        writeChunk(chunk, OP_CONSTANT, line);
        writeChunk(chunk, (uint8_t)constantIndex, line);
    } else { // Needs 3 bytes
        writeChunk(chunk, OP_CONSTANT_LONG, line);
        writeChunk(chunk, (constantIndex >> 16) & 0xFF, line); // high byte
        writeChunk(chunk, (constantIndex >> 8) & 0xFF, line);  // middle byte
        writeChunk(chunk, constantIndex & 0xFF, line);         // low byte
    }
}

int addConstant(Chunk* chunk, Value value) {
    writeValueArray(&chunk->constants, value);
    return chunk->constants.count - 1;
}