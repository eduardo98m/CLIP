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
  CLIP_DEFINE_SET_TYPE_IMPL(__VA_ARGS__, NULL, 256)

/**
 * @brief Defines the Set type with a given buffer size
 */
#define CLIP_DEFINE_SET_TYPE_BUF(Type, CompareFunc, BUF_SIZE) \
  CLIP_DEFINE_SET_TYPE_IMPL(Type, CompareFunc, NULL, BUF)

/**
 * @brief Defines the Set type with a given free function for the type
 */
#define CLIP_DEFINE_SET_TYPE_WITH_FREE(Type, CompareFunc, FREE_FN) \
  CLIP_DEFINE_SET_TYPE_IMPL(Type, CompareFunc, FREE_FN, 256)

/**
 * @brief Defines the Set type with a given free function and a custom buffer size for the print
 */
#define CLIP_DEFINE_SET_TYPE_FULL(Type, CompareFunc, FREE_FN, BUF_SIZE) \
  CLIP_DEFINE_SET_TYPE_IMPL(Type, CompareFunc, FREE_FN, BUF_SIZE)

/**
 * @brief Specialized implementation of `CLIP_DEFINE_SET_TYPE` but allows the
 * user to specify a buffer size for each of the types.
 *
 * @param Type The type
 * @param CompareFunc The comparator function
 * @param FREE_FN Destructor/Free function for the type : Useful for automatic liberating memory
 * @param BUF_SIZE The buffer size
 */
