/*
 * @author Based on Eduardo I. Lopez H. - eduardo98m@gmail.com
 * @brief In this file we define a series of macros that generates a **type-safe** set
 * (binary search tree) for any given element type in C.
 *
 * Example:
 * // Define a comparator function
 * int int_compare(const int* a, const int* b) {
 *     return (*a < *b) ? -1 : (*a > *b) ? 1 : 0;
 * }
 * 
 * CLIP_DEFINE_SET_TYPE(int, int_compare)
 *
 * Set(int) xs = Set_init(int);
 * Set_insert(int, &xs, 42);
 * Set_free(int, &xs);
 *
 * The following methods are generated automatically:
 * - init
 * - insert
 * - contains
 * - remove
 * - clear
 * - size
 * - to_str_custom (takes a user-supplied function)
 * - to_str (requires registration via CLIP_REGISTER_SET_PRINT)
 * - free
 */
#ifndef CLIP_SET_H
#define CLIP_SET_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Define a type-safe, tree-based ordered set for a given type.
 *
 * This macro generates:
 * - A typedef `Set_<Type>` structure containing root node and size.
 * - A set of **static inline functions** specialized for `<Type>`:
 *
 * Requirements:
 * - User provides a comparator function:
 *      int cmp(const Type* a, const Type* b)
 *   Returns <0 if a<b, 0 if a==b, >0 if a>b
 *
 * @param Type The element type (e.g., `int`, `float`, `struct Foo`).
 * @param CompareFunc The comparator function for the given type.
 * @param BUF_SIZE Optional: Buffer size allocated per element for string conversion. Default is 256.
 */
#define CLIP_DEFINE_SET_TYPE(...) \
  CLIP_DEFINE_SET_TYPE_IMPL(__VA_ARGS__, 256)

/**
 * @brief Specialized implementation of `CLIP_DEFINE_SET_TYPE` but allows the
 * user to specify a buffer size for each of the types.
 *
 * @param Type The type
 * @param CompareFunc The comparator function
 * @param BUF_SIZE The buffer size
 */
