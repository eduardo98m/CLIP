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

// Global counter to track how many times the destructor is called
static int destructor_call_count = 0;
typedef struct
{
    int *value;
} AllocatedInt;
int compare_allocated_int(const AllocatedInt *a, const AllocatedInt *b)
{
    return (*a->value > *b->value) - (*a->value < *b->value);
}
void free_allocated_int(AllocatedInt *v_ptr)
{
    if (v_ptr && v_ptr->value)
    {
        free(v_ptr->value);
        v_ptr->value = NULL;
        destructor_call_count++;
    }
}

AllocatedInt new_allocated_int(int val)
{
    int *p = malloc(sizeof(int));
    *p = val;
    return (AllocatedInt){.value = p};
}
CLIP_DEFINE_SET_TYPE_WITH_FREE(AllocatedInt, compare_allocated_int, free_allocated_int)

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

    for (int i = 0; i < num_values; i++)
    {
        Set_insert(int, &xs, values[i]);
    }

    ASSERT_TRUE(Set_size(int, &xs) == num_values);

    // All values should be present
    for (int i = 0; i < num_values; i++)
    {
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
    for (int i = 0; i < count; i++)
    {
        ASSERT_TRUE(Set_insert(int, &xs, i));
    }

    ASSERT_TRUE(Set_size(int, &xs) == count);

    // Check all elements are present
    for (int i = 0; i < count; i++)
    {
        ASSERT_TRUE(Set_contains(int, &xs, i));
    }

    // Remove half the elements
    for (int i = 0; i < count; i += 2)
    {
        ASSERT_TRUE(Set_remove(int, &xs, i));
    }

    ASSERT_TRUE(Set_size(int, &xs) == count / 2);

    // Check remaining elements
    for (int i = 0; i < count; i++)
    {
        if (i % 2 == 0)
        {
            ASSERT_FALSE(Set_contains(int, &xs, i));
        }
        else
        {
            ASSERT_TRUE(Set_contains(int, &xs, i));
        }
    }

    Set_free(int, &xs);
}

int validate_rb(SetNode_int *node, int black_count, int *expected_black_height)
{
    if (!node)
    {
        if (*expected_black_height == -1)
            *expected_black_height = black_count;
        return black_count == *expected_black_height;
    }

    if (node->color == SET_BLACK_int)
        black_count++;

    // No red parent with red child
    if (node->color == SET_RED_int)
    {
        if ((node->left && node->left->color == SET_RED_int) ||
            (node->right && node->right->color == SET_RED_int))
        {
            return 0;
        }
    }

    return validate_rb(node->left, black_count, expected_black_height) &&
           validate_rb(node->right, black_count, expected_black_height);
}

TEST(red_black_invariants)
{
    Set(int) xs = Set_init(int);

    // Insert many random values
    for (int i = 0; i < 200; i++)
    {
        Set_insert(int, &xs, rand() % 1000);
    }

    // Remove some random values
    for (int i = 0; i < 100; i++)
    {
        Set_remove(int, &xs, rand() % 1000);
    }

    // Validate RB properties
    ASSERT_TRUE(xs.root == NULL || xs.root->color == SET_BLACK_int);
    int expected_black_height = -1;
    ASSERT_TRUE(validate_rb(xs.root, 0, &expected_black_height));

    Set_free(int, &xs);
}

TEST(sequential_insert_remove)
{
    Set(int) xs = Set_init(int);

    // Insert sequential values
    for (int i = 1; i <= 50; i++)
    {
        Set_insert(int, &xs, i);
    }
    ASSERT_TRUE(Set_size(int, &xs) == 50);

    // Remove in same order
    for (int i = 1; i <= 50; i++)
    {
        ASSERT_TRUE(Set_remove(int, &xs, i));
    }
    ASSERT_TRUE(Set_empty(int, &xs));

    Set_free(int, &xs);
}

TEST(reverse_order_removal)
{
    Set(int) xs = Set_init(int);

    for (int i = 1; i <= 50; i++)
    {
        Set_insert(int, &xs, i);
    }

    for (int i = 50; i >= 1; i--)
    {
        ASSERT_TRUE(Set_remove(int, &xs, i));
    }
    ASSERT_TRUE(Set_empty(int, &xs));

    Set_free(int, &xs);
}

