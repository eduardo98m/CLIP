#include "CLIP/Test.h"
#include "CLIP/Stack.h"

CLIP_DEFINE_STACK_TYPE(int)

// --- Test Cases ---
TEST(init)
{
    Stack(int) stack = Stack_init(int, 4);

    ASSERT_TRUE(Stack_size(int, &stack) == 0);
    ASSERT_TRUE(Stack_capacity(int, &stack) == 4);
    ASSERT_TRUE(Stack_is_empty(int, &stack));
    ASSERT_NOT_NULL(stack.data);

    Stack_free(int, &stack);
}

TEST(init_from_array_and_static_array)
{
    // Array elements: 1, 2, 3 -> Stack (top to bottom): 3, 2, 1
    int arr[] = {1, 2, 3};
    Stack(int) stack = Stack_init_from_array(int, arr, 3);

    ASSERT_TRUE(Stack_size(int, &stack) == 3);
    ASSERT_FALSE(Stack_is_empty(int, &stack));

    int top_val;
    ASSERT_TRUE(Stack_peek(int, &stack, &top_val));
    ASSERT_TRUE(top_val == 3); // Last element of array should be at top

    Stack_free(int, &stack);

    Stack(int) stack2 = Stack_init_from_static_array(int, arr);
    ASSERT_TRUE(Stack_size(int, &stack2) == 3);
    ASSERT_TRUE(Stack_peek(int, &stack2, &top_val));
    ASSERT_TRUE(top_val == 3);

    Stack_free(int, &stack2);
}

TEST(push_and_peek)
{
    Stack(int) stack = Stack_init(int, 2);

    ASSERT_TRUE(Stack_push(int, &stack, 10));
    ASSERT_TRUE(Stack_push(int, &stack, 20));

    ASSERT_TRUE(Stack_size(int, &stack) == 2);
    ASSERT_FALSE(Stack_is_empty(int, &stack));

    int peek_val;
    ASSERT_TRUE(Stack_peek(int, &stack, &peek_val));
    ASSERT_TRUE(peek_val == 20);               // Last pushed should be at top
    ASSERT_TRUE(Stack_size(int, &stack) == 2); // Size shouldn't change after peek

    Stack_free(int, &stack);
}

TEST(peek_ptr)
{
    Stack(int) stack = Stack_init(int, 2);
    Stack_push(int, &stack, 42);

    // Valid peek
    int *ptr = Stack_peek_ptr(int, &stack);
    ASSERT_NOT_NULL(ptr);
    ASSERT_TRUE(*ptr == 42);

    // Modify through pointer
    *ptr = 99;
    int peek_val;
    Stack_peek(int, &stack, &peek_val);
    ASSERT_TRUE(peek_val == 99);

    // Empty stack peek
    Stack_clear(int, &stack);
    ptr = Stack_peek_ptr(int, &stack);
    ASSERT_TRUE(ptr == NULL);

    Stack_free(int, &stack);
}

TEST(pop)
{
    Stack(int) stack = Stack_init(int, 2);
    Stack_push(int, &stack, 10);
    Stack_push(int, &stack, 20);

    int pop_val;
    ASSERT_TRUE(Stack_pop(int, &stack, &pop_val));
    ASSERT_TRUE(pop_val == 20); // LIFO: last in, first out
    ASSERT_TRUE(Stack_size(int, &stack) == 1);

    ASSERT_TRUE(Stack_pop(int, &stack, &pop_val));
    ASSERT_TRUE(pop_val == 10);
    ASSERT_TRUE(Stack_size(int, &stack) == 0);
    ASSERT_TRUE(Stack_is_empty(int, &stack));

    // Pop from empty stack should fail
    ASSERT_FALSE(Stack_pop(int, &stack, &pop_val));

    Stack_free(int, &stack);
}

TEST(pop_without_output)
{
    Stack(int) stack = Stack_init(int, 2);
    Stack_push(int, &stack, 42);

    // Pop without caring about the value
    ASSERT_TRUE(Stack_pop(int, &stack, NULL));
    ASSERT_TRUE(Stack_is_empty(int, &stack));

    Stack_free(int, &stack);
}

TEST(capacity_and_resize)
{
    Stack(int) stack = Stack_init(int, 2);

    // Fill initial capacity
    Stack_push(int, &stack, 1);
    Stack_push(int, &stack, 2);
    ASSERT_TRUE(Stack_capacity(int, &stack) == 2);

    // This should trigger a resize
    Stack_push(int, &stack, 3);
    ASSERT_TRUE(Stack_capacity(int, &stack) > 2);
    ASSERT_TRUE(Stack_size(int, &stack) == 3);

    Stack_free(int, &stack);
}

