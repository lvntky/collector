cmake_minimum_required(VERSION 3.26)
project(collector)

set(SOURCE_FILES ./src/main.c ./src/memalloc.c ./include/mamalloc.h)
add_executable(memalloc ${SOURCE_FILES})
