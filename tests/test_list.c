#include "CLIP/List.h"
#include <stdio.h>
#include <setjmp.h> 
#include <time.h> 


static jmp_buf test_failure_jump; // Used to jump back on failure
static int tests_run = 0;
static int tests_failed = 0;

// A better assertion macro that doesn't abort
#define CHECK(condition)                                                      \
    do {                                                                      \
        if (!(condition)) {                                                   \
            fprintf(stderr, "      FAIL: %s\n", #condition);                  \
            longjmp(test_failure_jump, 1); /* Jump back to the runner loop */ \
        }                                                                     \
    } while (0)

// Structure to hold a test case
typedef struct {
    const char* name;
    void (*func)(void);
} test_case;


CLIP_DEFINE_LIST_TYPE(int)

void test_append_and_get() {
    List(int) xs = List_init(int, 2);
    CHECK(List_append(int, &xs, 10));
    CHECK(List_append(int, &xs, 20));
    CHECK(xs.size == 2);
    CHECK(List_get(int, &xs, 0) == 10);
    CHECK(List_get(int, &xs, 1) == 20);
    List_free(int, &xs);
}

void test_insert_and_remove() {
    List(int) xs = List_init(int, 2);
    List_append(int, &xs, 1);
    List_append(int, &xs, 3);
    CHECK(List_insert(int, &xs, 1, 2));
    CHECK(xs.size == 3);
    CHECK(List_get(int, &xs, 1) == 2);

    CHECK(List_remove_at(int, &xs, 1));
    CHECK(xs.size == 2);
    CHECK(List_get(int, &xs, 1) == 3);
    List_free(int, &xs);
}

void test_pop() {
    List(int) xs = List_init(int, 2);
    List_append(int, &xs, 42);
    int out;
    CHECK(List_pop(int, &xs, &out));
    CHECK(out == 42);
    CHECK(xs.size == 0);
    List_free(int, &xs);
}


int main(void) {
    // List of all test cases to run
    test_case tests[] = {
        {"test_append_and_get", test_append_and_get},
        {"test_insert_and_remove", test_insert_and_remove},
        {"test_pop", test_pop},
    };

    int num_tests = sizeof(tests) / sizeof(tests[0]);
    printf("Running %d tests...\n", num_tests);
    printf("------------------------------------\n");

    for (int i = 0; i < num_tests; i++) {
        printf(" [TEST] %s\n", tests[i].name);
        tests_run++;

        clock_t start = clock();
        // setjmp returns 0 on its first call, and non-zero if longjmp was called
        if (setjmp(test_failure_jump) == 0) {
            tests[i].func(); // Execute the test
            clock_t end = clock();
            double duration = (double)(end - start) / CLOCKS_PER_SEC;
            printf("   -> PASS (%.4fs)\n", duration);
        } else {
            // This block is executed if a CHECK failed
            tests_failed++;
            printf("   -> FAIL\n");
        }
    }
    
    printf("------------------------------------\n");
    printf("Summary: %d run, %d passed, %d failed.\n", tests_run, tests_run - tests_failed, tests_failed);

    // Return non-zero status code if any tests failed
    return (tests_failed > 0);
}
