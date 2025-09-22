#include "CLIP/Test.h"
#include "CLIP/List.h"

CLIP_DEFINE_LIST_TYPE(int)

// --- Test Cases ---
TEST(init)
{
    List(int) xs = List_init(int, 4);

    ASSERT_TRUE(xs.size == 0);
    ASSERT_TRUE(xs.capacity == 4);
    ASSERT_NOT_NULL(xs.data);

    List_free(int, &xs);
}

TEST(init_from_array_and_static_array)
{
    int arr[] = {1, 2, 3};
    List(int) xs = List_init_from_array(int, arr, 3);

    ASSERT_TRUE(xs.size == 3);
    ASSERT_TRUE(List_get(int, &xs, 0) == 1);
    ASSERT_TRUE(List_get(int, &xs, 2) == 3);

    List_free(int, &xs);

    List(int) ys = List_init_from_static_array(int, arr);
    ASSERT_TRUE(ys.size == 3);
    ASSERT_TRUE(List_get(int, &ys, 1) == 2);

    List_free(int, &ys);
}

TEST(append_and_get)
{
    List(int) xs = List_init(int, 2);
    ASSERT_TRUE(List_append(int, &xs, 10));
    ASSERT_TRUE(List_append(int, &xs, 20));

    ASSERT_TRUE(xs.size == 2);
    ASSERT_TRUE(List_get(int, &xs, 0) == 10);
    ASSERT_TRUE(List_get(int, &xs, 1) == 20);

    List_free(int, &xs);
}

TEST(get_and_get_ptr_bounds)
{
    List(int) xs = List_init(int, 2);
    List_append(int, &xs, 5);

    // valid access
    ASSERT_TRUE(List_get(int, &xs, 0) == 5);
    int *p = List_get_ptr(int, &xs, 0);
    ASSERT_NOT_NULL(p);
    ASSERT_TRUE(*p == 5);

    // invalid access (out of bounds)
    ASSERT_TRUE(List_get(int, &xs, 99) == 0); // safe fallback
    ASSERT_TRUE(List_get_ptr(int, &xs, 99) == NULL);

    List_free(int, &xs);
}

TEST(at_and_at_ptr)
{
    List(int) xs = List_init(int, 2);
    List_append(int, &xs, 10);
    List_append(int, &xs, 20);

    // unchecked access
    ASSERT_TRUE(List_at(int, &xs, 1) == 20);

    int *p = List_at_ptr(int, &xs, 0);
    ASSERT_TRUE(*p == 10);
    *p = 99;
    ASSERT_TRUE(List_at(int, &xs, 0) == 99);

    List_free(int, &xs);
}

TEST(insert_and_remove)
{
    List(int) xs = List_init(int, 2);
    List_append(int, &xs, 1);
    List_append(int, &xs, 3);

    ASSERT_TRUE(List_insert(int, &xs, 1, 2));
    ASSERT_TRUE(xs.size == 3);
    ASSERT_TRUE(List_get(int, &xs, 1) == 2);

    ASSERT_TRUE(List_remove_at(int, &xs, 1));
    ASSERT_TRUE(xs.size == 2);
    ASSERT_TRUE(List_get(int, &xs, 1) == 3);

    List_free(int, &xs);
}

TEST(clear_and_reserve)
{
    List(int) xs = List_init(int, 2);
    List_append(int, &xs, 1);
    List_append(int, &xs, 2);

    List_clear(int, &xs);
    ASSERT_TRUE(xs.size == 0);
    ASSERT_TRUE(xs.capacity >= 2);

    ASSERT_TRUE(List_reserve(int, &xs, 10));
    ASSERT_TRUE(xs.capacity >= 10);

    List_free(int, &xs);
}

// Comparator function for the following test
int compare_ints_ascending(const int *a, const int *b)
{
  return (*a > *b) - (*a < *b);
}


TEST(reverse_and_sort)
{
    int values[] = {3, 1, 2};
    List(int) xs = List_init_from_array(int, values, 3);

    List_reverse(int, &xs);
    ASSERT_TRUE(List_get(int, &xs, 0) == 2);
    ASSERT_TRUE(List_get(int, &xs, 2) == 3);

    // sort ascending
    ASSERT_TRUE(List_sort(int, &xs, compare_ints_ascending));
    ASSERT_TRUE(List_get(int, &xs, 0) == 1);
    ASSERT_TRUE(List_get(int, &xs, 2) == 3);

    List_free(int, &xs);
}

TEST(pop)
{
    List(int) xs = List_init(int, 2);
    List_append(int, &xs, 42);
    int out;

    ASSERT_TRUE(List_pop(int, &xs, &out));
    ASSERT_TRUE(out == 42);
    ASSERT_TRUE(xs.size == 0);

    List_free(int, &xs);
}

TEST(merge)
{
    int arr_a[] = {1, 2};
    int arr_b[] = {3, 4};
    List(int) a = List_init_from_array(int, arr_a, 2);
    List(int) b = List_init_from_array(int, arr_b, 2);

    ASSERT_TRUE(List_merge(int, &a, &b));
    ASSERT_TRUE(a.size == 4);
    ASSERT_TRUE(List_get(int, &a, 2) == 3);
    ASSERT_TRUE(List_get(int, &a, 3) == 4);

    List_free(int, &a);
    List_free(int, &b);
}

TEST(foreach_macro)
{
    int data[] = {1, 2, 3};
    List(int) xs = List_init_from_array(int, data, 3);

    int sum = 0;
    List_foreach(int, item, &xs)
    {
        sum += *item;
        *item *= 2; // modify in place
    }

    ASSERT_TRUE(sum == 6);
    ASSERT_TRUE(List_get(int, &xs, 0) == 2);
    ASSERT_TRUE(List_get(int, &xs, 2) == 6);

    List_free(int, &xs);
}

TEST_SUITE(
    RUN_TEST(init),
    RUN_TEST(init_from_array_and_static_array),
    RUN_TEST(get_and_get_ptr_bounds),
    RUN_TEST(at_and_at_ptr),
    RUN_TEST(append_and_get),
    RUN_TEST(insert_and_remove),
    RUN_TEST(clear_and_reserve),
    RUN_TEST(reverse_and_sort),
    RUN_TEST(pop),
    RUN_TEST(merge),
    RUN_TEST(foreach_macro))