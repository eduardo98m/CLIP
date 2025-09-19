
/*
 * @author Eduardo I. Lopez H. - eduardo98m@gmail.com
 * @brief In this file we define a macro that generates a type-safe list
 * (dynamic array) for any given element type in C.
 *
 * Example:
 *   CLIP_DEFINE_LIST_TYPE(int, Int)
 *
 *   List(Int) xs = List_init(Int, 4);
 *   List_append(Int, &xs, 42);
 *   List_free(Int, &xs);
 *
 * The following methods are generated automatically:
 *   - init
 *   - ensure_capacity
 *   - append
 *   - replace
 *   - get
 *   - remove_at
 *   - clear
 *   - reserve
 *   - shrink_to_fit
 *   - to_str
 *   - free
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
 *   - A typedef `List_<Type>` structure containing `data`, `size`, `capacity`.
 *   - A set of inline functions specialized for `<Type>`:
 *       - init_list_<Type>
 *       - list_ensure_capacity_<Type>
 *       - list_append_<Type>
 *       - list_replace_<Type>
 *       - list_get_<Type>
 *       - list_remove_at_<Type>
 *       - list_clear_<Type>
 *       - list_reserve_<Type>
 *       - list_shrink_to_fit_<Type>
 *       - list_to_str_<Type>
 *       - free_list_<Type>
 *
 * @param Type The element type (e.g., `int`, `float`, `struct Foo`).
 *
 * @note Each instantiation of this macro generates a new independent
 *       list type and associated functions.
 */
#define CLIP_DEFINE_LIST_TYPE(Type)                                            \
  typedef struct {                                                             \
    Type *data;                                                                \
    int size;                                                                  \
    int capacity;                                                              \
  } List_##Type;                                                               \
                                                                               \
  static inline List_##Type init_list_##Type(int capacity) {                   \
    List_##Type list;                                                          \
    list.data = malloc(capacity * sizeof(Type));                               \
    if (!list.data) {                                                          \
      fprintf(stderr, "Memory allocation failed!\n");                          \
      exit(EXIT_FAILURE);                                                      \
    }                                                                          \
    list.size = 0;                                                             \
    list.capacity = capacity;                                                  \
    return list;                                                               \
  }                                                                            \
                                                                               \
  static inline bool list_ensure_capacity_##Type(List_##Type *list,            \
                                                 int needed) {                 \
    if (list->size + needed > list->capacity) {                                \
      int new_capacity = list->capacity ? list->capacity * 2 : 1;              \
      while (new_capacity < list->size + needed)                               \
        new_capacity *= 2;                                                     \
      Type *new_data = realloc(list->data, new_capacity * sizeof(Type));       \
      if (!new_data)                                                           \
        return false;                                                          \
      list->data = new_data;                                                   \
      list->capacity = new_capacity;                                           \
    }                                                                          \
    return true;                                                               \
  }                                                                            \
                                                                               \
  static inline bool list_append_##Type(List_##Type *list, Type value) {       \
    if (!list_ensure_capacity_##Type(list, 1))                                 \
      return false;                                                            \
    list->data[list->size++] = value;                                          \
    return true;                                                               \
  }                                                                            \
                                                                               \
  static inline bool list_replace_##Type(List_##Type *list, int index,         \
                                         Type value) {                         \
    if (index < 0 || index >= list->size)                                      \
      return false;                                                            \
    list->data[index] = value;                                                 \
    return true;                                                               \
  }                                                                            \
                                                                               \
  static inline bool list_get_##Type(List_##Type *list, int index,             \
                                     Type *out) {                              \
    if (index < 0 || index >= list->size)                                      \
      return false;                                                            \
    *out = list->data[index];                                                  \
    return true;                                                               \
  }                                                                            \
                                                                               \
  static inline bool list_remove_at_##Type(List_##Type *list, int index) {     \
    if (index < 0 || index >= list->size)                                      \
      return false;                                                            \
    memmove(&list->data[index], &list->data[index + 1],                        \
            (list->size - index - 1) * sizeof(Type));                          \
    list->size--;                                                              \
    return true;                                                               \
  }                                                                            \
                                                                               \
  static inline void list_clear_##Type(List_##Type *list) { list->size = 0; }  \
                                                                               \
  static inline bool list_reserve_##Type(List_##Type *list, int capacity) {    \
    if (capacity <= list->capacity)                                            \
      return true;                                                             \
    Type *new_data = realloc(list->data, capacity * sizeof(Type));             \
    if (!new_data)                                                             \
      return false;                                                            \
    list->data = new_data;                                                     \
    list->capacity = capacity;                                                 \
    return true;                                                               \
  }                                                                            \
                                                                               \
  static inline bool list_shrink_to_fit_##Type(List_##Type *list) {            \
    if (list->size == list->capacity)                                          \
      return true;                                                             \
    Type *new_data = realloc(list->data, list->size * sizeof(Type));           \
    if (!new_data && list->size > 0)                                           \
      return false;                                                            \
    list->data = new_data;                                                     \
    list->capacity = list->size;                                               \
    return true;                                                               \
  }                                                                            \
                                                                               \
  static inline char *list_to_str_##Type(                                      \
      List_##Type *list, void (*elem_to_str)(Type, char *, size_t)) {          \
    size_t bufsize = list->size * 32 + 2; /* rough estimate */                 \
    char *buffer = malloc(bufsize);                                            \
    if (!buffer)                                                               \
      return NULL;                                                             \
    strcpy(buffer, "[");                                                       \
    char elem[64];                                                             \
    for (int i = 0; i < list->size; i++) {                                     \
      elem_to_str(list->data[i], elem, sizeof(elem));                          \
      strcat(buffer, elem);                                                    \
      if (i < list->size - 1)                                                  \
        strcat(buffer, ", ");                                                  \
    }                                                                          \
    strcat(buffer, "]");                                                       \
    return buffer;                                                             \
  }                                                                            \
                                                                               \
  static inline void free_list_##Type(List_##Type *list) {                     \
    free(list->data);                                                          \
    list->data = NULL;                                                         \
    list->size = 0;                                                            \
    list->capacity = 0;                                                        \
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
 * @def List_append(Type, lst, val)
 * @brief Append a value to the list (resizing if needed).
 */