TEST(randomized_stress)
{
    Set(int) xs = Set_init(int);
    const int ops = 1000;

    for (int i = 0; i < ops; i++)
    {
        int val = rand() % 200;
        if (rand() % 2)
        {
            Set_insert(int, &xs, val);
            ASSERT_TRUE(Set_contains(int, &xs, val));
        }
        else
        {
            Set_remove(int, &xs, val);
        }

        // Check invariants occasionally
        if (i % 100 == 0)
        {
            ASSERT_TRUE(xs.root == NULL || xs.root->color == SET_BLACK_int);
            int expected_black_height = -1;
            ASSERT_TRUE(validate_rb(xs.root, 0, &expected_black_height));
        }
    }

    Set_free(int, &xs);
}

TEST(join_empty_sets)
{
    Set(int) dest = Set_init(int);
    Set(int) src = Set_init(int);

    int added = Set_join(int, &dest, &src);
    ASSERT_TRUE(added == 0);
    ASSERT_TRUE(Set_size(int, &dest) == 0);
    ASSERT_TRUE(Set_size(int, &src) == 0);

    Set_free(int, &dest);
    Set_free(int, &src);
}

TEST(join_with_empty_source)
{
    Set(int) dest = Set_init(int);
    Set(int) src = Set_init(int);

    Set_insert(int, &dest, 1);
    Set_insert(int, &dest, 2);
    Set_insert(int, &dest, 3);

    int added = Set_join(int, &dest, &src);
    ASSERT_TRUE(added == 0);
    ASSERT_TRUE(Set_size(int, &dest) == 3);
    ASSERT_TRUE(Set_size(int, &src) == 0);
    ASSERT_TRUE(Set_contains(int, &dest, 1));
    ASSERT_TRUE(Set_contains(int, &dest, 2));
    ASSERT_TRUE(Set_contains(int, &dest, 3));

    Set_free(int, &dest);
    Set_free(int, &src);
}

TEST(join_with_empty_destination)
{
    Set(int) dest = Set_init(int);
    Set(int) src = Set_init(int);

    Set_insert(int, &src, 4);
    Set_insert(int, &src, 5);
    Set_insert(int, &src, 6);

    int added = Set_join(int, &dest, &src);
    ASSERT_TRUE(added == 3);
    ASSERT_TRUE(Set_size(int, &dest) == 3);
    ASSERT_TRUE(Set_size(int, &src) == 0);
    ASSERT_TRUE(Set_contains(int, &dest, 4));
    ASSERT_TRUE(Set_contains(int, &dest, 5));
    ASSERT_TRUE(Set_contains(int, &dest, 6));

    Set_free(int, &dest);
    Set_free(int, &src);
}

TEST(join_overlapping_sets)
{
    Set(int) dest = Set_init(int);
    Set(int) src = Set_init(int);

    // Add some elements to both sets
    Set_insert(int, &dest, 1);
    Set_insert(int, &dest, 2);
    Set_insert(int, &dest, 3);

    Set_insert(int, &src, 3); // duplicate
    Set_insert(int, &src, 4);
    Set_insert(int, &src, 5);

    int added = Set_join(int, &dest, &src);
    ASSERT_TRUE(added == 2); // Only 4 and 5 are new
    ASSERT_TRUE(Set_size(int, &dest) == 5);
    ASSERT_TRUE(Set_size(int, &src) == 0);

    // Check all elements are in dest
    ASSERT_TRUE(Set_contains(int, &dest, 1));
    ASSERT_TRUE(Set_contains(int, &dest, 2));
    ASSERT_TRUE(Set_contains(int, &dest, 3));
    ASSERT_TRUE(Set_contains(int, &dest, 4));
    ASSERT_TRUE(Set_contains(int, &dest, 5));

    Set_free(int, &dest);
    Set_free(int, &src);
}

TEST(join_large_sets)
{
    Set(int) dest = Set_init(int);
    Set(int) src = Set_init(int);

    // Add even numbers to dest
    for (int i = 0; i < 50; i += 2)
    {
        Set_insert(int, &dest, i);
    }

    // Add odd numbers and some even numbers to src
    for (int i = 1; i < 50; i += 2)
    {
        Set_insert(int, &src, i);
    }
    Set_insert(int, &src, 10); // duplicate
    Set_insert(int, &src, 20); // duplicate

    int added = Set_join(int, &dest, &src);
    ASSERT_TRUE(added == 25);                // 25 odd numbers added
    ASSERT_TRUE(Set_size(int, &dest) == 50); // all numbers 0-49
    ASSERT_TRUE(Set_size(int, &src) == 0);

    // Verify all numbers are present
    for (int i = 0; i < 50; i++)
    {
        ASSERT_TRUE(Set_contains(int, &dest, i));
    }

    Set_free(int, &dest);
    Set_free(int, &src);
}