#define CLIP_DEFINE_SET_TYPE_IMPL(Type, CompareFunc, BUF_SIZE, ...)                    \
                                                                                       \
  typedef struct SetNode_##Type                                                        \
  {                                                                                    \
    Type value;                                                                        \
    struct SetNode_##Type *left;                                                       \
    struct SetNode_##Type *right;                                                      \
  } SetNode_##Type;                                                                    \
                                                                                       \
  typedef struct                                                                       \
  {                                                                                    \
    SetNode_##Type *root;                                                              \
    int size;                                                                          \
  } Set_##Type;                                                                        \
                                                                                       \
  static inline Set_##Type init_set_##Type()                                           \
  {                                                                                    \
    Set_##Type set = {0};                                                              \
    return set;                                                                        \
  }                                                                                    \
                                                                                       \
  static inline SetNode_##Type *set_node_new_##Type(Type value)                       \
  {                                                                                    \
    SetNode_##Type *node = (SetNode_##Type *)malloc(sizeof(SetNode_##Type));          \
    if (!node)                                                                         \
    {                                                                                  \
      fprintf(stderr, "Memory allocation failed!\n");                                 \
      exit(EXIT_FAILURE);                                                              \
    }                                                                                  \
    node->value = value;                                                               \
    node->left = NULL;                                                                 \
    node->right = NULL;                                                                \
    return node;                                                                       \
  }                                                                                    \
                                                                                       \
  static inline bool set_insert_node_##Type(SetNode_##Type **node, Type value,        \
                                            bool *was_inserted)                        \
  {                                                                                    \
    if (!*node)                                                                        \
    {                                                                                  \
      *node = set_node_new_##Type(value);                                             \
      *was_inserted = true;                                                            \
      return true;                                                                     \
    }                                                                                  \
    int cmp = CompareFunc(&value, &(*node)->value);                                    \
    if (cmp == 0)                                                                      \
    {                                                                                  \
      *was_inserted = false;                                                           \
      return false;                                                                    \
    }                                                                                  \
    else if (cmp < 0)                                                                  \
    {                                                                                  \
      return set_insert_node_##Type(&(*node)->left, value, was_inserted);             \
    }                                                                                  \
    else                                                                               \
    {                                                                                  \
      return set_insert_node_##Type(&(*node)->right, value, was_inserted);            \
    }                                                                                  \
  }                                                                                    \
                                                                                       \
  static inline bool set_insert_##Type(Set_##Type *set, Type value)                   \
  {                                                                                    \
    bool was_inserted = false;                                                         \
    bool success = set_insert_node_##Type(&set->root, value, &was_inserted);          \
    if (was_inserted)                                                                  \
      set->size++;                                                                     \
    return success;                                                                    \
  }                                                                                    \
                                                                                       \
  static inline bool set_contains_node_##Type(SetNode_##Type *node, Type value)       \
  {                                                                                    \
    if (!node)                                                                         \
      return false;                                                                    \
    int cmp = CompareFunc(&value, &node->value);                                       \
    if (cmp == 0)                                                                      \
      return true;                                                                     \
    else if (cmp < 0)                                                                  \
      return set_contains_node_##Type(node->left, value);                             \
    else                                                                               \
      return set_contains_node_##Type(node->right, value);                            \
  }                                                                                    \
                                                                                       \
  static inline bool set_contains_##Type(Set_##Type *set, Type value)                 \
  {                                                                                    \
    return set_contains_node_##Type(set->root, value);                                \
  }                                                                                    \
                                                                                       \
  static inline SetNode_##Type *set_find_min_node_##Type(SetNode_##Type *node)        \
  {                                                                                    \
    while (node && node->left)                                                         \
      node = node->left;                                                               \
    return node;                                                                       \
  }                                                                                    \
                                                                                       \
  static inline SetNode_##Type *set_remove_node_##Type(SetNode_##Type *node,          \
                                                       Type value, bool *was_removed)  \
  {                                                                                    \
    if (!node)                                                                         \
    {                                                                                  \
      *was_removed = false;                                                            \
      return NULL;                                                                     \
    }                                                                                  \
                                                                                       \
    int cmp = CompareFunc(&value, &node->value);                                       \
    if (cmp < 0)                                                                       \
    {                                                                                  \
      node->left = set_remove_node_##Type(node->left, value, was_removed);            \
    }                                                                                  \
    else if (cmp > 0)                                                                  \
    {                                                                                  \
      node->right = set_remove_node_##Type(node->right, value, was_removed);          \
    }                                                                                  \
    else                                                                               \
    {                                                                                  \
      *was_removed = true;                                                             \
      if (!node->left)                                                                 \
      {                                                                                \
        SetNode_##Type *temp = node->right;                                            \
        free(node);                                                                    \
        return temp;                                                                   \
      }                                                                                \
      else if (!node->right)                                                           \
      {                                                                                \
        SetNode_##Type *temp = node->left;                                             \
        free(node);                                                                    \
        return temp;                                                                   \
      }                                                                                \
      SetNode_##Type *temp = set_find_min_node_##Type(node->right);                   \
      node->value = temp->value;                                                       \
      bool dummy = false;                                                              \
      node->right = set_remove_node_##Type(node->right, temp->value, &dummy);         \
    }                                                                                  \
    return node;                                                                       \
  }                                                                                    \
                                                                                       \
  static inline bool set_remove_##Type(Set_##Type *set, Type value)                   \
  {                                                                                    \
    bool was_removed = false;                                                          \
    set->root = set_remove_node_##Type(set->root, value, &was_removed);               \
    if (was_removed)                                                                   \
      set->size--;                                                                     \
    return was_removed;                                                                \
  }                                                                                    \
                                                                                       \
  static inline int set_size_##Type(Set_##Type *set)                                  \
  {                                                                                    \
    return set->size;                                                                  \
  }                                                                                    \
                                                                                       \
  static inline bool set_empty_##Type(Set_##Type *set)                                \
  {                                                                                    \
    return set->size == 0;                                                             \
  }                                                                                    \
                                                                                       \
  static inline void set_clear_node_##Type(SetNode_##Type *node)                      \
  {                                                                                    \
    if (!node)                                                                         \
      return;                                                                          \
    set_clear_node_##Type(node->left);                                                \
    set_clear_node_##Type(node->right);                                               \
    free(node);                                                                        \
  }                                                                                    \
                                                                                       \
  static inline void set_clear_##Type(Set_##Type *set)                                \
  {                                                                                    \
    set_clear_node_##Type(set->root);                                                 \
    set->root = NULL;                                                                  \
    set->size = 0;                                                                     \
  }                                                                                    \
                                                                                       \
  static inline void free_set_##Type(Set_##Type *set)                                 \
  {                                                                                    \
    set_clear_##Type(set);                                                             \
  }                                                                                    \
                                                                                       \
  static inline void set_to_str_node_##Type(SetNode_##Type *node, char *buf,          \
                                            size_t size,                              \
                                            void (*elem_to_str)(Type, char *, size_t), \
                                            bool *is_first)                            \
  {                                                                                    \
    if (!node)                                                                         \
      return;                                                                          \
    set_to_str_node_##Type(node->left, buf, size, elem_to_str, is_first);            \
    if (!*is_first)                                                                    \
    {                                                                                  \
      size_t current_len = strlen(buf);                                                \
      if (current_len + 2 < size)                                                      \
        strcat(buf, ", ");                                                             \
    }                                                                                  \
    size_t current_len = strlen(buf);                                                  \
    if (current_len < size)                                                            \
      elem_to_str(node->value, buf + current_len, size - current_len);                \
    *is_first = false;                                                                 \
    set_to_str_node_##Type(node->right, buf, size, elem_to_str, is_first);           \
  }                                                                                    \
                                                                                       \
  static inline char *set_to_str_##Type##_custom(Set_##Type *set,                     \
                                                 void (*elem_to_str)(Type, char *, size_t)) \
  {                                                                                    \
    if (!set)                                                                          \
      return NULL;                                                                     \
    size_t bufsize = set->size * BUF_SIZE + 16;                                       \
    char *buffer = malloc(bufsize);                                                    \
    if (!buffer)                                                                       \
      return NULL;                                                                     \
    strcpy(buffer, "{");                                                               \
    bool is_first = true;                                                              \
    set_to_str_node_##Type(set->root, buffer, bufsize, elem_to_str, &is_first);      \
    size_t current_len = strlen(buffer);                                               \
    if (current_len + 1 < bufsize)                                                     \
      strcat(buffer, "}");                                                             \
    return buffer;                                                                     \
  }