TEST(clear_and_reserve)
{
    Stack(int) stack = Stack_init(int, 2);
    Stack_push(int, &stack, 1);
    Stack_push(int, &stack, 2);
    Stack_push(int, &stack, 3);

    Stack_clear(int, &stack);
    ASSERT_TRUE(Stack_size(int, &stack) == 0);
    ASSERT_TRUE(Stack_is_empty(int, &stack));
    ASSERT_TRUE(Stack_capacity(int, &stack) > 0); // capacity should remain

    ASSERT_TRUE(Stack_reserve(int, &stack, 20));
    ASSERT_TRUE(Stack_capacity(int, &stack) >= 20);

    // Should be able to use after clearing and reserving
    Stack_push(int, &stack, 42);
    ASSERT_TRUE(Stack_size(int, &stack) == 1);

    Stack_free(int, &stack);
}

TEST(shrink_to_fit)
{
    Stack(int) stack = Stack_init(int, 10);
    Stack_push(int, &stack, 1);
    Stack_push(int, &stack, 2);

    // Should have excess capacity
    ASSERT_TRUE(Stack_capacity(int, &stack) > Stack_size(int, &stack));

    ASSERT_TRUE(Stack_shrink_to_fit(int, &stack));
    ASSERT_TRUE(Stack_capacity(int, &stack) == Stack_size(int, &stack));

    // Should still work after shrinking
    int val;
    ASSERT_TRUE(Stack_pop(int, &stack, &val));
    ASSERT_TRUE(val == 2);

    Stack_free(int, &stack);
}

TEST(reverse)
{
    Stack(int) stack = Stack_init(int, 5);

    // Push elements 1, 2, 3, 4, 5 (5 will be at top)
    for (int i = 1; i <= 5; i++)
    {
        Stack_push(int, &stack, i);
    }

    Stack_reverse(int, &stack);

    // After reverse, 1 should be at top
    int val;
    ASSERT_TRUE(Stack_pop(int, &stack, &val));
    ASSERT_TRUE(val == 1);
    ASSERT_TRUE(Stack_pop(int, &stack, &val));
    ASSERT_TRUE(val == 2);
    ASSERT_TRUE(Stack_pop(int, &stack, &val));
    ASSERT_TRUE(val == 3);

    Stack_free(int, &stack);
}

TEST(copy)
{
    Stack(int) original = Stack_init(int, 3);
    Stack_push(int, &original, 10);
    Stack_push(int, &original, 20);
    Stack_push(int, &original, 30);

    Stack(int) copy = Stack_copy(int, &original);

    ASSERT_TRUE(Stack_size(int, &copy) == Stack_size(int, &original));
    ASSERT_TRUE(Stack_capacity(int, &copy) == Stack_capacity(int, &original));

    // Verify elements are the same
    int orig_val, copy_val;
    while (!Stack_is_empty(int, &original))
    {
        ASSERT_TRUE(Stack_pop(int, &original, &orig_val));
        ASSERT_TRUE(Stack_pop(int, &copy, &copy_val));
        ASSERT_TRUE(orig_val == copy_val);
    }

    ASSERT_TRUE(Stack_is_empty(int, &copy));

    Stack_free(int, &original);
    Stack_free(int, &copy);
}

// Element to string function for testing print functionality
void int_to_str(int value, char *buffer, size_t size)
{
    snprintf(buffer, size, "%d", value);
}

// Register print function for int
CLIP_REGISTER_STACK_PRINT(int, int_to_str)

TEST(to_str_custom)
{
    Stack(int) stack = Stack_init(int, 5);

    // Test empty stack
    char *str = Stack_to_str_custom(int, &stack, int_to_str);
    ASSERT_NOT_NULL(str);
    ASSERT_TRUE(strcmp(str, "[top:  :bottom]") == 0);
    free(str);

    // Test with elements
    Stack_push(int, &stack, 10);
    Stack_push(int, &stack, 20);
    Stack_push(int, &stack, 30);

    str = Stack_to_str_custom(int, &stack, int_to_str);
    ASSERT_NOT_NULL(str);

    // Should show from top to bottom: 30, 20, 10
    ASSERT_TRUE(strstr(str, "30") != NULL);
    ASSERT_TRUE(strstr(str, "20") != NULL);
    ASSERT_TRUE(strstr(str, "10") != NULL);
    ASSERT_TRUE(strstr(str, "[top:") != NULL);
    ASSERT_TRUE(strstr(str, ":bottom]") != NULL);

    free(str);
    Stack_free(int, &stack);
}

TEST(to_str_registered)
{
    Stack(int) stack = Stack_init(int, 3);
    Stack_push(int, &stack, 42);
    Stack_push(int, &stack, 17);

    char *str = Stack_to_str(int, &stack);
    ASSERT_NOT_NULL(str);

    // Should contain both values
    ASSERT_TRUE(strstr(str, "42") != NULL);
    ASSERT_TRUE(strstr(str, "17") != NULL);
    ASSERT_TRUE(strstr(str, "[top:") != NULL);
    ASSERT_TRUE(strstr(str, ":bottom]") != NULL);

    free(str);
    Stack_free(int, &stack);
}