TEST(join_and_then_insert)
{
    Set(int) dest = Set_init(int);
    Set(int) src = Set_init(int);

    Set_insert(int, &dest, 1);
    Set_insert(int, &src, 2);
    Set_insert(int, &src, 3);

    int added = Set_join(int, &dest, &src);
    ASSERT_TRUE(added == 2);
    ASSERT_TRUE(Set_size(int, &dest) == 3);

    // Should still be able to insert after join
    ASSERT_TRUE(Set_insert(int, &dest, 4));
    ASSERT_TRUE(Set_size(int, &dest) == 4);
    ASSERT_TRUE(Set_contains(int, &dest, 4));

    Set_free(int, &dest);
    Set_free(int, &src);
}

TEST(join_null_source)
{
    Set(int) dest = Set_init(int);
    Set_insert(int, &dest, 1);
    Set_insert(int, &dest, 2);

    // Test with NULL source (should be safe)
    int added = Set_join(int, &dest, NULL);
    ASSERT_TRUE(added == 0);
    ASSERT_TRUE(Set_size(int, &dest) == 2);

    Set_free(int, &dest);
}

/**
 * Test a compelx case of destruction of memory allocated structs
 */
TEST(join_with_destructor)
{
    Set(AllocatedInt) dest = Set_init(AllocatedInt);
    Set(AllocatedInt) src = Set_init(AllocatedInt);

    // 1. Insert unique elements into dest (10, 20)
    Set_insert(AllocatedInt, &dest, new_allocated_int(10));
    Set_insert(AllocatedInt, &dest, new_allocated_int(20));

    // 2. Insert overlapping and unique elements into src (20, 30)
    Set_insert(AllocatedInt, &src, new_allocated_int(20)); // Duplicate value
    Set_insert(AllocatedInt, &src, new_allocated_int(30)); // New value

    // 3. Reset counter (destructor should not have been called yet)
    destructor_call_count = 0;

    // Total unique elements inserted: 3 (10, 20, 30)

    // 4. Join the sets
    int added = Set_join(AllocatedInt, &dest, &src);

    // Check successful merge
    ASSERT_TRUE(added == 1); // Only '30' was new
    ASSERT_TRUE(Set_size(AllocatedInt, &dest) == 3);
    ASSERT_TRUE(Set_size(AllocatedInt, &src) == 0);

    // Crucial Check: Destructor MUST NOT have been called on transferred elements
    // The duplicate '20' from src should have been destroyed (destructor called once)
    // The new '30' was transferred and should NOT be destroyed yet.
    // The old '20' from src (which was copied but not inserted) should have been destroyed.
    // If the implementation correctly detects duplication *before* the join,
    // the source's '20' is cleaned up *by the clear* or *by the insert failure*.
    // If the '30' value was zeroed out during transfer, the count should be 1 (for the duplicate '20' that was in src).
    
    // Simplest assumption for *correct* transfer logic: 
    // The duplicate '20' node in SRC is freed and its destructor called (1 call).
    // The transferred '30' node in SRC is freed, but its value is NULLed, so its destructor is NOT called.
    ASSERT_TRUE(destructor_call_count >= 1); 

    // 5. Free the destination set (This should trigger the destructor for all 3 unique elements)
    Set_free(AllocatedInt, &dest);
    
    // Free the source set (should be empty, no further destructor calls)
    Set_free(AllocatedInt, &src); 

    // Final Check: Destructor should have been called 3 times in total 
    // (for 10, 20, and 30 which were in the final 'dest' set)
    // PLUS 1 time for the duplicate '20' in 'src' that was rejected.
    // Total free count must be 4.
    ASSERT_TRUE(destructor_call_count == 4); 
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
    RUN_TEST(large_set_operations),
    RUN_TEST(red_black_invariants),
    RUN_TEST(sequential_insert_remove),
    RUN_TEST(reverse_order_removal),
    RUN_TEST(randomized_stress),
    RUN_TEST(join_empty_sets),
    RUN_TEST(join_with_empty_source),
    RUN_TEST(join_with_empty_destination),
    RUN_TEST(join_overlapping_sets),
    RUN_TEST(join_large_sets),
    RUN_TEST(join_and_then_insert),
    RUN_TEST(join_null_source),
    RUN_TEST(join_with_destructor))