/**
 * @brief Registers the default print function for a specific set type.
 *
 * This macro **defines** the `set_to_str_<Type>_main` function, which is used
 * by convenience macros like `Set_to_str` and `Set_print`.
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
 *     snprintf(buffer, size, "%d", value);
 * }
 *
 * CLIP_DEFINE_SET_TYPE(int, int_compare);
 * CLIP_REGISTER_SET_PRINT(int, int_to_str);
 *
 * Set(int) xs = Set_init(int);
 * Set_insert(int, &xs, 42);
 * Set_print(int, &xs);  // Uses registered function!
 * ```
 */
#define CLIP_REGISTER_SET_PRINT(Type, print_fn)                              \
  static inline char *set_to_str_##Type##_main(Set_##Type *set)              \
  {                                                                           \
    if (!print_fn)                                                            \
    {                                                                         \
      fprintf(stderr, "No print function registered for type " #Type "!\n"); \
      return NULL;                                                            \
    }                                                                         \
    return set_to_str_##Type##_custom(set, print_fn);                        \
  }

/**
 * @def Set(Type)
 * @brief Alias to the generated `Set_<Type>` struct.
 *
 * Example:
 * ```c
 * Set(int) xs = Set_init(int);
 * ```
 */
#define Set(Type) Set_##Type

/**
 * @def Set_init(Type)
 * @brief Initialize an empty set of the given type.
 */
#define Set_init(Type) init_set_##Type()

/**
 * @def Set_insert(Type, set, val)
 * @brief Insert a value into the set.
 * @return Returns `true` if the value was inserted, `false` if it already existed.
 */
#define Set_insert(Type, set, val) set_insert_##Type(set, val)

/**
 * @def Set_contains(Type, set, val)
 * @brief Check if a value exists in the set.
 * @return Returns `true` if the value exists, `false` otherwise.
 */
#define Set_contains(Type, set, val) set_contains_##Type(set, val)

/**
 * @def Set_remove(Type, set, val)
 * @brief Remove a value from the set.
 * @return Returns `true` if the value was removed, `false` if it didn't exist.
 */
#define Set_remove(Type, set, val) set_remove_##Type(set, val)

/**
 * @def Set_size(Type, set)
 * @brief Get the number of elements in the set.
 */
#define Set_size(Type, set) set_size_##Type(set)

/**
 * @def Set_empty(Type, set)
 * @brief Check if the set is empty.
 * @return Returns `true` if the set is empty, `false` otherwise.
 */
#define Set_empty(Type, set) set_empty_##Type(set)

/**
 * @def Set_clear(Type, set)
 * @brief Remove all elements from the set and free their memory.
 */
#define Set_clear(Type, set) set_clear_##Type(set)

/**
 * @def Set_free(Type, set)
 * @brief Free all memory held by the set and reset its fields.
 */
#define Set_free(Type, set) free_set_##Type(set)

/**
 * @def Set_to_str(Type, set)
 * @brief Convert set contents to a string using the registered
 * print function for this type. The returned string must be freed by the caller.
 *
 * @note You must call `CLIP_REGISTER_SET_PRINT(Type, fn)` for this type first.
 */
#define Set_to_str(Type, set) set_to_str_##Type##_main(set)

/**
 * @def Set_to_str_custom(Type, set, fn)
 * @brief Convert set contents to a string using a user-supplied
 *        element-to-string function.
 */
#define Set_to_str_custom(Type, set, fn) set_to_str_##Type##_custom(set, fn)

/**
 * @def Set_print(Type, set)
 * @brief Pretty-print a set directly to stdout using the registered
 * print function. (Does not print a trailing newline)
 *
 * @note You must call `CLIP_REGISTER_SET_PRINT(Type, fn)` for this type first.
 */
#define Set_print(Type, set)                \
  do                                        \
  {                                         \
    char *_tmp_str = Set_to_str(Type, set); \
    if (_tmp_str)                           \
    {                                       \
      printf("%s", _tmp_str);               \
      free(_tmp_str);                       \
    }                                       \
  } while (0)

/**
 * @def Set_print_custom(Type, set, fn)
 * @brief Pretty-print a set directly to stdout using the given
 * element-to-string function. (Does not print the end-line character)
 */
#define Set_print_custom(Type, set, fn)                \
  do                                                   \
  {                                                    \
    char *_tmp_str = Set_to_str_custom(Type, set, fn); \
    if (_tmp_str)                                      \
    {                                                  \
      printf("%s", _tmp_str);                          \
      free(_tmp_str);                                  \
    }                                                  \
  } while (0)

/**
 * @def Set_println(Type, set)
 * @brief Pretty-print a set directly to stdout using the registered
 * print function, followed by a newline.
 *
 * @note You must call `CLIP_REGISTER_SET_PRINT(Type, fn)` for this type first.
 */
#define Set_println(Type, set) \
  do                           \
  {                            \
    Set_print(Type, set);      \
    printf("\n");              \
  } while (0)

/**
 * @def Set_println_custom(Type, set, fn)
 * @brief Pretty-print a set directly to stdout using a custom
 * print function, followed by a newline.
 */
#define Set_println_custom(Type, set, fn) \
  do                                      \
  {                                       \
    Set_print_custom(Type, set, fn);      \
    printf("\n");                         \
  } while (0)

#endif /* CLIP_SET_H */