#include "CLIP/Test.h"
#include "CLIP/Set.h"

// Comparator function for integers
int compare_ints(const int *a, const int *b)
{
    return (*a > *b) - (*a < *b);
}

// Element to string function for testing print functionality
void int_to_str(int value, char *buffer, size_t size)
{
    snprintf(buffer, size, "%d", value);
}

CLIP_DEFINE_SET_TYPE(int, compare_ints)
CLIP_REGISTER_SET_PRINT(int, int_to_str)

// --- Test Cases ---
TEST(init)
{
    Set(int) xs = Set_init(int);

    ASSERT_TRUE(Set_size(int, &xs) == 0);
    ASSERT_TRUE(Set_empty(int, &xs));
    ASSERT_TRUE(xs.root == NULL);

    Set_free(int, &xs);
}

TEST(insert_and_contains)
{
    Set(int) xs = Set_init(int);
    
    // Insert elements
    ASSERT_TRUE(Set_insert(int, &xs, 42));
    ASSERT_TRUE(Set_insert(int, &xs, 17));
    ASSERT_TRUE(Set_insert(int, &xs, 99));
    
    // Check size
    ASSERT_TRUE(Set_size(int, &xs) == 3);
    ASSERT_FALSE(Set_empty(int, &xs));
    
    // Check contains
    ASSERT_TRUE(Set_contains(int, &xs, 42));
    ASSERT_TRUE(Set_contains(int, &xs, 17));
    ASSERT_TRUE(Set_contains(int, &xs, 99));
    ASSERT_FALSE(Set_contains(int, &xs, 100));
    
    Set_free(int, &xs);
}

TEST(insert_duplicate)
{
    Set(int) xs = Set_init(int);
    
    // First insertion should succeed
    ASSERT_TRUE(Set_insert(int, &xs, 42));
    ASSERT_TRUE(Set_size(int, &xs) == 1);
    
    // Second insertion of same value should fail
    ASSERT_FALSE(Set_insert(int, &xs, 42));
    ASSERT_TRUE(Set_size(int, &xs) == 1);
    
    Set_free(int, &xs);
}

TEST(remove_existing)
{
    Set(int) xs = Set_init(int);
    
    // Insert some elements
    Set_insert(int, &xs, 42);
    Set_insert(int, &xs, 17);
    Set_insert(int, &xs, 99);
    Set_insert(int, &xs, 25);
    Set_insert(int, &xs, 5);
    
    ASSERT_TRUE(Set_size(int, &xs) == 5);
    
    // Remove existing element
    ASSERT_TRUE(Set_remove(int, &xs, 42));
    ASSERT_TRUE(Set_size(int, &xs) == 4);
    ASSERT_FALSE(Set_contains(int, &xs, 42));
    
    // Other elements should still be there
    ASSERT_TRUE(Set_contains(int, &xs, 17));
    ASSERT_TRUE(Set_contains(int, &xs, 99));
    ASSERT_TRUE(Set_contains(int, &xs, 25));
    ASSERT_TRUE(Set_contains(int, &xs, 5));
    
    Set_free(int, &xs);
}

TEST(remove_nonexistent)
{
    Set(int) xs = Set_init(int);
    
    Set_insert(int, &xs, 42);
    Set_insert(int, &xs, 17);
    
    // Try to remove non-existent element
    ASSERT_FALSE(Set_remove(int, &xs, 99));
    ASSERT_TRUE(Set_size(int, &xs) == 2);
    
    Set_free(int, &xs);
}

TEST(remove_from_empty)
{
    Set(int) xs = Set_init(int);
    
    // Try to remove from empty set
    ASSERT_FALSE(Set_remove(int, &xs, 42));
    ASSERT_TRUE(Set_size(int, &xs) == 0);
    
    Set_free(int, &xs);
}

TEST(remove_root_node_cases)
{
    Set(int) xs = Set_init(int);
    
    // Test removing root with no children
    Set_insert(int, &xs, 42);
    ASSERT_TRUE(Set_remove(int, &xs, 42));
    ASSERT_TRUE(Set_size(int, &xs) == 0);
    ASSERT_TRUE(Set_empty(int, &xs));
    
    // Test removing root with one child
    Set_insert(int, &xs, 50);
    Set_insert(int, &xs, 25);
    ASSERT_TRUE(Set_remove(int, &xs, 50));
    ASSERT_TRUE(Set_size(int, &xs) == 1);
    ASSERT_TRUE(Set_contains(int, &xs, 25));
    
    Set_free(int, &xs);
    
    // Test removing root with two children
    Set(int) ys = Set_init(int);
    Set_insert(int, &ys, 50);
    Set_insert(int, &ys, 25);
    Set_insert(int, &ys, 75);
    Set_insert(int, &ys, 60);
    Set_insert(int, &ys, 80);
    
    ASSERT_TRUE(Set_remove(int, &ys, 50));
    ASSERT_TRUE(Set_size(int, &ys) == 4);
    ASSERT_FALSE(Set_contains(int, &ys, 50));
    ASSERT_TRUE(Set_contains(int, &ys, 25));
    ASSERT_TRUE(Set_contains(int, &ys, 75));
    ASSERT_TRUE(Set_contains(int, &ys, 60));
    ASSERT_TRUE(Set_contains(int, &ys, 80));
    
    Set_free(int, &ys);
}

