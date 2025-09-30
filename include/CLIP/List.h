/*
 * @author Eduardo I. Lopez H. - eduardo98m@gmail.com
 * @brief In this file we define a series of macros that generates a **type-safe** list
 * (dynamic array) for any given element type in C.
 *
 * Example:
 * // Assuming no second argument is used for default behavior
 * CLIP_DEFINE_LIST_TYPE(int)
 *
 * List(int) xs = List_init(int, 4);
 * List_append(int, &xs, 42);
 * List_free(int, &xs);
 *
 * The following methods are generated automatically:
 * - init
 * - init_from_array
 * - ensure_capacity
 * - append
 * - pop
 * - replace
 * - insert
 * - get
 * - get_ptr
 * - at
 * - at_ptr
 * - remove_at
 * - clear
 * - reserve
 * - shrink_to_fit
 * - to_str (requires registration via CLIP_REGISTER_LIST_PRINT)
 * - to_str_custom (takes a user-supplied function)
 * - reverse
 * - sort
 * - merge
 * - free
 */
#ifndef CLIP_LIST_H
#define CLIP_LIST_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Define a type-safe dynamic list for the given element type.
 *
 * This macro generates:
 * - A typedef `List_<Type>` structure containing `data`, `size`, and `capacity`.
 * - A set of **static inline functions** specialized for `<Type>`:
 *
 * - `init_list_<Type>`
 *
 * - `init_list_from_array_<Type>`
 *
 * - `list_ensure_capacity_<Type>`
 *
 * - `list_append_<Type>`
 *
 * - `list_pop_<Type>`
 *
 * - `list_replace_<Type>`
 *
 * - `list_insert_<Type>`
 *
 * - `list_find_<Type>` // TODO
 *
 * - `list_containts_<Type>` // TODO
 *
 * - `list_get_<Type>`
 *
 * - `list_get_ptr_<Type>`
 *
 * - `list_at_<Type>`
 *
 * - `list_at_ptr_<Type>`
 *
 * - `list_remove_at_<Type>`
 *
 * - `list_clear_<Type>`
 *
 * - `list_reserve_<Type>`
 *
 * - `list_shrink_to_fit_<Type>`
 *
 * - `list_to_str_<Type>_custom`
 *
 * - `list_reverse_<Type>`
 *
 * - `list_sort_<Type>`
 *
 * - `list_slice_<Type>`
 *
 * - `list_merge_<Type>`
 *
 * - `list_copy_<Type>`
 *
 * - `free_list_<Type>`
 *
 * @param Type The element type (e.g., `int`, `float`, `struct Foo`).
 * @param BUF_SIZE Optional: Buffer size allocated per element for string conversion (used by `list_to_str_..._custom`). Default is 256.
 *
 * @note Each instantiation of this macro generates a new independent
 * list type and associated functions, all scoped within the including file.
 *
 * @note Usage examples:
 * CLIP_DEFINE_LIST_TYPE(int)             // default BUF_SIZE = 256
 * CLIP_DEFINE_LIST_TYPE(Student, 512)   // custom BUF_SIZE = 512
 */
#define CLIP_DEFINE_LIST_TYPE(...) \
  CLIP_DEFINE_LIST_TYPE_IMPL(__VA_ARGS__, NULL, 256)

/**
 * @brief Defines the List type with a given buffer size
 */
#define CLIP_DEFINE_LIST_TYPE_BUF(Type, BUF) \
  CLIP_DEFINE_LIST_TYPE_IMPL(Type, NULL, BUF)

/**
 * @brief Defines the List type with a given free function
 */
#define CLIP_DEFINE_LIST_TYPE_WITH_FREE(Type, FREE_FN) \
  CLIP_DEFINE_LIST_TYPE_IMPL(Type, FREE_FN, 256)

/**
 * @brief Defines the List type with a given free function and a custom buffer size for the print
 */
#define CLIP_DEFINE_LIST_TYPE_FULL(Type, FREE_FN, BUF) \
  CLIP_DEFINE_LIST_TYPE_IMPL(Type, FREE_FN, BUF)