#define List_append(Type, lst, val) list_append_##Type(lst, val)

/**
 * @def List_replace(Type, lst, i, val)
 * @brief Replace the element at index `i` with a new value.
 */
#define List_replace(Type, lst, i, val) list_replace_##Type(lst, i, val)

/**
 * @def List_get(Type, lst, i, out)
 * @brief Retrieve the element at index `i`.  
 * @param out Pointer where the element will be written.
 */
#define List_get(Type, lst, i, out) list_get_##Type(lst, i, out)

/**
 * @def List_remove_at(Type, lst, i)
 * @brief Remove the element at index `i` by shifting subsequent elements.
 */
#define List_remove_at(Type, lst, i) list_remove_at_##Type(lst, i)

/**
 * @def List_clear(Type, lst)
 * @brief Reset the list size to 0 without freeing memory.
 */
#define List_clear(Type, lst) list_clear_##Type(lst)

/**
 * @def List_reserve(Type, lst, cap)
 * @brief Ensure the list has at least `cap` capacity.
 */
#define List_reserve(Type, lst, cap) list_reserve_##Type(lst, cap)

/**
 * @def List_shrink_to_fit(Type, lst)
 * @brief Shrink the list's capacity down to its current size.
 */
#define List_shrink_to_fit(Type, lst) list_shrink_to_fit_##Type(lst)

/**
 * @def List_to_str(Type, lst, fn)
 * @brief Convert list contents to a string using a user-supplied
 *        element-to-string function.
 */
#define List_to_str(Type, lst, fn) list_to_str_##Type(lst, fn)

/**
 * @def List_free(Type, lst)
 * @brief Free all memory held by the list and reset its fields.
 */
#define List_free(Type, lst) free_list_##Type(lst)

#endif /* CLIP_LIST_H */