TEST(large_stack_operations)
{
    Stack(int) stack = Stack_init(int, 10);

    // Push many elements
    const int count = 1000;
    for (int i = 0; i < count; i++)
    {
        ASSERT_TRUE(Stack_push(int, &stack, i));
    }

    ASSERT_TRUE(Stack_size(int, &stack) == count);

    // Peek should show last pushed element
    int peek_val;
    ASSERT_TRUE(Stack_peek(int, &stack, &peek_val));
    ASSERT_TRUE(peek_val == count - 1);

    // Pop all elements in reverse order
    for (int i = count - 1; i >= 0; i--)
    {
        int val;
        ASSERT_TRUE(Stack_pop(int, &stack, &val));
        ASSERT_TRUE(val == i);
    }

    ASSERT_TRUE(Stack_is_empty(int, &stack));
    ASSERT_TRUE(Stack_size(int, &stack) == 0);

    Stack_free(int, &stack);
}

TEST(stress_push_pop_mixed)
{
    Stack(int) stack = Stack_init(int, 4);
    const int operations = 10000;
    int expected_top = -1;
    int current_size = 0;

    for (int i = 0; i < operations; i++)
    {
        if (current_size == 0 || (rand() % 3 == 0 && current_size < 500))
        {
            // Push operation (75% chance when not empty, 100% when empty)
            int value = rand() % 1000;
            ASSERT_TRUE(Stack_push(int, &stack, value));
            expected_top = value;
            current_size++;
        }
        else
        {
            // Pop operation
            int popped;
            ASSERT_TRUE(Stack_pop(int, &stack, &popped));
            ASSERT_TRUE(popped == expected_top);
            current_size--;

            if (current_size > 0)
            {
                // We don't know what the new top is, so just peek to update expected_top
                ASSERT_TRUE(Stack_peek(int, &stack, &expected_top));
            }
            else
            {
                expected_top = -1;
            }
        }

        ASSERT_TRUE(Stack_size(int, &stack) == current_size);
        ASSERT_TRUE(Stack_is_empty(int, &stack) == (current_size == 0));

        // Verify peek consistency
        if (current_size > 0)
        {
            int peek_val;
            ASSERT_TRUE(Stack_peek(int, &stack, &peek_val));
            ASSERT_TRUE(peek_val == expected_top);
        }
    }

    Stack_free(int, &stack);
}

TEST(stress_capacity_management)
{
    Stack(int) stack = Stack_init(int, 1);

    // Test rapid growth and shrinkage
    for (int cycle = 0; cycle < 10; cycle++)
    {
        // Grow phase
        for (int i = 0; i < 100; i++)
        {
            Stack_push(int, &stack, i + cycle * 100);
        }

        ASSERT_TRUE(Stack_size(int, &stack) == 100);
        ASSERT_TRUE(Stack_capacity(int, &stack) >= 100);

        // Shrink phase
        for (int i = 99; i >= 0; i--)
        {
            int val;
            ASSERT_TRUE(Stack_pop(int, &stack, &val));
            ASSERT_TRUE(val == i + cycle * 100);
        }

        ASSERT_TRUE(Stack_is_empty(int, &stack));

        // Optionally shrink to fit
        if (cycle % 2 == 0)
        {
            Stack_shrink_to_fit(int, &stack);
            ASSERT_TRUE(Stack_capacity(int, &stack) == 0);
        }
    }

    Stack_free(int, &stack);
}

TEST(lifo_property_stress)
{
    Stack(int) stack = Stack_init(int, 5);
    const int test_size = 500;
    int values[test_size];

    // Fill array with random values
    for (int i = 0; i < test_size; i++)
    {
        values[i] = rand() % 10000;
        Stack_push(int, &stack, values[i]);
    }

    // Pop all values and verify LIFO property
    for (int i = test_size - 1; i >= 0; i--)
    {
        int popped;
        ASSERT_TRUE(Stack_pop(int, &stack, &popped));
        ASSERT_TRUE(popped == values[i]);
    }

    ASSERT_TRUE(Stack_is_empty(int, &stack));

    Stack_free(int, &stack);
}

TEST_SUITE(
    RUN_TEST(init),
    RUN_TEST(init_from_array_and_static_array),
    RUN_TEST(push_and_peek),
    RUN_TEST(peek_ptr),
    RUN_TEST(pop),
    RUN_TEST(pop_without_output),
    RUN_TEST(capacity_and_resize),
    RUN_TEST(shrink_to_fit),
    RUN_TEST(reverse),
    RUN_TEST(copy),
    RUN_TEST(to_str_custom),
    RUN_TEST(to_str_registered),
    RUN_TEST(large_stack_operations),
    RUN_TEST(stress_push_pop_mixed),
    RUN_TEST(stress_capacity_management),
    RUN_TEST(lifo_property_stress))