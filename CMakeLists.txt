cmake_minimum_required(VERSION 3.2)

project(CLIP C)

set(INCLUDE_DIR "include")

set(SOURCES "src/main.c")

add_executable(CLIP ${SOURCES})

target_include_directories(CLIP PUBLIC ${INCLUDE_DIR})



