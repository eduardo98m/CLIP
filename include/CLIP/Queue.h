/*
 * @author Eduardo I. Lopez H. - eduardo98m@gmail.com
 * @brief In this file we define a series of macros that generates a **type-safe** queue
 * (FIFO) for any given element type in C using a circular buffer.
 *
 * Example:
 * // Generates Queue(int) and associated functions
 * CLIP_DEFINE_QUEUE_TYPE(int)
 *
 * Queue(int) q = Queue_init(int, 4); // Capacity of 4
 * Queue_enqueue(int, &q, 10);
 * Queue_enqueue(int, &q, 20);
 *
 * int val;
 * if (Queue_dequeue(int, &q, &val)) {
 * // val is 10
 * }
 *
 * Queue_free(int, &q);
 */
#ifndef CLIP_QUEUE_H
#define CLIP_QUEUE_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CLIP_DEFINE_QUEUE_TYPE(...) \
  CLIP_DEFINE_QUEUE_TYPE_IMPL(__VA_ARGS__, 256)

/**
 * @brief Define a type-safe queue for the given element type.
 *
 * This macro generates:
 * - A typedef `Queue_<Type>` structure.
 * - A set of **static inline functions** specialized for `<Type>`:
 * - `init_queue_<Type>`
 * - `free_queue_<Type>`
 * - `queue_is_empty_<Type>`
 * - `queue_is_full_<Type>`
 * - `queue_size_<Type>`
 * - `queue_enqueue_<Type>`
 * - `queue_dequeue_<Type>`
 * - `queue_peek_<Type>`
 * - `queue_peek_ptr_<Type>`
 * - `queue_clear_<Type>`
 * - `queue_to_str_<Type>_custom`
 *
 * @param Type The element type (e.g., `int`, `float`, `struct Foo`).
 * @param BUF_SIZE Optional: Buffer size allocated per element for string conversion. Default is 256.
 */