TEST(clear)
{
    Set(int) xs = Set_init(int);
    
    // Add some elements
    Set_insert(int, &xs, 1);
    Set_insert(int, &xs, 2);
    Set_insert(int, &xs, 3);
    
    ASSERT_TRUE(Set_size(int, &xs) == 3);
    ASSERT_FALSE(Set_empty(int, &xs));
    
    // Clear the set
    Set_clear(int, &xs);
    
    ASSERT_TRUE(Set_size(int, &xs) == 0);
    ASSERT_TRUE(Set_empty(int, &xs));
    ASSERT_TRUE(xs.root == NULL);
    
    // Should be able to insert after clearing
    ASSERT_TRUE(Set_insert(int, &xs, 42));
    ASSERT_TRUE(Set_size(int, &xs) == 1);
    
    Set_free(int, &xs);
}

TEST(ordered_insertion)
{
    Set(int) xs = Set_init(int);
    
    // Insert in random order
    int values[] = {50, 25, 75, 10, 30, 60, 80};
    int num_values = sizeof(values) / sizeof(values[0]);
    
    for (int i = 0; i < num_values; i++) {
        Set_insert(int, &xs, values[i]);
    }
    
    ASSERT_TRUE(Set_size(int, &xs) == num_values);
    
    // All values should be present
    for (int i = 0; i < num_values; i++) {
        ASSERT_TRUE(Set_contains(int, &xs, values[i]));
    }
    
    Set_free(int, &xs);
}

TEST(to_str_custom)
{
    Set(int) xs = Set_init(int);
    
    Set_insert(int, &xs, 3);
    Set_insert(int, &xs, 1);
    Set_insert(int, &xs, 2);
    
    char *str = Set_to_str_custom(int, &xs, int_to_str);
    ASSERT_NOT_NULL(str);
    
    // Should contain all elements in sorted order (in-order traversal)
    // Expected: "{1, 2, 3}"
    ASSERT_TRUE(strstr(str, "1") != NULL);
    ASSERT_TRUE(strstr(str, "2") != NULL);
    ASSERT_TRUE(strstr(str, "3") != NULL);
    ASSERT_TRUE(str[0] == '{');
    ASSERT_TRUE(str[strlen(str) - 1] == '}');
    
    free(str);
    Set_free(int, &xs);
}

TEST(to_str_registered)
{
    Set(int) xs = Set_init(int);
    
    Set_insert(int, &xs, 42);
    Set_insert(int, &xs, 17);
    
    char *str = Set_to_str(int, &xs);
    ASSERT_NOT_NULL(str);
    
    ASSERT_TRUE(strstr(str, "17") != NULL);
    ASSERT_TRUE(strstr(str, "42") != NULL);
    ASSERT_TRUE(str[0] == '{');
    ASSERT_TRUE(str[strlen(str) - 1] == '}');
    
    free(str);
    Set_free(int, &xs);
}

TEST(empty_set_to_str)
{
    Set(int) xs = Set_init(int);
    
    char *str = Set_to_str_custom(int, &xs, int_to_str);
    ASSERT_NOT_NULL(str);
    
    // Should be "{}"
    ASSERT_TRUE(strcmp(str, "{}") == 0);
    
    free(str);
    Set_free(int, &xs);
}

TEST(large_set_operations)
{
    Set(int) xs = Set_init(int);
    
    // Insert many elements
    const int count = 100;
    for (int i = 0; i < count; i++) {
        ASSERT_TRUE(Set_insert(int, &xs, i));
    }
    
    ASSERT_TRUE(Set_size(int, &xs) == count);
    
    // Check all elements are present
    for (int i = 0; i < count; i++) {
        ASSERT_TRUE(Set_contains(int, &xs, i));
    }
    
    // Remove half the elements
    for (int i = 0; i < count; i += 2) {
        ASSERT_TRUE(Set_remove(int, &xs, i));
    }
    
    ASSERT_TRUE(Set_size(int, &xs) == count / 2);
    
    // Check remaining elements
    for (int i = 0; i < count; i++) {
        if (i % 2 == 0) {
            ASSERT_FALSE(Set_contains(int, &xs, i));
        } else {
            ASSERT_TRUE(Set_contains(int, &xs, i));
        }
    }
    
    Set_free(int, &xs);
}

TEST_SUITE(
    RUN_TEST(init),
    RUN_TEST(insert_and_contains),
    RUN_TEST(insert_duplicate),
    RUN_TEST(remove_existing),
    RUN_TEST(remove_nonexistent),
    RUN_TEST(remove_from_empty),
    RUN_TEST(remove_root_node_cases),
    RUN_TEST(clear),
    RUN_TEST(ordered_insertion),
    RUN_TEST(to_str_custom),
    RUN_TEST(to_str_registered),
    RUN_TEST(empty_set_to_str),
    RUN_TEST(large_set_operations)
)