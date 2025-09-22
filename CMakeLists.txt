cmake_minimum_required(VERSION 3.2)

project(CLIP C)

set(INCLUDE_DIR "include")

set(SOURCES "src/main.c")

add_executable(CLIP ${SOURCES})

target_include_directories(CLIP PUBLIC ${INCLUDE_DIR})


# Testing
enable_testing()

add_executable(test_list "tests/test_list.c")
target_include_directories(test_list PUBLIC ${INCLUDE_DIR})

add_test(NAME test_list COMMAND test_list)



