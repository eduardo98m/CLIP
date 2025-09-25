#include "CLIP/Test.h" // Assuming your test framework header
#include "CLIP/Queue.h"

CLIP_DEFINE_QUEUE_TYPE(int)

// --- Test Cases ---

TEST(init_and_free)
{
    Queue(int) q = Queue_init(int, 10);

    ASSERT_TRUE(q.capacity == 10);
    ASSERT_NOT_NULL(q.data);
    ASSERT_TRUE(q.count == 0);
    ASSERT_TRUE(q.head == 0);
    ASSERT_TRUE(q.tail == 0);
    ASSERT_TRUE(Queue_is_empty(int, &q));
    ASSERT_FALSE(Queue_is_full(int, &q));

    Queue_free(int, &q);
    ASSERT_NULL(q.data);
    ASSERT_TRUE(q.capacity == 0);
}

TEST(enqueue_and_size)
{
    Queue(int) q = Queue_init(int, 3);

    ASSERT_TRUE(Queue_size(int, &q) == 0);

    ASSERT_TRUE(Queue_enqueue(int, &q, 10));
    ASSERT_TRUE(Queue_size(int, &q) == 1);
    ASSERT_FALSE(Queue_is_empty(int, &q));

    ASSERT_TRUE(Queue_enqueue(int, &q, 20));
    ASSERT_TRUE(Queue_size(int, &q) == 2);

    ASSERT_TRUE(Queue_enqueue(int, &q, 30));
    ASSERT_TRUE(Queue_size(int, &q) == 3);
    ASSERT_TRUE(Queue_is_full(int, &q));

    // Queue is full, should fail
    ASSERT_FALSE(Queue_enqueue(int, &q, 40));
    ASSERT_TRUE(Queue_size(int, &q) == 3);

    Queue_free(int, &q);
}

TEST(dequeue)
{
    Queue(int) q = Queue_init(int, 3);
    Queue_enqueue(int, &q, 10);
    Queue_enqueue(int, &q, 20);

    int val;
    ASSERT_TRUE(Queue_dequeue(int, &q, &val));
    ASSERT_TRUE(val == 10);
    ASSERT_TRUE(Queue_size(int, &q) == 1);

    ASSERT_TRUE(Queue_dequeue(int, &q, &val));
    ASSERT_TRUE(val == 20);
    ASSERT_TRUE(Queue_size(int, &q) == 0);
    ASSERT_TRUE(Queue_is_empty(int, &q));

    // Queue is empty, should fail
    ASSERT_FALSE(Queue_dequeue(int, &q, &val));

    Queue_free(int, &q);
}

TEST(peek)
{
    Queue(int) q = Queue_init(int, 3);
    int val;

    // Peek on empty queue
    ASSERT_FALSE(Queue_peek(int, &q, &val));
    ASSERT_NULL(Queue_peek_ptr(int, &q));

    Queue_enqueue(int, &q, 100);
    Queue_enqueue(int, &q, 200);

    // Peek and check value
    ASSERT_TRUE(Queue_peek(int, &q, &val));
    ASSERT_TRUE(val == 100);
    // Size should not change
    ASSERT_TRUE(Queue_size(int, &q) == 2);

    // Peek via pointer
    int *ptr = Queue_peek_ptr(int, &q);
    ASSERT_NOT_NULL(ptr);
    ASSERT_TRUE(*ptr == 100);

    // Dequeue and peek again
    Queue_dequeue(int, &q, NULL);
    ASSERT_TRUE(Queue_peek(int, &q, &val));
    ASSERT_TRUE(val == 200);

    Queue_free(int, &q);
}

