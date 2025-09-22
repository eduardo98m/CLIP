/**
 * @file Test.h
 * @author Eduardo Ignacio Lopez Hernandez / eduardo98m@gmail.com
 * @brief Macro-based, single-header testing framework for C.
 *
 * Example Usage:
 * ```c
 * #include "CLIP/Test.h"
 *
 * TEST(my_first_test) {
 * int x = 5;
 * ASSERT_TRUE(x == 5);
 * ASSERT_TRUE(x > 0);
 * }
 *
 * TEST(another_test) {
 * const char* s = "hello";
 * ASSERT_STR_EQ(s, "hello");
 * ASSERT_NOT_NULL(s);
 * }
 *
 * TEST_SUITE(
 * RUN_TEST(my_first_test),
 * RUN_TEST(another_test)
 * )
 * ```
 */
#ifndef CLIP_TEST_H
#define CLIP_TEST_H

#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <time.h>

// --- Internal Globals & Structures ---

static jmp_buf _clip_test_failure_jump;

typedef struct {
    const char* name;
    void (*func)(void);
} _clip_test_case;


// --- Core Assertion Macro ---

// Internal macro that handles the actual assertion logic and failure jump.
#define _CLIP_ASSERT(condition, msg, ...)                                                               \
    do {                                                                                                \
        if (!(condition)) {                                                                             \
            fprintf(stderr, "\n      \x1b[31mFAIL:\x1b[0m [%s:%d] " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
            longjmp(_clip_test_failure_jump, 1);                                                        \
        }                                                                                               \
    } while (0)


// --- Public Assertion Macros ---

/**
 * @def ASSERT_TRUE(cond)
 * @brief Asserts that a condition is true.
 */
#define ASSERT_TRUE(cond) _CLIP_ASSERT((cond), "Expected true, but was false: `%s`", #cond)

/**
 * @def ASSERT_FALSE(cond)
 * @brief Asserts that a condition is false.
 */
#define ASSERT_FALSE(cond) _CLIP_ASSERT(!(cond), "Expected false, but was true: `%s`", #cond)

/**
 * @def ASSERT_NULL(ptr)
 * @brief Asserts that a pointer is NULL.
 */
#define ASSERT_NULL(ptr) _CLIP_ASSERT((ptr) == NULL, "Expected NULL pointer for `%s`", #ptr)

/**
 * @def ASSERT_NOT_NULL(ptr)
 * @brief Asserts that a pointer is not NULL.
 */
#define ASSERT_NOT_NULL(ptr) _CLIP_ASSERT((ptr) != NULL, "Expected non-NULL pointer for `%s`", #ptr)


/**
 * @def ASSERT_STR_EQ(a, b)
 * @brief Asserts that two strings are equal.
 */
#define ASSERT_STR_EQ(a, b)                                                              \
    do {                                                                                 \
        const char* _a = (a);                                                            \
        const char* _b = (b);                                                            \
        _CLIP_ASSERT(_a && _b && strcmp(_a, _b) == 0, "Expected strings to be equal: got %s vs %s", _a ? _a : "NULL", _b ? _b : "NULL"); \
    } while (0)


// --- Test Definition and Running Macros ---

/**
 * @def TEST(name)
 * @brief Defines a new test case function.
 *
 * Example:
 * TEST(my_feature) {
 * ASSERT_TRUE(1);
 * }
 */
#define TEST(name) static void test_##name(void)

/**
 * @def RUN_TEST(name)
 * @brief A helper macro used inside TEST_SUITE to register a test.
 */
#define RUN_TEST(name) {#name, test_##name}

/**
 * @def TEST_SUITE(...)
 * @brief Generates the main function to run all specified tests.
 *
 * This macro takes a comma-separated list of RUN_TEST(name) calls.
 *
 * Example:
 * TEST_SUITE(
 * RUN_TEST(test_one),
 * RUN_TEST(test_two)
 * )
 */
#define TEST_SUITE(...)                                                                        \
    int main(void) {                                                                           \
        _clip_test_case tests[] = {__VA_ARGS__};                                               \
        int num_tests = sizeof(tests) / sizeof(tests[0]);                                      \
        int tests_run = 0;                                                                     \
        int tests_failed = 0;                                                                  \
                                                                                               \
        printf("🚀 Running %d tests from suite...\n", num_tests);                              \
        printf("------------------------------------\n");                                      \
                                                                                               \
        for (int i = 0; i < num_tests; i++) {                                                  \
            printf(" [TEST] %s\n", tests[i].name);                                             \
            tests_run++;                                                                       \
            clock_t start = clock();                                                           \
                                                                                               \
            if (setjmp(_clip_test_failure_jump) == 0) {                                        \
                tests[i].func();                                                               \
                clock_t end = clock();                                                         \
                double duration = (double)(end - start) / CLOCKS_PER_SEC;                      \
                printf("   -> \x1b[32mPASS\x1b[0m (%.4fs)\n", duration);                         \
            } else {                                                                           \
                tests_failed++;                                                                \
                printf("   -> \x1b[31mFAIL\x1b[0m\n");                                           \
            }                                                                                  \
        }                                                                                      \
                                                                                               \
        printf("------------------------------------\n");                                      \
        if (tests_failed == 0) {                                                               \
            printf("✅ \x1b[32mAll tests passed!\x1b[0m (%d/%d)\n", tests_run, tests_run);       \
        } else {                                                                               \
            printf("❌ \x1b[31m%d of %d tests failed.\x1b[0m\n", tests_failed, tests_run);      \
        }                                                                                      \
                                                                                               \
        return (tests_failed > 0);                                                             \
    }

#endif // CLIP_TEST_H