cmake_minimum_required(VERSION 3.2)

project(CLIP C)

set(INCLUDE_DIR "include")

set(PARSERS_SOURCES "src/Parsers/json.c")

set(SOURCES "src/main.c")

add_executable(CLIP ${SOURCES} ${PARSERS_SOURCES})

target_include_directories(CLIP PUBLIC ${INCLUDE_DIR})


# Testing
enable_testing()

add_executable(test_list "tests/test_list.c")
target_include_directories(test_list PUBLIC ${INCLUDE_DIR})
add_test(NAME test_list COMMAND test_list)

add_executable(test_set "tests/test_set.c")
target_include_directories(test_set PUBLIC ${INCLUDE_DIR})
add_test(NAME test_set COMMAND test_set)

add_executable(test_map "tests/test_map.c")
target_include_directories(test_map PUBLIC ${INCLUDE_DIR})
add_test(NAME test_map COMMAND test_map)

add_executable(test_stack "tests/test_stack.c")
target_include_directories(test_stack PUBLIC ${INCLUDE_DIR})
add_test(NAME test_stack COMMAND test_stack)

add_executable(test_queue "tests/test_queue.c")
target_include_directories(test_queue PUBLIC ${INCLUDE_DIR})
add_test(NAME test_queue COMMAND test_queue)

add_executable(test_json "tests/Parsers/test_json.c" ${PARSERS_SOURCES})
target_include_directories(test_json PUBLIC ${INCLUDE_DIR})
add_test(NAME test_json COMMAND test_json)