/**
 * @brief Specialized implementation of `CLIP_DEFINE_LIST_TYPE` but allows the
 * user to specify a buffer size for each of the types.
 *
 * @param Type The type
 * @param BUF_SIZE The buffer size
 */
#define CLIP_DEFINE_LIST_TYPE_IMPL(Type, DestructorFunc, BUF_SIZE)                \
  typedef struct                                                                  \
  {                                                                               \
    Type *data;                                                                   \
    int size;                                                                     \
    int capacity;                                                                 \
  } List_##Type;                                                                  \
                                                                                  \
  static inline List_##Type init_list_##Type(int capacity)                        \
  {                                                                               \
    List_##Type list;                                                             \
    list.data = malloc(capacity * sizeof(Type));                                  \
    if (!list.data)                                                               \
    {                                                                             \
      fprintf(stderr, "Memory allocation failed!\n");                             \
      exit(EXIT_FAILURE);                                                         \
    }                                                                             \
    list.size = 0;                                                                \
    list.capacity = capacity;                                                     \
    return list;                                                                  \
  }                                                                               \
                                                                                  \
  static inline List_##Type init_list_from_array_##Type(const Type *arr,          \
                                                        int n)                    \
  {                                                                               \
    List_##Type list;                                                             \
    list.data = malloc(n * sizeof(Type));                                         \
    if (!list.data && n > 0)                                                      \
    {                                                                             \
      fprintf(stderr, "Memory allocation failed!\n");                             \
      exit(EXIT_FAILURE);                                                         \
    }                                                                             \
    memcpy(list.data, arr, n * sizeof(Type));                                     \
    list.size = n;                                                                \
    list.capacity = n;                                                            \
    return list;                                                                  \
  }                                                                               \
                                                                                  \
  static inline bool list_ensure_capacity_##Type(List_##Type *list,               \
                                                 int needed)                      \
  {                                                                               \
    if (list->size + needed > list->capacity)                                     \
    {                                                                             \
      int new_capacity = list->capacity ? list->capacity * 2 : 1;                 \
      while (new_capacity < list->size + needed)                                  \
        new_capacity *= 2;                                                        \
      Type *new_data = realloc(list->data, new_capacity * sizeof(Type));          \
      if (!new_data)                                                              \
        return false;                                                             \
      list->data = new_data;                                                      \
      list->capacity = new_capacity;                                              \
    }                                                                             \
    return true;                                                                  \
  }                                                                               \
                                                                                  \
  static inline bool list_append_##Type(List_##Type *list, Type value)            \
  {                                                                               \
    if (!list_ensure_capacity_##Type(list, 1))                                    \
      return false;                                                               \
    list->data[list->size++] = value;                                             \
    return true;                                                                  \
  }                                                                               \
                                                                                  \
  static inline bool list_pop_##Type(List_##Type *list, Type *out)                \
  {                                                                               \
    if (list->size == 0)                                                          \
      return false;                                                               \
    if (out)                                                                      \
      *out = list->data[list->size - 1];                                          \
    list->size--;                                                                 \
    return true;                                                                  \
  }                                                                               \
                                                                                  \
  static inline bool list_replace_##Type(List_##Type *list, int index,            \
                                         Type value)                              \
  {                                                                               \
    if (index < 0 || index >= list->size)                                         \
      return false;                                                               \
    list->data[index] = value;                                                    \
    return true;                                                                  \
  }                                                                               \
                                                                                  \
  static inline bool list_insert_##Type(List_##Type *list, int index, Type value) \
  {                                                                               \
    if (index < 0 || index > list->size)                                          \
      return false;                                                               \
    if (!list_ensure_capacity_##Type(list, 1))                                    \
      return false;                                                               \
    memmove(&list->data[index + 1], &list->data[index],                           \
            (list->size - index) * sizeof(Type));                                 \
    list->data[index] = value;                                                    \
    list->size++;                                                                 \
    return true;                                                                  \
  }                                                                               \
                                                                                  \
  static inline Type                                                              \
  list_get_##Type(List_##Type *list, int index)                                   \
  {                                                                               \
    if (index < 0 || index >= list->size)                                         \
    {                                                                             \
      fprintf(stderr, "List index out of bounds\n");                              \
      return (Type){0};                                                           \
    }                                                                             \
    return list->data[index];                                                     \
  }                                                                               \
                                                                                  \
  static inline Type *list_get_ptr_##Type(List_##Type *list, int index)           \
  {                                                                               \
    if (index < 0 || index >= list->size)                                         \
    {                                                                             \
      fprintf(stderr, "List index out of bounds\n");                              \
      return NULL;                                                                \
    }                                                                             \
    return &list->data[index];                                                    \
  }                                                                               \
                                                                                  \
  static inline Type list_at_##Type(List_##Type *list, int index)                 \
  {                                                                               \
    return list->data[index];                                                     \
  }                                                                               \
                                                                                  \
  static inline Type *list_at_ptr_##Type(List_##Type *list, int index)            \
  {                                                                               \
    return &list->data[index];                                                    \
  }                                                                               \
                                                                                  \
  static inline bool list_remove_at_##Type(List_##Type *list, int index)          \
  {                                                                               \
    if (index < 0 || index >= list->size)                                         \
      return false;                                                               \
    memmove(&list->data[index], &list->data[index + 1],                           \
            (list->size - index - 1) * sizeof(Type));                             \
    list->size--;                                                                 \
    return true;                                                                  \
  }                                                                               \
                                                                                  \
  static inline void list_clear_##Type(List_##Type *list) { list->size = 0; }     \
                                                                                  \
  static inline bool list_reserve_##Type(List_##Type *list, int capacity)         \
  {                                                                               \
    if (capacity <= list->capacity)                                               \
      return true;                                                                \
    Type *new_data = realloc(list->data, capacity * sizeof(Type));                \
    if (!new_data)                                                                \
      return false;                                                               \
    list->data = new_data;                                                        \
    list->capacity = capacity;                                                    \
    return true;                                                                  \
  }                                                                               \
                                                                                  \
  static inline bool list_shrink_to_fit_##Type(List_##Type *list)                 \
  {                                                                               \
    if (list->size == list->capacity)                                             \
      return true;                                                                \
    Type *new_data = realloc(list->data, list->size * sizeof(Type));              \
    if (!new_data && list->size > 0)                                              \
      return false;                                                               \
    list->data = new_data;                                                        \
    list->capacity = list->size;                                                  \
    return true;                                                                  \
  }                                                                               \
                                                                                  \
  static inline char *list_to_str_##Type##_custom(                                \
      List_##Type *list, void (*elem_to_str)(Type, char *, size_t))               \
  {                                                                               \
    if (!list)                                                                    \
      return NULL;                                                                \
    size_t bufsize = list->size * BUF_SIZE + 2;                                   \
    char *buffer = malloc(bufsize);                                               \
    if (!buffer)                                                                  \
      return NULL;                                                                \
    strcpy(buffer, "[");                                                          \
    char elem[BUF_SIZE];                                                          \
    for (int i = 0; i < list->size; i++)                                          \
    {                                                                             \
      elem_to_str(list->data[i], elem, sizeof(elem));                             \
      strcat(buffer, elem);                                                       \
      if (i < list->size - 1)                                                     \
        strcat(buffer, ", ");                                                     \
    }                                                                             \
    strcat(buffer, "]");                                                          \
    return buffer;                                                                \
  }                                                                               \
                                                                                  \
  static inline void list_reverse_##Type(List_##Type *list)                       \
  {                                                                               \
    if (list->size < 2)                                                           \
      return;                                                                     \
    for (int i = 0; i < list->size / 2; i++)                                      \
    {                                                                             \
      Type temp = list->data[i];                                                  \
      list->data[i] = list->data[list->size - 1 - i];                             \
      list->data[list->size - 1 - i] = temp;                                      \
    }                                                                             \
  }                                                                               \
                                                                                  \
  static inline bool                                                              \
  list_sort_##Type(                                                               \
      List_##Type *list, int (*comparator)(const Type *a, const Type *b))         \
  {                                                                               \
    if (!list || !list->data || !comparator)                                      \
    {                                                                             \
      return false;                                                               \
    }                                                                             \
    if (list->size < 2)                                                           \
    {                                                                             \
      return true;                                                                \
    }                                                                             \
    qsort(list->data, list->size, sizeof(Type),                                   \
          (int (*)(const void *, const void *))comparator);                       \
    return true;                                                                  \
  }                                                                               \
  static inline bool list_merge_##Type(List_##Type *dest,                         \
                                       const List_##Type *src)                    \
  {                                                                               \
    if (!list_ensure_capacity_##Type(dest, src->size))                            \
      return false;                                                               \
    memcpy(&dest->data[dest->size], src->data, src->size * sizeof(Type));         \
    dest->size += src->size;                                                      \
    return true;                                                                  \
  }                                                                               \
                                                                                  \
  static inline void free_list_##Type(List_##Type *list)                          \
  {                                                                               \
    void (*Dtor_fn)(Type *) = DestructorFunc;                                     \
    if (Dtor_fn)                                                                  \
    {                                                                             \
      for (int i = 0; i < list->size; i++)                                        \
      {                                                                           \
                                                                                  \
        (Dtor_fn)(&list->data[i]);                                                \
      }                                                                           \
    }                                                                             \
    free(list->data);                                                             \
    list->data = NULL;                                                            \
    list->size = 0;                                                               \
    list->capacity = 0;                                                           \
  }

/**
 * @brief Registers the default print function for a specific list type.
 *
 * This macro **defines** the `list_to_str_<Type>_main` function, which is used
 * by convenience macros like `List_to_str` and `List_print`.
 *
 * @param Type The element type for which to register the print function.
 * @param print_fn The function with signature: `void func(Type, char*, size_t)`.
 *
 * @note IMPORTANT: Due to C macro limitations, this macro **must be called exactly once**
 * for a given type in a single translation unit (e.g., a `.c` file). Calling it
 * more than once will result in a compiler error due to redefinition.
 *
 * Example:
 * ```c
 * void int_to_str(int value, char *buffer, size_t size) {
 * snprintf(buffer, size, "%d", value);
 * }
 *
 * CLIP_DEFINE_LIST_TYPE(int);
 * CLIP_REGISTER_LIST_PRINT(int, int_to_str);
 *
 * List(int) xs = List_init(int, 4);
 * List_append(int, &xs, 42);
 * List_print(int, &xs);  // Uses registered function!
 * ```
 */
#define CLIP_REGISTER_LIST_PRINT(Type, print_fn)                             \
  static inline char *list_to_str_##Type##_main(List_##Type *list)           \
  {                                                                          \
    if (!print_fn)                                                           \
    {                                                                        \
      fprintf(stderr, "No print function registered for type " #Type "!\n"); \
      return NULL;                                                           \
    }                                                                        \
    return list_to_str_##Type##_custom(list, print_fn);                      \
  }

/**
 * @def List(Type)
 * @brief Alias to the generated `List_<Type>` struct.
 *
 * Example:
 * ```c
 * List(int) xs = List_init(int, 8);
 * ```
 */
#define List(Type) List_##Type

/**
 * @def List_init(Type, cap)
 * @brief Initialize a list of the given type with the given initial capacity.
 */
#define List_init(Type, cap) init_list_##Type(cap)

/**
 * @def List_init_from_array(Type, arr, n)
 * @brief Initialize a list with elements copied from an existing C array.
 *
 * Example:
 * ```c
 * int values[] = {1, 2, 3, 5};
 * List(int) xs = List_init_from_array(int, values, 4);
 *
 * // or using a compound literal:
 * List(int) ys = List_init_from_array(int, (int[]){10, 20, 30}, 3);
 * ```
 *
 * @param Type The element type (e.g., `int`, `float`).
 * @param arr  Pointer to the source C array.
 * @param n    Number of elements in the source array.
 * @return A fully initialized list containing the copied elements.
 */

#define List_init_from_array(Type, arr, n) init_list_from_array_##Type(arr, n)

/**
 * @def List_init_from_static_array(Type, arr)
 * @brief Initialize a list from a compile-time array.
 *
 * This macro automatically infers the length of a static (stack-allocated)
 * array and calls the generated `init_list_from_array_<Type>` function.
 *
 * @param Type  The element type (e.g., `int`, `float`, `MyStruct`).
 * @param arr   A static array (not a pointer). The macro uses
 *              `sizeof(arr) / sizeof(arr[0])` to compute the element count.
 *
 * @note This will raise a compiler error if `arr` is a pointer,
 *       since the size cannot be determined automatically.
 *
 * Example:
 * ```c
 * int xs[] = {1, 2, 3};
 * List(int) list = List_init_from_static_array(int, xs);
 * ```
 */
#define List_init_from_static_array(Type, arr) \
  init_list_from_array_##Type((arr), (int)(sizeof(arr) / sizeof((arr)[0])))

/**
 * @def List_append(Type, list, val)
 * @brief Append a value to the list (resizing if needed).
 */
#define List_append(Type, list, val) list_append_##Type(list, val)

/**
 * @def List_pop(Type, list, val)
 * @brief Append a value to the list (resizing if needed).
 */
#define List_pop(Type, list, val) list_pop_##Type(list, val)

/**
 * @def List_replace(Type, list, i, val)
 * @brief Replace the element at index `i` with a new value.
 */
#define List_replace(Type, list, i, val) list_replace_##Type(list, i, val)

/**
 * @def List_insert(Type, list, i, val)
 * @brief Insert an element at index `i` with a new value.
 */
#define List_insert(Type, list, i, val) list_insert_##Type(list, i, val)

/**
 * @def List_get(Type, list, index)
 * @brief Safely retrieve an element at the specified index with bounds checking.
 *
 * This function performs bounds checking and will print an error message to stderr
 * if the index is out of bounds, returning a zero-initialized value of the given type.
 *
 * @param Type The element type (e.g., `int`, `float`, `struct MyStruct`).
 * @param list Pointer to the List(Type) structure.
 * @param index The zero-based index of the element to retrieve.
 * @return The element at the specified index, or zero-initialized value if out of bounds.
 *
 * @note This is the safe version. For performance-critical code where bounds are
 *       guaranteed to be valid, consider using List_at() instead.
 *
 * Example:
 * ```c
 * List(int) xs = List_init_from_array(int, (int[]){10, 20, 30}, 3);
 * int value = List_get(int, &xs, 1);  // Returns 20
 * int invalid = List_get(int, &xs, 5);  // Prints error, returns 0
 * ```
 */
#define List_get(Type, list, index) list_get_##Type(list, index)

/**
 * @def List_get_ptr(Type, list, index)
 * @brief Safely retrieve a pointer to an element at the specified index with bounds checking.
 *
 * This function performs bounds checking and will print an error message to stderr
 * if the index is out of bounds, returning NULL. The returned pointer can be used
 * to read or modify the element in place.
 *
 * @param Type The element type (e.g., `int`, `float`, `struct MyStruct`).
 * @param list Pointer to the List(Type) structure.
 * @param index The zero-based index of the element to retrieve.
 * @return Pointer to the element at the specified index, or NULL if out of bounds.
 *
 * @note Always check for NULL before dereferencing the returned pointer.
 *
 * Example:
 * ```c
 * List(int) xs = List_init_from_array(int, (int[]){10, 20, 30}, 3);
 * int* ptr = List_get_ptr(int, &xs, 1);
 * if (ptr != NULL) {
 *     printf("Value: %d\n", *ptr);  // Prints: Value: 20
 *     *ptr = 99;  // Modify the element
 * }
 * ```
 */
#define List_get_ptr(Type, list, index) list_get_ptr_##Type(list, index)

/**
 * @def List_at(Type, list, index)
 * @brief Directly retrieve an element at the specified index without bounds checking.
 *
 * This is the fast, unchecked version that directly accesses the underlying array.
 * No bounds checking is performed, so accessing an invalid index results in
 * undefined behavior.
 *
 * @param Type The element type (e.g., `int`, `float`, `struct MyStruct`).
 * @param list Pointer to the List(Type) structure.
 * @param index The zero-based index of the element to retrieve.
 * @return The element at the specified index.
 *
 * @warning This function does NOT perform bounds checking. Accessing an invalid
 *          index may cause a segmentation fault or return garbage data.
 *          Use List_get() for safer access with bounds checking.
 *
 * @note This function can also be used for assignment: `List_at(int, &xs, 2) = 42;`
 *
 * Example:
 * ```c
 * List(int) xs = List_init_from_array(int, (int[]){10, 20, 30}, 3);
 *
 * // Fast read access (make sure index is valid!)
 * if (xs.size > 1) {
 *     int value = List_at(int, &xs, 1);  // Returns 20
 * }
 *
 * // Direct assignment
 * if (xs.size > 2) {
 *     List_at(int, &xs, 2) = 99;  // Set element to 99
 * }
 * ```
 */
#define List_at(Type, list, index) list_at_##Type(list, index)

/**
 * @def List_at_ptr(Type, list, index)
 * @brief Directly retrieve a pointer to an element at the specified index without bounds checking.
 *
 * This is the fast, unchecked version that directly accesses the underlying array
 * and returns a pointer to the element. No bounds checking is performed.
 *
 * @param Type The element type (e.g., `int`, `float`, `struct MyStruct`).
 * @param list Pointer to the List(Type) structure.
 * @param index The zero-based index of the element to retrieve.
 * @return Pointer to the element at the specified index.
 *
 * @warning This function does NOT perform bounds checking. Accessing an invalid
 *          index may cause a segmentation fault. Use List_get_ptr() for safer
 *          access with bounds checking.
 *
 * Example:
 * ```c
 * List(int) xs = List_init_from_array(int, (int[]){10, 20, 30}, 3);
 *
 * // Fast pointer access (make sure index is valid!)
 * if (xs.size > 1) {
 *     int* ptr = List_at_ptr(int, &xs, 1);
 *     *ptr = 42;  // Modify the element
 * }
 * ```
 */
#define List_at_ptr(Type, list, index) list_at_ptr_##Type(list, index)

/**
 * @def List_remove_at(Type, list, i)
 * @brief Remove the element at index `i` by shifting subsequent elements.
 */
#define List_remove_at(Type, list, i) list_remove_at_##Type(list, i)

/**
 * @def List_clear(Type, list)
 * @brief Reset the list size to 0 without freeing memory.
 */
#define List_clear(Type, list) list_clear_##Type(list)

/**
 * @def List_reserve(Type, list, cap)
 * @brief Ensure the list has at least `cap` capacity.
 */
#define List_reserve(Type, list, cap) list_reserve_##Type(list, cap)

/**
 * @def List_shrink_to_fit(Type, list)
 * @brief Shrink the list's capacity down to its current size.
 */
#define List_shrink_to_fit(Type, list) list_shrink_to_fit_##Type(list)

/**
 * @def List_to_str(Type, list)
 * @brief Convert list contents to a string using the registered
 * print function for this type. The returned string must be freed by the caller.
 *
 * @note You must call `CLIP_REGISTER_LIST_PRINT(Type, fn)` for this type first.
 */
#define List_to_str(Type, list) list_to_str_##Type##_main(list)

/**
 * @def List_to_str_custom(Type, list, fn)
 * @brief Convert list contents to a string using a user-supplied
 *        element-to-string function.
 */
#define List_to_str_custom(Type, list, fn) list_to_str_##Type##_custom(list, fn)

/**
 * @def List_reverse(Type, list)
 * @brief Reverses the given list
 */
#define List_reverse(Type, list) list_reverse_##Type(list)

/**
 * @def List_sort(Type, list, cmp)
 * @brief Sorts the list in-place using the provided comparator function.
 * @return Returns `true` on success, `false` if the list or comparator is NULL.
 */
#define List_sort(Type, list, cmp) list_sort_##Type(list, cmp)

/**
 * @def List_merge(Type, dest, src)
 * @brief Append all elements from one list into another.
 *
 * Example:
 * ```c
 * List(int) a = List_init_from_array(int, (int[]){1, 2, 3}, 3);
 * List(int) b = List_init_from_array(int, (int[]){4, 5}, 2);
 *
 * List_merge(int, &a, &b); // a = [1, 2, 3, 4, 5]
 * ```
 *
 * @param Type The element type (e.g., `int`, `float`).
 * @param dest Pointer to the destination list (will be extended).
 * @param src  Pointer to the source list (contents are copied, unchanged).
 * @return Returns `true` on success, `false` if allocation failed.
 */

#define List_merge(Type, dest, src) list_merge_##Type(dest, src)

/**
 * @def List_free(Type, list)
 * @brief Free all memory held by the list and reset its fields.
 */
#define List_free(Type, list) free_list_##Type(list)

#define List_free_fn(Type) free_list_##Type

/**
 * @def List_print(Type, list)
 * @brief Pretty-print a list directly to stdout using the registered
 * print function. (Does not print a trailing newline)
 *
 * @note You must call `CLIP_REGISTER_LIST_PRINT(Type, fn)` for this type first.
 *
 * Example:
 * ```c
 * CLIP_REGISTER_LIST_PRINT(int, int_to_str);
 * List_print(int, &my_list);
 * ```
 */
#define List_print(Type, list)                \
  do                                          \
  {                                           \
    char *_tmp_str = List_to_str(Type, list); \
    if (_tmp_str)                             \
    {                                         \
      printf("%s", _tmp_str);                 \
      free(_tmp_str);                         \
    }                                         \
  } while (0)

/**
 * @def List_print_custom(Type, list, fn)
 * @brief Pretty-print a list directly to stdout using the given
 * element-to-string function. (Does not print the end-line character)
 *
 * Example:
 * ```c
 * List_print_custom(Student, &classroom, Student_to_str);
 * ```
 */
#define List_print_custom(Type, list, fn)                \
  do                                                     \
  {                                                      \
    char *_tmp_str = List_to_str_custom(Type, list, fn); \
    if (_tmp_str)                                        \
    {                                                    \
      printf("%s", _tmp_str);                            \
      free(_tmp_str);                                    \
    }                                                    \
  } while (0)

/**
 * @def List_println(Type, list)
 * @brief Pretty-print a list directly to stdout using the registered
 * print function, followed by a newline.
 *
 * @note You must call `CLIP_REGISTER_LIST_PRINT(Type, fn)` for this type first.
 */
#define List_println(Type, list) \
  do                             \
  {                              \
    List_print(Type, list);      \
    printf("\n");                \
  } while (0)

/**
 * @def List_println_custom(Type, list, fn)
 * @brief Pretty-print a list directly to stdout using a custom
 * print function, followed by a newline.
 */
#define List_println_custom(Type, list, fn) \
  do                                        \
  {                                         \
    List_print_custom(Type, list, fn);      \
    printf("\n");                           \
  } while (0)

/**
 * @def List_foreach(Type, var, list)
 * @brief Iterates over the list, providing a pointer to the current element.
 *
 * Example:
 * ```c
 * List(int) xs = List_init_from_array(int, (int[]){10, 20, 30}, 3);
 *
 * List_foreach(int, item, &xs) {
 * printf("Value: %d\n", *item); // 'item' is a 'int*'
 * *item += 1; // You can modify the element!
 * }
 * ```
 *
 * @param Type  The element type (e.g., `int`, `float`).
 * @param var   The name of the variable (a **pointer to Type**) to hold the
 * current element.
 * @param list  Pointer to the List(Type) structure (e.g., `&my_list`).
 *
 * @note The iterator stores the size at the start of the loop to avoid problems.
 */
#define List_foreach(Type, var, list)                \
  for (int _i_##var = 0, _size_##var = (list)->size; \
       _i_##var < _size_##var;                       \
       _i_##var++)                                   \
    for (Type *var = &(list)->data[_i_##var]; var != NULL; var = NULL)

#endif /* CLIP_LIST_H */