#define CLIP_DEFINE_QUEUE_TYPE_IMPL(Type, BUF_SIZE, ...)                       \
  typedef struct                                                               \
  {                                                                            \
    Type *data;                                                                \
    int head;     /* Index of the front element */                             \
    int tail;     /* Index where the next element will be inserted */          \
    int count;    /* Number of elements in the queue */                        \
    int capacity; /* Total storage capacity */                                 \
  } Queue_##Type;                                                              \
                                                                               \
  static inline Queue_##Type init_queue_##Type(int capacity)                   \
  {                                                                            \
    Queue_##Type q;                                                            \
    /* Allocate one extra slot to easily distinguish full from empty */        \
    q.data = malloc((capacity + 1) * sizeof(Type));                            \
    if (!q.data)                                                               \
    {                                                                          \
      fprintf(stderr, "Queue memory allocation failed!\n");                    \
      exit(EXIT_FAILURE);                                                      \
    }                                                                          \
    q.capacity = capacity;                                                     \
    q.head = 0;                                                                \
    q.tail = 0;                                                                \
    q.count = 0;                                                               \
    return q;                                                                  \
  }                                                                            \
                                                                               \
  static inline void free_queue_##Type(Queue_##Type *q)                        \
  {                                                                            \
    free(q->data);                                                             \
    q->data = NULL;                                                            \
    q->capacity = 0;                                                           \
    q->head = 0;                                                               \
    q->tail = 0;                                                               \
    q->count = 0;                                                              \
  }                                                                            \
                                                                               \
  static inline bool queue_is_empty_##Type(const Queue_##Type *q)              \
  {                                                                            \
    return q->count == 0;                                                      \
  }                                                                            \
                                                                               \
  static inline bool queue_is_full_##Type(const Queue_##Type *q)               \
  {                                                                            \
    return q->count == q->capacity;                                            \
  }                                                                            \
                                                                               \
  static inline int queue_size_##Type(const Queue_##Type *q)                   \
  {                                                                            \
    return q->count;                                                           \
  }                                                                            \
                                                                               \
  static inline bool queue_enqueue_##Type(Queue_##Type *q, Type value)         \
  {                                                                            \
    if (queue_is_full_##Type(q))                                               \
    {                                                                          \
      return false; /* Queue is full */                                        \
    }                                                                          \
    q->data[q->tail] = value;                                                  \
    q->tail = (q->tail + 1) % (q->capacity + 1);                               \
    q->count++;                                                                \
    return true;                                                               \
  }                                                                            \
                                                                               \
  static inline bool queue_dequeue_##Type(Queue_##Type *q, Type *out_value)    \
  {                                                                            \
    if (queue_is_empty_##Type(q))                                              \
    {                                                                          \
      return false; /* Queue is empty */                                       \
    }                                                                          \
    if (out_value)                                                             \
    {                                                                          \
      *out_value = q->data[q->head];                                           \
    }                                                                          \
    q->head = (q->head + 1) % (q->capacity + 1);                               \
    q->count--;                                                                \
    return true;                                                               \
  }                                                                            \
                                                                               \
  static inline bool queue_peek_##Type(const Queue_##Type *q, Type *out_value) \
  {                                                                            \
    if (queue_is_empty_##Type(q))                                              \
    {                                                                          \
      return false;                                                            \
    }                                                                          \
    if (out_value)                                                             \
    {                                                                          \
      *out_value = q->data[q->head];                                           \
    }                                                                          \
    return true;                                                               \
  }                                                                            \
                                                                               \
  static inline Type *queue_peek_ptr_##Type(const Queue_##Type *q)             \
  {                                                                            \
    if (queue_is_empty_##Type(q))                                              \
    {                                                                          \
      return NULL;                                                             \
    }                                                                          \
    return &q->data[q->head];                                                  \
  }                                                                            \
                                                                               \
  static inline void queue_clear_##Type(Queue_##Type *q)                       \
  {                                                                            \
    q->head = 0;                                                               \
    q->tail = 0;                                                               \
    q->count = 0;                                                              \
  }                                                                            \
                                                                               \
  static inline char *queue_to_str_##Type##_custom(                            \
      const Queue_##Type *q, void (*elem_to_str)(Type, char *, size_t))        \
  {                                                                            \
    if (!q)                                                                    \
      return NULL;                                                             \
    if (q->count == 0)                                                         \
    {                                                                          \
      char *buffer = malloc(3);                                                \
      if (!buffer)                                                             \
        return NULL;                                                           \
      strcpy(buffer, "[]");                                                    \
      return buffer;                                                           \
    }                                                                          \
    size_t bufsize = q->count * BUF_SIZE + 3;                                  \
    char *buffer = malloc(bufsize);                                            \
    if (!buffer)                                                               \
      return NULL;                                                             \
    strcpy(buffer, "[");                                                       \
    char elem[BUF_SIZE];                                                       \
    for (int i = 0; i < q->count; i++)                                         \
    {                                                                          \
      int current_index = (q->head + i) % (q->capacity + 1);                   \
      elem_to_str(q->data[current_index], elem, sizeof(elem));                 \
      strcat(buffer, elem);                                                    \
      if (i < q->count - 1)                                                    \
        strcat(buffer, ", ");                                                  \
    }                                                                          \
    strcat(buffer, "]");                                                       \
    return buffer;                                                             \
  }

#define CLIP_REGISTER_QUEUE_PRINT(Type, print_fn)                                  \
  static inline char *queue_to_str_##Type##_main(const Queue_##Type *q)            \
  {                                                                                \
    if (!print_fn)                                                                 \
    {                                                                              \
      fprintf(stderr, "No print function registered for queue type " #Type "!\n"); \
      return NULL;                                                                 \
    }                                                                              \
    return queue_to_str_##Type##_custom(q, print_fn);                              \
  }

/* --- Convenience Macros --- */

#define Queue(Type) Queue_##Type
#define Queue_init(Type, cap) init_queue_##Type(cap)
#define Queue_free(Type, q) free_queue_##Type(q)
#define Queue_is_empty(Type, q) queue_is_empty_##Type(q)
#define Queue_is_full(Type, q) queue_is_full_##Type(q)
#define Queue_size(Type, q) queue_size_##Type(q)
#define Queue_enqueue(Type, q, val) queue_enqueue_##Type(q, val)
#define Queue_dequeue(Type, q, out) queue_dequeue_##Type(q, out)
#define Queue_peek(Type, q, out) queue_peek_##Type(q, out)
#define Queue_peek_ptr(Type, q) queue_peek_ptr_##Type(q)
#define Queue_clear(Type, q) queue_clear_##Type(q)
#define Queue_to_str(Type, q) queue_to_str_##Type##_main(q)
#define Queue_to_str_custom(Type, q, fn) queue_to_str_##Type##_custom(q, fn)

#define Queue_print(Type, q)                \
  do                                        \
  {                                         \
    char *_tmp_str = Queue_to_str(Type, q); \
    if (_tmp_str)                           \
    {                                       \
      printf("%s", _tmp_str);               \
      free(_tmp_str);                       \
    }                                       \
  } while (0)

#define Queue_print_custom(Type, q, fn)                \
  do                                                   \
  {                                                    \
    char *_tmp_str = Queue_to_str_custom(Type, q, fn); \
    if (_tmp_str)                                      \
    {                                                  \
      printf("%s", _tmp_str);                          \
      free(_tmp_str);                                  \
    }                                                  \
  } while (0)

#define Queue_println(Type, q) \
  do                           \
  {                            \
    Queue_print(Type, q);      \
    printf("\n");              \
  } while (0)

#define Queue_println_custom(Type, q, fn) \
  do                                      \
  {                                       \
    Queue_print_custom(Type, q, fn);      \
    printf("\n");                         \
  } while (0)

#endif /* CLIP_QUEUE_H */