TEST(circular_behavior)
{
    Queue(int) q = Queue_init(int, 3);
    int val;

    // Fill the queue
    Queue_enqueue(int, &q, 1);
    Queue_enqueue(int, &q, 2);
    Queue_enqueue(int, &q, 3);
    ASSERT_TRUE(Queue_is_full(int, &q));

    // Dequeue one item to make space at the head
    Queue_dequeue(int, &q, &val);
    ASSERT_TRUE(val == 1);
    ASSERT_FALSE(Queue_is_full(int, &q));

    // Enqueue another item, which should wrap around
    ASSERT_TRUE(Queue_enqueue(int, &q, 4));
    ASSERT_TRUE(Queue_is_full(int, &q));

    // Check FIFO order
    ASSERT_TRUE(Queue_dequeue(int, &q, &val));
    ASSERT_TRUE(val == 2);

    ASSERT_TRUE(Queue_dequeue(int, &q, &val));
    ASSERT_TRUE(val == 3);

    ASSERT_TRUE(Queue_dequeue(int, &q, &val));
    ASSERT_TRUE(val == 4);

    ASSERT_TRUE(Queue_is_empty(int, &q));

    Queue_free(int, &q);
}

TEST(clear)
{
    Queue(int) q = Queue_init(int, 5);
    Queue_enqueue(int, &q, 1);
    Queue_enqueue(int, &q, 2);
    Queue_enqueue(int, &q, 3);

    ASSERT_FALSE(Queue_is_empty(int, &q));
    ASSERT_TRUE(Queue_size(int, &q) == 3);

    Queue_clear(int, &q);

    ASSERT_TRUE(Queue_is_empty(int, &q));
    ASSERT_TRUE(Queue_size(int, &q) == 0);

    // Should be able to enqueue again after clearing
    ASSERT_TRUE(Queue_enqueue(int, &q, 100));
    ASSERT_TRUE(Queue_size(int, &q) == 1);

    int val;
    Queue_peek(int, &q, &val);
    ASSERT_TRUE(val == 100);

    Queue_free(int, &q);
}

TEST(stress_test)
{
    const int capacity = 50;
    const int operations = 100000;
    Queue(int) q = Queue_init(int, capacity);

    // Simple array to mirror the queue's state for validation
    int mirror[capacity];
    int mirror_head = 0;
    int mirror_count = 0;

    int enqueued_value_counter = 0;

    srand(time(NULL));

    for (int i = 0; i < operations; ++i)
    {
        // Decide randomly to enqueue or dequeue
        if (rand() % 2 == 0)
        { // Try to enqueue
            if (!Queue_is_full(int, &q))
            {
                ASSERT_TRUE(Queue_enqueue(int, &q, enqueued_value_counter));

                // Update mirror
                int tail_index = (mirror_head + mirror_count) % capacity;
                mirror[tail_index] = enqueued_value_counter;
                mirror_count++;

                enqueued_value_counter++;
            }
        }
        else
        { // Try to dequeue
            if (!Queue_is_empty(int, &q))
            {
                int dequeued_val;
                ASSERT_TRUE(Queue_dequeue(int, &q, &dequeued_val));

                // Check against mirror
                ASSERT_TRUE(dequeued_val == mirror[mirror_head]);

                // Update mirror
                mirror_head = (mirror_head + 1) % capacity;
                mirror_count--;
            }
        }
        ASSERT_TRUE(Queue_size(int, &q) == mirror_count);
    }

    // Dequeue any remaining items and validate them
    while (!Queue_is_empty(int, &q))
    {
        int dequeued_val;
        ASSERT_TRUE(Queue_dequeue(int, &q, &dequeued_val));
        ASSERT_TRUE(dequeued_val == mirror[mirror_head]);
        mirror_head = (mirror_head + 1) % capacity;
    }

    Queue_free(int, &q);
}

// Define and run the test suite
TEST_SUITE(
    RUN_TEST(init_and_free),
    RUN_TEST(enqueue_and_size),
    RUN_TEST(dequeue),
    RUN_TEST(peek),
    RUN_TEST(circular_behavior),
    RUN_TEST(clear),
    RUN_TEST(stress_test))