#define CLIP_DEFINE_SET_TYPE_IMPL(Type, CompareFunc, FREE_FN, BUF_SIZE, ...)                           \
                                                                                                       \
  typedef enum                                                                                         \
  {                                                                                                    \
    SET_RED_##Type,                                                                                    \
    SET_BLACK_##Type                                                                                   \
  } SetColor_##Type;                                                                                   \
                                                                                                       \
  typedef struct SetNode_##Type                                                                        \
  {                                                                                                    \
    Type value;                                                                                        \
    struct SetNode_##Type *left, *right, *parent;                                                      \
    SetColor_##Type color;                                                                             \
  } SetNode_##Type;                                                                                    \
                                                                                                       \
  typedef struct                                                                                       \
  {                                                                                                    \
    SetNode_##Type *root;                                                                              \
    int size;                                                                                          \
  } Set_##Type;                                                                                        \
                                                                                                       \
  static inline Set_##Type init_set_##Type()                                                           \
  {                                                                                                    \
    Set_##Type set = {0};                                                                              \
    return set;                                                                                        \
  }                                                                                                    \
                                                                                                       \
  static inline SetNode_##Type *set_node_new_##Type(Type value)                                        \
  {                                                                                                    \
    SetNode_##Type *node = (SetNode_##Type *)malloc(sizeof(SetNode_##Type));                           \
    if (!node)                                                                                         \
    {                                                                                                  \
      fprintf(stderr, "Memory allocation failed!\n");                                                  \
      exit(EXIT_FAILURE);                                                                              \
    }                                                                                                  \
    node->value = value;                                                                               \
    node->left = node->right = node->parent = NULL;                                                    \
    node->color = SET_RED_##Type;                                                                      \
    return node;                                                                                       \
  }                                                                                                    \
                                                                                                       \
  static inline void rotate_left_##Type(Set_##Type *set, SetNode_##Type *x)                            \
  {                                                                                                    \
    SetNode_##Type *y = x->right;                                                                      \
    x->right = y->left;                                                                                \
    if (y->left)                                                                                       \
      y->left->parent = x;                                                                             \
    y->parent = x->parent;                                                                             \
    if (!x->parent)                                                                                    \
      set->root = y;                                                                                   \
    else if (x == x->parent->left)                                                                     \
      x->parent->left = y;                                                                             \
    else                                                                                               \
      x->parent->right = y;                                                                            \
    y->left = x;                                                                                       \
    x->parent = y;                                                                                     \
  }                                                                                                    \
                                                                                                       \
  static inline void rotate_right_##Type(Set_##Type *set, SetNode_##Type *y)                           \
  {                                                                                                    \
    SetNode_##Type *x = y->left;                                                                       \
    y->left = x->right;                                                                                \
    if (x->right)                                                                                      \
      x->right->parent = y;                                                                            \
    x->parent = y->parent;                                                                             \
    if (!y->parent)                                                                                    \
      set->root = x;                                                                                   \
    else if (y == y->parent->left)                                                                     \
      y->parent->left = x;                                                                             \
    else                                                                                               \
      y->parent->right = x;                                                                            \
    x->right = y;                                                                                      \
    y->parent = x;                                                                                     \
  }                                                                                                    \
  static inline void insert_fixup_##Type(Set_##Type *set, SetNode_##Type *z)                           \
  {                                                                                                    \
    while (z->parent && z->parent->color == SET_RED_##Type)                                            \
    {                                                                                                  \
      if (z->parent == z->parent->parent->left)                                                        \
      {                                                                                                \
        SetNode_##Type *y = z->parent->parent->right; /* uncle */                                      \
        if (y && y->color == SET_RED_##Type)                                                           \
        {                                                                                              \
          z->parent->color = SET_BLACK_##Type;                                                         \
          y->color = SET_BLACK_##Type;                                                                 \
          z->parent->parent->color = SET_RED_##Type;                                                   \
          z = z->parent->parent;                                                                       \
        }                                                                                              \
        else                                                                                           \
        {                                                                                              \
          if (z == z->parent->right)                                                                   \
          {                                                                                            \
            z = z->parent;                                                                             \
            rotate_left_##Type(set, z);                                                                \
          }                                                                                            \
          z->parent->color = SET_BLACK_##Type;                                                         \
          z->parent->parent->color = SET_RED_##Type;                                                   \
          rotate_right_##Type(set, z->parent->parent);                                                 \
        }                                                                                              \
      }                                                                                                \
      else                                                                                             \
      {                                              /* mirror case */                                 \
        SetNode_##Type *y = z->parent->parent->left; /* uncle */                                       \
        if (y && y->color == SET_RED_##Type)                                                           \
        {                                                                                              \
          z->parent->color = SET_BLACK_##Type;                                                         \
          y->color = SET_BLACK_##Type;                                                                 \
          z->parent->parent->color = SET_RED_##Type;                                                   \
          z = z->parent->parent;                                                                       \
        }                                                                                              \
        else                                                                                           \
        {                                                                                              \
          if (z == z->parent->left)                                                                    \
          {                                                                                            \
            z = z->parent;                                                                             \
            rotate_right_##Type(set, z);                                                               \
          }                                                                                            \
          z->parent->color = SET_BLACK_##Type;                                                         \
          z->parent->parent->color = SET_RED_##Type;                                                   \
          rotate_left_##Type(set, z->parent->parent);                                                  \
        }                                                                                              \
      }                                                                                                \
    }                                                                                                  \
    set->root->color = SET_BLACK_##Type;                                                               \
  }                                                                                                    \
  static inline void transplant_##Type(Set_##Type *set,                                                \
                                       SetNode_##Type *u,                                              \
                                       SetNode_##Type *v)                                              \
  {                                                                                                    \
    if (!u->parent)                                                                                    \
      set->root = v;                                                                                   \
    else if (u == u->parent->left)                                                                     \
      u->parent->left = v;                                                                             \
    else                                                                                               \
      u->parent->right = v;                                                                            \
                                                                                                       \
    if (v)                                                                                             \
      v->parent = u->parent;                                                                           \
  }                                                                                                    \
  static inline void remove_fixup_##Type(Set_##Type *set, SetNode_##Type *x, SetNode_##Type *x_parent) \
  {                                                                                                    \
    while ((x != set->root) && (!x || x->color == SET_BLACK_##Type))                                   \
    {                                                                                                  \
      if (x == (x_parent ? x_parent->left : NULL))                                                     \
      {                                                                                                \
        SetNode_##Type *w = x_parent ? x_parent->right : NULL;                                         \
        if (w && w->color == SET_RED_##Type)                                                           \
        {                                                                                              \
          w->color = SET_BLACK_##Type;                                                                 \
          x_parent->color = SET_RED_##Type;                                                            \
          rotate_left_##Type(set, x_parent);                                                           \
          w = x_parent->right;                                                                         \
        }                                                                                              \
        if ((!w->left || w->left->color == SET_BLACK_##Type) &&                                        \
            (!w->right || w->right->color == SET_BLACK_##Type))                                        \
        {                                                                                              \
          if (w)                                                                                       \
            w->color = SET_RED_##Type;                                                                 \
          x = x_parent;                                                                                \
          x_parent = x ? x->parent : NULL;                                                             \
        }                                                                                              \
        else                                                                                           \
        {                                                                                              \
          if (!w->right || w->right->color == SET_BLACK_##Type)                                        \
          {                                                                                            \
            if (w->left)                                                                               \
              w->left->color = SET_BLACK_##Type;                                                       \
            if (w)                                                                                     \
              w->color = SET_RED_##Type;                                                               \
            rotate_right_##Type(set, w);                                                               \
            w = x_parent ? x_parent->right : NULL;                                                     \
          }                                                                                            \
          if (w)                                                                                       \
            w->color = x_parent->color;                                                                \
          if (x_parent)                                                                                \
            x_parent->color = SET_BLACK_##Type;                                                        \
          if (w && w->right)                                                                           \
            w->right->color = SET_BLACK_##Type;                                                        \
          rotate_left_##Type(set, x_parent);                                                           \
          x = set->root;                                                                               \
        }                                                                                              \
      }                                                                                                \
      else                                                                                             \
      {                                                                                                \
        SetNode_##Type *w = x_parent ? x_parent->left : NULL;                                          \
        if (w && w->color == SET_RED_##Type)                                                           \
        {                                                                                              \
          w->color = SET_BLACK_##Type;                                                                 \
          x_parent->color = SET_RED_##Type;                                                            \
          rotate_right_##Type(set, x_parent);                                                          \
          w = x_parent->left;                                                                          \
        }                                                                                              \
        if ((!w->right || w->right->color == SET_BLACK_##Type) &&                                      \
            (!w->left || w->left->color == SET_BLACK_##Type))                                          \
        {                                                                                              \
          if (w)                                                                                       \
            w->color = SET_RED_##Type;                                                                 \
          x = x_parent;                                                                                \
          x_parent = x ? x->parent : NULL;                                                             \
        }                                                                                              \
        else                                                                                           \
        {                                                                                              \
          if (!w->left || w->left->color == SET_BLACK_##Type)                                          \
          {                                                                                            \
            if (w->right)                                                                              \
              w->right->color = SET_BLACK_##Type;                                                      \
            if (w)                                                                                     \
              w->color = SET_RED_##Type;                                                               \
            rotate_left_##Type(set, w);                                                                \
            w = x_parent ? x_parent->left : NULL;                                                      \
          }                                                                                            \
          if (w)                                                                                       \
            w->color = x_parent->color;                                                                \
          if (x_parent)                                                                                \
            x_parent->color = SET_BLACK_##Type;                                                        \
          if (w && w->left)                                                                            \
            w->left->color = SET_BLACK_##Type;                                                         \
          rotate_right_##Type(set, x_parent);                                                          \
          x = set->root;                                                                               \
        }                                                                                              \
      }                                                                                                \
    }                                                                                                  \
    if (x)                                                                                             \
      x->color = SET_BLACK_##Type;                                                                     \
  }                                                                                                    \
                                                                                                       \
  static inline bool set_insert_##Type(Set_##Type *set, Type value)                                    \
  {                                                                                                    \
    SetNode_##Type *y = NULL;                                                                          \
    SetNode_##Type *x = set->root;                                                                     \
    while (x)                                                                                          \
    {                                                                                                  \
      int cmp = CompareFunc(&value, &x->value);                                                        \
      if (cmp == 0)                                                                                    \
        return false; /* duplicate */                                                                  \
      y = x;                                                                                           \
      x = (cmp < 0) ? x->left : x->right;                                                              \
    }                                                                                                  \
    SetNode_##Type *z = set_node_new_##Type(value);                                                    \
    z->parent = y;                                                                                     \
    if (!y)                                                                                            \
      set->root = z;                                                                                   \
    else if (CompareFunc(&z->value, &y->value) < 0)                                                    \
      y->left = z;                                                                                     \
    else                                                                                               \
      y->right = z;                                                                                    \
    insert_fixup_##Type(set, z);                                                                       \
    set->size++;                                                                                       \
    return true;                                                                                       \
  }                                                                                                    \
                                                                                                       \
  static inline bool set_contains_node_##Type(SetNode_##Type *node, Type value)                        \
  {                                                                                                    \
    if (!node)                                                                                         \
      return false;                                                                                    \
    int cmp = CompareFunc(&value, &node->value);                                                       \
    if (cmp == 0)                                                                                      \
      return true;                                                                                     \
    else if (cmp < 0)                                                                                  \
      return set_contains_node_##Type(node->left, value);                                              \
    else                                                                                               \
      return set_contains_node_##Type(node->right, value);                                             \
  }                                                                                                    \
                                                                                                       \
  static inline bool set_contains_##Type(Set_##Type *set, Type value)                                  \
  {                                                                                                    \
    return set_contains_node_##Type(set->root, value);                                                 \
  }                                                                                                    \
                                                                                                       \
  static inline SetNode_##Type *set_find_min_node_##Type(SetNode_##Type *node)                         \
  {                                                                                                    \
    while (node && node->left)                                                                         \
      node = node->left;                                                                               \
    return node;                                                                                       \
  }                                                                                                    \
                                                                                                       \
  static inline bool set_remove_##Type(Set_##Type *set, Type value)                                    \
  {                                                                                                    \
    SetNode_##Type *z = set->root;                                                                     \
    while (z)                                                                                          \
    {                                                                                                  \
      int cmp = CompareFunc(&value, &z->value);                                                        \
      if (cmp == 0)                                                                                    \
        break;                                                                                         \
      else if (cmp < 0)                                                                                \
        z = z->left;                                                                                   \
      else                                                                                             \
        z = z->right;                                                                                  \
    }                                                                                                  \
    if (!z)                                                                                            \
      return false;                                                                                    \
                                                                                                       \
    SetNode_##Type *y = z;                                                                             \
    SetNode_##Type *x = NULL;                                                                          \
    SetNode_##Type *x_parent = NULL;                                                                   \
    SetColor_##Type y_original_color = y->color;                                                       \
                                                                                                       \
    if (!z->left)                                                                                      \
    {                                                                                                  \
      x = z->right;                                                                                    \
      x_parent = z->parent;                                                                            \
      transplant_##Type(set, z, z->right);                                                             \
    }                                                                                                  \
    else if (!z->right)                                                                                \
    {                                                                                                  \
      x = z->left;                                                                                     \
      x_parent = z->parent;                                                                            \
      transplant_##Type(set, z, z->left);                                                              \
    }                                                                                                  \
    else                                                                                               \
    {                                                                                                  \
      y = set_find_min_node_##Type(z->right);                                                          \
      y_original_color = y->color;                                                                     \
      x = y->right;                                                                                    \
      if (y->parent == z)                                                                              \
      {                                                                                                \
        if (x)                                                                                         \
          x->parent = y;                                                                               \
        x_parent = y;                                                                                  \
      }                                                                                                \
      else                                                                                             \
      {                                                                                                \
        transplant_##Type(set, y, y->right);                                                           \
        y->right = z->right;                                                                           \
        if (y->right)                                                                                  \
          y->right->parent = y;                                                                        \
        x_parent = y->parent;                                                                          \
      }                                                                                                \
      transplant_##Type(set, z, y);                                                                    \
      y->left = z->left;                                                                               \
      if (y->left)                                                                                     \
        y->left->parent = y;                                                                           \
      y->color = z->color;                                                                             \
    }                                                                                                  \
                                                                                                       \
    free(z);                                                                                           \
    set->size--;                                                                                       \
                                                                                                       \
    if (y_original_color == SET_BLACK_##Type)                                                          \
      remove_fixup_##Type(set, x, x_parent);                                                           \
                                                                                                       \
    return true;                                                                                       \
  }                                                                                                    \
                                                                                                       \
  static inline int                                                                                    \
  set_size_##Type(Set_##Type *set)                                                                     \
  {                                                                                                    \
    return set->size;                                                                                  \
  }                                                                                                    \
                                                                                                       \
  static inline bool set_empty_##Type(Set_##Type *set)                                                 \
  {                                                                                                    \
    return set->size == 0;                                                                             \
  }                                                                                                    \
                                                                                                       \
  static inline void set_clear_node_##Type(SetNode_##Type *node)                                       \
  {                                                                                                    \
    if (!node)                                                                                         \
      return;                                                                                          \
    set_clear_node_##Type(node->left);                                                                 \
    set_clear_node_##Type(node->right);                                                                \
    void (*Dtor_fn)(Type *) = FREE_FN;                                                                 \
    if (Dtor_fn)                                                                                       \
    {                                                                                                  \
      (Dtor_fn)(&node->value);                                                                         \
    }                                                                                                  \
    free(node);                                                                                        \
  }                                                                                                    \
                                                                                                       \
  static inline void set_clear_##Type(Set_##Type *set)                                                 \
  {                                                                                                    \
    set_clear_node_##Type(set->root);                                                                  \
    set->root = NULL;                                                                                  \
    set->size = 0;                                                                                     \
  }                                                                                                    \
  /**                                                                                                  \
   * @brief Helper function to recursively insert all nodes from one set into another.                 \
   */                                                                                                  \
  static inline void set_join_recursive_##Type(Set_##Type *dest, SetNode_##Type *node)                 \
  {                                                                                                    \
    if (!node)                                                                                         \
      return;                                                                                          \
    set_join_recursive_##Type(dest, node->left);                                                       \
    set_join_recursive_##Type(dest, node->right);                                                      \
    if (set_insert_##Type(dest, node->value))                                                          \
    {                                                                                                  \
      node->value = (Type){0}; /* We nullify the values form the source node (transfer ownership) */   \
    }                                                                                                  \
  }                                                                                                    \
                                                                                                       \
  /**                                                                                                  \
   * @def Set_join(Type, dest, src)                                                                    \
   * @brief Merge all elements from source set into destination set.                                   \
   * After this operation, source set will be cleared.                                                 \
   * Elements that already exist in dest will not be duplicated.                                       \
   * @return The number of elements successfully added from src to dest.                               \
   */                                                                                                  \
  static inline int set_join_##Type(Set_##Type *dest, Set_##Type *src)                                 \
  {                                                                                                    \
    if (!src || !dest || set_empty_##Type(src))                                                        \
      return 0;                                                                                        \
                                                                                                       \
    int initial_size = set_size_##Type(dest);                                                          \
    set_join_recursive_##Type(dest, src->root);                                                        \
    int final_size = set_size_##Type(dest);                                                            \
    set_clear_##Type(src);                                                                             \
    return final_size - initial_size;                                                                  \
  }                                                                                                    \
                                                                                                       \
  static inline void free_set_##Type(Set_##Type *set)                                                  \
  {                                                                                                    \
    set_clear_##Type(set);                                                                             \
  }                                                                                                    \
                                                                                                       \
  static inline void set_to_str_node_##Type(SetNode_##Type *node, char *buf,                           \
                                            size_t size,                                               \
                                            void (*elem_to_str)(Type, char *, size_t),                 \
                                            bool *is_first)                                            \
  {                                                                                                    \
    if (!node)                                                                                         \
      return;                                                                                          \
    set_to_str_node_##Type(node->left, buf, size, elem_to_str, is_first);                              \
    if (!*is_first)                                                                                    \
    {                                                                                                  \
      size_t current_len = strlen(buf);                                                                \
      if (current_len + 2 < size)                                                                      \
        strcat(buf, ", ");                                                                             \
    }                                                                                                  \
    size_t current_len = strlen(buf);                                                                  \
    if (current_len < size)                                                                            \
      elem_to_str(node->value, buf + current_len, size - current_len);                                 \
    *is_first = false;                                                                                 \
    set_to_str_node_##Type(node->right, buf, size, elem_to_str, is_first);                             \
  }                                                                                                    \
                                                                                                       \
  static inline char *set_to_str_##Type##_custom(Set_##Type *set,                                      \
                                                 void (*elem_to_str)(Type, char *, size_t))            \
  {                                                                                                    \
    if (!set)                                                                                          \
      return NULL;                                                                                     \
    size_t bufsize = set->size * BUF_SIZE + 16;                                                        \
    char *buffer = malloc(bufsize);                                                                    \
    if (!buffer)                                                                                       \
      return NULL;                                                                                     \
    strcpy(buffer, "{");                                                                               \
    bool is_first = true;                                                                              \
    set_to_str_node_##Type(set->root, buffer, bufsize, elem_to_str, &is_first);                        \
    size_t current_len = strlen(buffer);                                                               \
    if (current_len + 1 < bufsize)                                                                     \
      strcat(buffer, "}");                                                                             \
    return buffer;                                                                                     \
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
  {                                                                          \
    if (!print_fn)                                                           \
    {                                                                        \
      fprintf(stderr, "No print function registered for type " #Type "!\n"); \
      return NULL;                                                           \
    }                                                                        \
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
 * @def Set_join(Type, dest, src)
 * @brief Merge all elements from source set into destination set.
 * After this operation, source set will be cleared.
 * Elements that already exist in dest will not be duplicated.
 * @return The number of elements successfully added from src to dest.
 */
#define Set_join(Type, dest, src) set_join_##Type(dest, src)

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