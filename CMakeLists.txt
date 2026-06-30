cmake_minimum_required(VERSION 3.10.0)

#project name
project (clox)

add_executable(clox main.c chunk.c compiler.c debug.c memory.c scanner.c value.c vm.c )