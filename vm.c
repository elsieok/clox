#include <stdio.h>
#include "common.h"
#include "compiler.h"
#include "debug.h"
#include "vm.h"

VM vm;

static void resetStack(void) {
    vm.stackTop = vm.stack;
}

void initVM(void) {
    resetStack();
}

void freeVM(void) {
}

void push(Value value) {
    *vm.stackTop = value;
    vm.stackTop++;
}

Value pop(void) {
    vm.stackTop--;
    return *vm.stackTop;
}

static inline uint8_t read_byte(void) {
    return *vm.ip++;
}

static inline Value read_constant(void) {
    return vm.chunk->constants.values[read_byte()];
}

static inline Value read_constant_long(void) {
    uint32_t index = 
        ((uint32_t)read_byte() << 16) |
        ((uint32_t)read_byte() << 8)  |
        (uint32_t)read_byte();

    return vm.chunk->constants.values[index];
}

static InterpretResult run(void) {
    #define BINARY_OP(op) \
        do { \
            double b = pop(); \
            double a = pop(); \
            push(a op b); \
    } while (false)

    for (;;) {
        #ifdef DEBUG_TRACE_EXECUTION
            printf("          ");
            for (Value* slot = vm.stack; slot < vm.stackTop; slot++) {
                printf("[ ");
                printValue(*slot);
                printf(" ]");
            }
            printf("\n");
            dissassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
        #endif
        uint8_t instruction  = read_byte();
        switch (instruction) {
            case OP_CONSTANT: {
                Value constant = read_constant();
                push(constant);
                break;
            }case OP_CONSTANT_LONG: {
                Value constant = read_constant_long();
                push(constant);
                break;
            }
            case OP_ADD:      BINARY_OP(+); break;
            case OP_SUBTRACT: BINARY_OP(-); break;
            case OP_MULTIPLY: BINARY_OP(*); break;
            case OP_DIVIDE:   BINARY_OP(/); break;
            case OP_NEGATE:   push(-pop()); break;
            case OP_RETURN: {
                printValue(pop());
                printf("\n");
                return INTERPRET_OK;
            }
            
        }
    }
    #undef BINARY_OP

}

InterpretResult interpret(const char* source) {
    compile(source);
    return INTERPRET_OK;
}