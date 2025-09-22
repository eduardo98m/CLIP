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

TEST_SUITE(
    RUN_TEST(init),
    RUN_TEST(append_and_get),
    RUN_TEST(insert_and_remove),
    RUN_TEST(pop))