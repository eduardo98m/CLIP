/*
 * @author Based on Eduardo I. Lopez H. - eduardo98m@gmail.com
 * @brief In this file we define a series of macros that generates a **type-safe** map
 * (red-black tree) for any given key-value pair types in C.
 *
 * Example:
 * // Define a comparator function for keys
 * int string_compare(const char* const* a, const char* const* b) {
 *     return strcmp(*a, *b);
 * }
 *
 * CLIP_DEFINE_MAP_TYPE(char*, int, string_compare)
 *
 * Map(char*, int) map = Map_init(char*, int);
 * Map_insert(char*, int, &map, "hello", 42);
 * Map_free(char*, int, &map);
 *
 * The following methods are generated automatically:
 * - init
 * - insert
 * - get
 * - contains_key
 * - remove
 * - clear
 * - size
 * - to_str_custom (takes user-supplied key and value conversion functions)
 * - to_str (requires registration via CLIP_REGISTER_MAP_PRINT)
 * - free
 * - for_each (lets you apply a function over the map values iterator like)
 */
#ifndef CLIP_MAP_H
#define CLIP_MAP_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Define a type-safe, tree-based ordered map for given key-value types.
 *
 * This macro generates:
 * - A typedef `Map_<KeyType>_<ValueType>` structure containing root node and size.
 * - A set of **static inline functions** specialized for `<KeyType, ValueType>`:
 *
 * Requirements:
 * - User provides a comparator function for keys:
 *      int cmp(const KeyType* a, const KeyType* b)
 *   Returns <0 if a<b, 0 if a==b, >0 if a>b
 *
 * @param KeyType The key type (e.g., `int`, `char*`, `struct Foo`).
 * @param ValueType The value type (e.g., `int`, `char*`, `struct Bar`).
 * @param CompareFunc The comparator function for the key type.
 * @param BUF_SIZE Optional: Buffer size allocated per element for string conversion. Default is 256.
 */
#define CLIP_DEFINE_MAP_TYPE(...) \
  CLIP_DEFINE_MAP_TYPE_IMPL(__VA_ARGS__, 256)

/**
 * @brief Specialized implementation of `CLIP_DEFINE_MAP_TYPE` but allows the
 * user to specify a buffer size for each of the types.
 *
 * @param KeyType The key type
 * @param ValueType The value type
 * @param CompareFunc The comparator function for keys
 * @param BUF_SIZE The buffer size
 */
#define CLIP_DEFINE_MAP_TYPE_IMPL(KeyType, ValueType, CompareFunc, BUF_SIZE, ...)                                                                                              \
                                                                                                                                                                               \
  typedef enum                                                                                                                                                                 \
  {                                                                                                                                                                            \
    MAP_RED_##KeyType##_##ValueType,                                                                                                                                           \
    MAP_BLACK_##KeyType##_##ValueType                                                                                                                                          \
  } MapColor_##KeyType##_##ValueType;                                                                                                                                          \
                                                                                                                                                                               \
  typedef struct MapNode_##KeyType##_##ValueType                                                                                                                               \
  {                                                                                                                                                                            \
    KeyType key;                                                                                                                                                               \
    ValueType value;                                                                                                                                                           \
    struct MapNode_##KeyType##_##ValueType *left, *right, *parent;                                                                                                             \
    MapColor_##KeyType##_##ValueType color;                                                                                                                                    \
  } MapNode_##KeyType##_##ValueType;                                                                                                                                           \
                                                                                                                                                                               \
  typedef struct                                                                                                                                                               \
  {                                                                                                                                                                            \
    MapNode_##KeyType##_##ValueType *root;                                                                                                                                     \
    int size;                                                                                                                                                                  \
  } Map_##KeyType##_##ValueType;                                                                                                                                               \
                                                                                                                                                                               \
  static inline Map_##KeyType##_##ValueType init_map_##KeyType##_##ValueType()                                                                                                 \
  {                                                                                                                                                                            \
    Map_##KeyType##_##ValueType map = {0};                                                                                                                                     \
    return map;                                                                                                                                                                \
  }                                                                                                                                                                            \
                                                                                                                                                                               \
  static inline MapNode_##KeyType##_##ValueType *map_node_new_##KeyType##_##ValueType(KeyType key, ValueType value)                                                            \
  {                                                                                                                                                                            \
    MapNode_##KeyType##_##ValueType *node = (MapNode_##KeyType##_##ValueType *)malloc(sizeof(MapNode_##KeyType##_##ValueType));                                                \
    if (!node)                                                                                                                                                                 \
    {                                                                                                                                                                          \
      fprintf(stderr, "Memory allocation failed!\n");                                                                                                                          \
      exit(EXIT_FAILURE);                                                                                                                                                      \
    }                                                                                                                                                                          \
    node->key = key;                                                                                                                                                           \
    node->value = value;                                                                                                                                                       \
    node->left = node->right = node->parent = NULL;                                                                                                                            \
    node->color = MAP_RED_##KeyType##_##ValueType;                                                                                                                             \
    return node;                                                                                                                                                               \
  }                                                                                                                                                                            \
                                                                                                                                                                               \
  static inline void map_rotate_left_##KeyType##_##ValueType(Map_##KeyType##_##ValueType *map, MapNode_##KeyType##_##ValueType *x)                                             \
  {                                                                                                                                                                            \
    MapNode_##KeyType##_##ValueType *y = x->right;                                                                                                                             \
    x->right = y->left;                                                                                                                                                        \
    if (y->left)                                                                                                                                                               \
      y->left->parent = x;                                                                                                                                                     \
    y->parent = x->parent;                                                                                                                                                     \
    if (!x->parent)                                                                                                                                                            \
      map->root = y;                                                                                                                                                           \
    else if (x == x->parent->left)                                                                                                                                             \
      x->parent->left = y;                                                                                                                                                     \
    else                                                                                                                                                                       \
      x->parent->right = y;                                                                                                                                                    \
    y->left = x;                                                                                                                                                               \
    x->parent = y;                                                                                                                                                             \
  }                                                                                                                                                                            \
                                                                                                                                                                               \
  static inline void map_rotate_right_##KeyType##_##ValueType(Map_##KeyType##_##ValueType *map, MapNode_##KeyType##_##ValueType *y)                                            \
  {                                                                                                                                                                            \
    MapNode_##KeyType##_##ValueType *x = y->left;                                                                                                                              \
    y->left = x->right;                                                                                                                                                        \
    if (x->right)                                                                                                                                                              \
      x->right->parent = y;                                                                                                                                                    \
    x->parent = y->parent;                                                                                                                                                     \
    if (!y->parent)                                                                                                                                                            \
      map->root = x;                                                                                                                                                           \
    else if (y == y->parent->left)                                                                                                                                             \
      y->parent->left = x;                                                                                                                                                     \
    else                                                                                                                                                                       \
      y->parent->right = x;                                                                                                                                                    \
    x->right = y;                                                                                                                                                              \
    y->parent = x;                                                                                                                                                             \
  }                                                                                                                                                                            \
                                                                                                                                                                               \
  static inline void map_insert_fixup_##KeyType##_##ValueType(Map_##KeyType##_##ValueType *map, MapNode_##KeyType##_##ValueType *z)                                            \
  {                                                                                                                                                                            \
    while (z->parent && z->parent->color == MAP_RED_##KeyType##_##ValueType)                                                                                                   \
    {                                                                                                                                                                          \
      if (z->parent == z->parent->parent->left)                                                                                                                                \
      {                                                                                                                                                                        \
        MapNode_##KeyType##_##ValueType *y = z->parent->parent->right; /* uncle */                                                                                             \
        if (y && y->color == MAP_RED_##KeyType##_##ValueType)                                                                                                                  \
        {                                                                                                                                                                      \
          z->parent->color = MAP_BLACK_##KeyType##_##ValueType;                                                                                                                \
          y->color = MAP_BLACK_##KeyType##_##ValueType;                                                                                                                        \
          z->parent->parent->color = MAP_RED_##KeyType##_##ValueType;                                                                                                          \
          z = z->parent->parent;                                                                                                                                               \
        }                                                                                                                                                                      \
        else                                                                                                                                                                   \
        {                                                                                                                                                                      \
          if (z == z->parent->right)                                                                                                                                           \
          {                                                                                                                                                                    \
            z = z->parent;                                                                                                                                                     \
            map_rotate_left_##KeyType##_##ValueType(map, z);                                                                                                                   \
          }                                                                                                                                                                    \
          z->parent->color = MAP_BLACK_##KeyType##_##ValueType;                                                                                                                \
          z->parent->parent->color = MAP_RED_##KeyType##_##ValueType;                                                                                                          \
          map_rotate_right_##KeyType##_##ValueType(map, z->parent->parent);                                                                                                    \
        }                                                                                                                                                                      \
      }                                                                                                                                                                        \
      else                                                                                                                                                                     \
      {                                                               /* mirror case */                                                                                        \
        MapNode_##KeyType##_##ValueType *y = z->parent->parent->left; /* uncle */                                                                                              \
        if (y && y->color == MAP_RED_##KeyType##_##ValueType)                                                                                                                  \
        {                                                                                                                                                                      \
          z->parent->color = MAP_BLACK_##KeyType##_##ValueType;                                                                                                                \
          y->color = MAP_BLACK_##KeyType##_##ValueType;                                                                                                                        \
          z->parent->parent->color = MAP_RED_##KeyType##_##ValueType;                                                                                                          \
          z = z->parent->parent;                                                                                                                                               \
        }                                                                                                                                                                      \
        else                                                                                                                                                                   \
        {                                                                                                                                                                      \
          if (z == z->parent->left)                                                                                                                                            \
          {                                                                                                                                                                    \
            z = z->parent;                                                                                                                                                     \
            map_rotate_right_##KeyType##_##ValueType(map, z);                                                                                                                  \
          }                                                                                                                                                                    \
          z->parent->color = MAP_BLACK_##KeyType##_##ValueType;                                                                                                                \
          z->parent->parent->color = MAP_RED_##KeyType##_##ValueType;                                                                                                          \
          map_rotate_left_##KeyType##_##ValueType(map, z->parent->parent);                                                                                                     \
        }                                                                                                                                                                      \
      }                                                                                                                                                                        \
    }                                                                                                                                                                          \
    map->root->color = MAP_BLACK_##KeyType##_##ValueType;                                                                                                                      \
  }                                                                                                                                                                            \
                                                                                                                                                                               \
  static inline void map_transplant_##KeyType##_##ValueType(Map_##KeyType##_##ValueType *map,                                                                                  \
                                                            MapNode_##KeyType##_##ValueType *u,                                                                                \
                                                            MapNode_##KeyType##_##ValueType *v)                                                                                \
  {                                                                                                                                                                            \
    if (!u->parent)                                                                                                                                                            \
      map->root = v;                                                                                                                                                           \
    else if (u == u->parent->left)                                                                                                                                             \
      u->parent->left = v;                                                                                                                                                     \
    else                                                                                                                                                                       \
      u->parent->right = v;                                                                                                                                                    \
                                                                                                                                                                               \
    if (v)                                                                                                                                                                     \
      v->parent = u->parent;                                                                                                                                                   \
  }                                                                                                                                                                            \
                                                                                                                                                                               \
  static inline void map_remove_fixup_##KeyType##_##ValueType(Map_##KeyType##_##ValueType *map, MapNode_##KeyType##_##ValueType *x, MapNode_##KeyType##_##ValueType *x_parent) \
  {                                                                                                                                                                            \
    while ((x != map->root) && (!x || x->color == MAP_BLACK_##KeyType##_##ValueType))                                                                                          \
    {                                                                                                                                                                          \
      if (x == (x_parent ? x_parent->left : NULL))                                                                                                                             \
      {                                                                                                                                                                        \
        MapNode_##KeyType##_##ValueType *w = x_parent ? x_parent->right : NULL;                                                                                                \
        if (w && w->color == MAP_RED_##KeyType##_##ValueType)                                                                                                                  \
        {                                                                                                                                                                      \
          w->color = MAP_BLACK_##KeyType##_##ValueType;                                                                                                                        \
          x_parent->color = MAP_RED_##KeyType##_##ValueType;                                                                                                                   \
          map_rotate_left_##KeyType##_##ValueType(map, x_parent);                                                                                                              \
          w = x_parent->right;                                                                                                                                                 \
        }                                                                                                                                                                      \
        if ((!w->left || w->left->color == MAP_BLACK_##KeyType##_##ValueType) &&                                                                                               \
            (!w->right || w->right->color == MAP_BLACK_##KeyType##_##ValueType))                                                                                               \
        {                                                                                                                                                                      \
          if (w)                                                                                                                                                               \
            w->color = MAP_RED_##KeyType##_##ValueType;                                                                                                                        \
          x = x_parent;                                                                                                                                                        \
          x_parent = x ? x->parent : NULL;                                                                                                                                     \
        }                                                                                                                                                                      \
        else                                                                                                                                                                   \
        {                                                                                                                                                                      \
          if (!w->right || w->right->color == MAP_BLACK_##KeyType##_##ValueType)                                                                                               \
          {                                                                                                                                                                    \
            if (w->left)                                                                                                                                                       \
              w->left->color = MAP_BLACK_##KeyType##_##ValueType;                                                                                                              \
            if (w)                                                                                                                                                             \
              w->color = MAP_RED_##KeyType##_##ValueType;                                                                                                                      \
            map_rotate_right_##KeyType##_##ValueType(map, w);                                                                                                                  \
            w = x_parent ? x_parent->right : NULL;                                                                                                                             \
          }                                                                                                                                                                    \
          if (w)                                                                                                                                                               \
            w->color = x_parent->color;                                                                                                                                        \
          if (x_parent)                                                                                                                                                        \
            x_parent->color = MAP_BLACK_##KeyType##_##ValueType;                                                                                                               \
          if (w && w->right)                                                                                                                                                   \
            w->right->color = MAP_BLACK_##KeyType##_##ValueType;                                                                                                               \
          map_rotate_left_##KeyType##_##ValueType(map, x_parent);                                                                                                              \
          x = map->root;                                                                                                                                                       \
        }                                                                                                                                                                      \
      }                                                                                                                                                                        \
      else                                                                                                                                                                     \
      {                                                                                                                                                                        \
        MapNode_##KeyType##_##ValueType *w = x_parent ? x_parent->left : NULL;                                                                                                 \
        if (w && w->color == MAP_RED_##KeyType##_##ValueType)                                                                                                                  \
        {                                                                                                                                                                      \
          w->color = MAP_BLACK_##KeyType##_##ValueType;                                                                                                                        \
          x_parent->color = MAP_RED_##KeyType##_##ValueType;                                                                                                                   \
          map_rotate_right_##KeyType##_##ValueType(map, x_parent);                                                                                                             \
          w = x_parent->left;                                                                                                                                                  \
        }                                                                                                                                                                      \
        if ((!w->right || w->right->color == MAP_BLACK_##KeyType##_##ValueType) &&                                                                                             \
            (!w->left || w->left->color == MAP_BLACK_##KeyType##_##ValueType))                                                                                                 \
        {                                                                                                                                                                      \
          if (w)                                                                                                                                                               \
            w->color = MAP_RED_##KeyType##_##ValueType;                                                                                                                        \
          x = x_parent;                                                                                                                                                        \
          x_parent = x ? x->parent : NULL;                                                                                                                                     \
        }                                                                                                                                                                      \
        else                                                                                                                                                                   \
        {                                                                                                                                                                      \
          if (!w->left || w->left->color == MAP_BLACK_##KeyType##_##ValueType)                                                                                                 \
          {                                                                                                                                                                    \
            if (w->right)                                                                                                                                                      \
              w->right->color = MAP_BLACK_##KeyType##_##ValueType;                                                                                                             \
            if (w)                                                                                                                                                             \
              w->color = MAP_RED_##KeyType##_##ValueType;                                                                                                                      \
            map_rotate_left_##KeyType##_##ValueType(map, w);                                                                                                                   \
            w = x_parent ? x_parent->left : NULL;                                                                                                                              \
          }                                                                                                                                                                    \
          if (w)                                                                                                                                                               \
            w->color = x_parent->color;                                                                                                                                        \
          if (x_parent)                                                                                                                                                        \
            x_parent->color = MAP_BLACK_##KeyType##_##ValueType;                                                                                                               \
          if (w && w->left)                                                                                                                                                    \
            w->left->color = MAP_BLACK_##KeyType##_##ValueType;                                                                                                                \
          map_rotate_right_##KeyType##_##ValueType(map, x_parent);                                                                                                             \
          x = map->root;                                                                                                                                                       \
        }                                                                                                                                                                      \
      }                                                                                                                                                                        \
    }                                                                                                                                                                          \
    if (x)                                                                                                                                                                     \
      x->color = MAP_BLACK_##KeyType##_##ValueType;                                                                                                                            \
  }                                                                                                                                                                            \
                                                                                                                                                                               \
  static inline bool map_insert_##KeyType##_##ValueType(Map_##KeyType##_##ValueType *map, KeyType key, ValueType value)                                                        \
  {                                                                                                                                                                            \
    MapNode_##KeyType##_##ValueType *y = NULL;                                                                                                                                 \
    MapNode_##KeyType##_##ValueType *x = map->root;                                                                                                                            \
    while (x)                                                                                                                                                                  \
    {                                                                                                                                                                          \
      int cmp = CompareFunc(&key, &x->key);                                                                                                                                    \
      if (cmp == 0)                                                                                                                                                            \
      {                                                                                                                                                                        \
        x->value = value; /* update existing value */                                                                                                                          \
        return false;     /* indicate no new node was added */                                                                                                                 \
      }                                                                                                                                                                        \
      y = x;                                                                                                                                                                   \
      x = (cmp < 0) ? x->left : x->right;                                                                                                                                      \
    }                                                                                                                                                                          \
    MapNode_##KeyType##_##ValueType *z = map_node_new_##KeyType##_##ValueType(key, value);                                                                                     \
    z->parent = y;                                                                                                                                                             \
    if (!y)                                                                                                                                                                    \
      map->root = z;                                                                                                                                                           \
    else if (CompareFunc(&z->key, &y->key) < 0)                                                                                                                                \
      y->left = z;                                                                                                                                                             \
    else                                                                                                                                                                       \
      y->right = z;                                                                                                                                                            \
    map_insert_fixup_##KeyType##_##ValueType(map, z);                                                                                                                          \
    map->size++;                                                                                                                                                               \
    return true;                                                                                                                                                               \
  }                                                                                                                                                                            \
                                                                                                                                                                               \
  static inline MapNode_##KeyType##_##ValueType *map_find_node_##KeyType##_##ValueType(MapNode_##KeyType##_##ValueType *node, KeyType key)                                     \
  {                                                                                                                                                                            \
    if (!node)                                                                                                                                                                 \
      return NULL;                                                                                                                                                             \
    int cmp = CompareFunc(&key, &node->key);                                                                                                                                   \
    if (cmp == 0)                                                                                                                                                              \
      return node;                                                                                                                                                             \
    else if (cmp < 0)                                                                                                                                                          \
      return map_find_node_##KeyType##_##ValueType(node->left, key);                                                                                                           \
    else                                                                                                                                                                       \
      return map_find_node_##KeyType##_##ValueType(node->right, key);                                                                                                          \
  }                                                                                                                                                                            \
                                                                                                                                                                               \
  static inline bool map_contains_##KeyType##_##ValueType(Map_##KeyType##_##ValueType *map, KeyType key)                                                                       \
  {                                                                                                                                                                            \
    return map_find_node_##KeyType##_##ValueType(map->root, key) != NULL;                                                                                                      \
  }                                                                                                                                                                            \
                                                                                                                                                                               \
  static inline ValueType *map_get_##KeyType##_##ValueType(Map_##KeyType##_##ValueType *map, KeyType key)                                                                      \
  {                                                                                                                                                                            \
    MapNode_##KeyType##_##ValueType *node = map_find_node_##KeyType##_##ValueType(map->root, key);                                                                             \
    return node ? &node->value : NULL;                                                                                                                                         \
  }                                                                                                                                                                            \
                                                                                                                                                                               \
  static inline MapNode_##KeyType##_##ValueType *map_find_min_node_##KeyType##_##ValueType(MapNode_##KeyType##_##ValueType *node)                                              \
  {                                                                                                                                                                            \
    while (node && node->left)                                                                                                                                                 \
      node = node->left;                                                                                                                                                       \
    return node;                                                                                                                                                               \
  }                                                                                                                                                                            \
                                                                                                                                                                               \
  static inline bool map_remove_##KeyType##_##ValueType(Map_##KeyType##_##ValueType *map, KeyType key)                                                                         \
  {                                                                                                                                                                            \
    MapNode_##KeyType##_##ValueType *z = map->root;                                                                                                                            \
    while (z)                                                                                                                                                                  \
    {                                                                                                                                                                          \
      int cmp = CompareFunc(&key, &z->key);                                                                                                                                    \
      if (cmp == 0)                                                                                                                                                            \
        break;                                                                                                                                                                 \
      else if (cmp < 0)                                                                                                                                                        \
        z = z->left;                                                                                                                                                           \
      else                                                                                                                                                                     \
        z = z->right;                                                                                                                                                          \
    }                                                                                                                                                                          \
    if (!z)                                                                                                                                                                    \
      return false;                                                                                                                                                            \
                                                                                                                                                                               \
    MapNode_##KeyType##_##ValueType *y = z;                                                                                                                                    \
    MapNode_##KeyType##_##ValueType *x = NULL;                                                                                                                                 \
    MapNode_##KeyType##_##ValueType *x_parent = NULL;                                                                                                                          \
    MapColor_##KeyType##_##ValueType y_original_color = y->color;                                                                                                              \
                                                                                                                                                                               \
    if (!z->left)                                                                                                                                                              \
    {                                                                                                                                                                          \
      x = z->right;                                                                                                                                                            \
      x_parent = z->parent;                                                                                                                                                    \
      map_transplant_##KeyType##_##ValueType(map, z, z->right);                                                                                                                \
    }                                                                                                                                                                          \
    else if (!z->right)                                                                                                                                                        \
    {                                                                                                                                                                          \
      x = z->left;                                                                                                                                                             \
      x_parent = z->parent;                                                                                                                                                    \
      map_transplant_##KeyType##_##ValueType(map, z, z->left);                                                                                                                 \
    }                                                                                                                                                                          \
    else                                                                                                                                                                       \
    {                                                                                                                                                                          \
      y = map_find_min_node_##KeyType##_##ValueType(z->right);                                                                                                                 \
      y_original_color = y->color;                                                                                                                                             \
      x = y->right;                                                                                                                                                            \
      if (y->parent == z)                                                                                                                                                      \
      {                                                                                                                                                                        \
        if (x)                                                                                                                                                                 \
          x->parent = y;                                                                                                                                                       \
        x_parent = y;                                                                                                                                                          \
      }                                                                                                                                                                        \
      else                                                                                                                                                                     \
      {                                                                                                                                                                        \
        map_transplant_##KeyType##_##ValueType(map, y, y->right);                                                                                                              \
        y->right = z->right;                                                                                                                                                   \
        if (y->right)                                                                                                                                                          \
          y->right->parent = y;                                                                                                                                                \
        x_parent = y->parent;                                                                                                                                                  \
      }                                                                                                                                                                        \
      map_transplant_##KeyType##_##ValueType(map, z, y);                                                                                                                       \
      y->left = z->left;                                                                                                                                                       \
      if (y->left)                                                                                                                                                             \
        y->left->parent = y;                                                                                                                                                   \
      y->color = z->color;                                                                                                                                                     \
    }                                                                                                                                                                          \
                                                                                                                                                                               \
    free(z);                                                                                                                                                                   \
    map->size--;                                                                                                                                                               \
                                                                                                                                                                               \
    if (y_original_color == MAP_BLACK_##KeyType##_##ValueType)                                                                                                                 \
      map_remove_fixup_##KeyType##_##ValueType(map, x, x_parent);                                                                                                              \
                                                                                                                                                                               \
    return true;                                                                                                                                                               \
  }                                                                                                                                                                            \
                                                                                                                                                                               \
  static inline int map_size_##KeyType##_##ValueType(Map_##KeyType##_##ValueType *map)                                                                                         \
  {                                                                                                                                                                            \
    return map->size;                                                                                                                                                          \
  }                                                                                                                                                                            \
                                                                                                                                                                               \
  static inline bool map_empty_##KeyType##_##ValueType(Map_##KeyType##_##ValueType *map)                                                                                       \
  {                                                                                                                                                                            \
    return map->size == 0;                                                                                                                                                     \
  }                                                                                                                                                                            \
                                                                                                                                                                               \
  static inline void map_clear_node_##KeyType##_##ValueType(MapNode_##KeyType##_##ValueType *node)                                                                             \
  {                                                                                                                                                                            \
    if (!node)                                                                                                                                                                 \
      return;                                                                                                                                                                  \
    map_clear_node_##KeyType##_##ValueType(node->left);                                                                                                                        \
    map_clear_node_##KeyType##_##ValueType(node->right);                                                                                                                       \
    free(node);                                                                                                                                                                \
  }                                                                                                                                                                            \
                                                                                                                                                                               \
  static inline void map_clear_##KeyType##_##ValueType(Map_##KeyType##_##ValueType *map)                                                                                       \
  {                                                                                                                                                                            \
    map_clear_node_##KeyType##_##ValueType(map->root);                                                                                                                         \
    map->root = NULL;                                                                                                                                                          \
    map->size = 0;                                                                                                                                                             \
  }                                                                                                                                                                            \
                                                                                                                                                                               \
  static inline void free_map_##KeyType##_##ValueType(Map_##KeyType##_##ValueType *map)                                                                                        \
  {                                                                                                                                                                            \
    map_clear_##KeyType##_##ValueType(map);                                                                                                                                    \
  }                                                                                                                                                                            \
                                                                                                                                                                               \
  static inline void map_to_str_node_##KeyType##_##ValueType(                                                                                                                  \
      MapNode_##KeyType##_##ValueType *node, char *buf, size_t size,                                                                                                           \
      void (*key_to_str)(KeyType, char *, size_t),                                                                                                                             \
      void (*val_to_str)(ValueType, char *, size_t), bool *is_first)                                                                                                           \
  {                                                                                                                                                                            \
    if (!node)                                                                                                                                                                 \
      return;                                                                                                                                                                  \
    map_to_str_node_##KeyType##_##ValueType(node->left, buf, size, key_to_str, val_to_str, is_first);                                                                          \
    if (!*is_first)                                                                                                                                                            \
    {                                                                                                                                                                          \
      size_t len = strlen(buf);                                                                                                                                                \
      if (len + 2 < size)                                                                                                                                                      \
        strcat(buf, ", ");                                                                                                                                                     \
    }                                                                                                                                                                          \
    size_t len = strlen(buf);                                                                                                                                                  \
    if (len < size)                                                                                                                                                            \
    {                                                                                                                                                                          \
      strcat(buf, "{");                                                                                                                                                        \
      key_to_str(node->key, buf + strlen(buf), size - strlen(buf));                                                                                                            \
      strcat(buf, " : ");                                                                                                                                                      \
      val_to_str(node->value, buf + strlen(buf), size - strlen(buf));                                                                                                          \
      strcat(buf, "}");                                                                                                                                                        \
    }                                                                                                                                                                          \
    *is_first = false;                                                                                                                                                         \
    map_to_str_node_##KeyType##_##ValueType(node->right, buf, size, key_to_str, val_to_str, is_first);                                                                         \
  }                                                                                                                                                                            \
                                                                                                                                                                               \
  static inline char *map_to_str_##KeyType##_##ValueType##_custom(Map_##KeyType##_##ValueType *map,                                                                            \
                                                                  void (*key_to_str)(KeyType, char *, size_t),                                                                 \
                                                                  void (*val_to_str)(ValueType, char *, size_t))                                                               \
  {                                                                                                                                                                            \
    if (!map)                                                                                                                                                                  \
      return NULL;                                                                                                                                                             \
    size_t bufsize = map->size * BUF_SIZE + 32;                                                                                                                                \
    char *buffer = malloc(bufsize);                                                                                                                                            \
    if (!buffer)                                                                                                                                                               \
      return NULL;                                                                                                                                                             \
    strcpy(buffer, "{");                                                                                                                                                       \
    bool is_first = true;                                                                                                                                                      \
    map_to_str_node_##KeyType##_##ValueType(map->root, buffer, bufsize, key_to_str, val_to_str, &is_first);                                                                    \
    strcat(buffer, "}");                                                                                                                                                       \
    return buffer;                                                                                                                                                             \
  }                                                                                                                                                                            \
  /* Iterator Functions */                                                                                                                                                     \
  static inline void map_foreach_node_##KeyType##_##ValueType(                                                                                                                 \
      MapNode_##KeyType##_##ValueType *node,                                                                                                                                   \
      void (*fn)(KeyType * key, ValueType * val, void *userdata),                                                                                                              \
      void *userdata)                                                                                                                                                          \
  {                                                                                                                                                                            \
    if (!node)                                                                                                                                                                 \
      return;                                                                                                                                                                  \
    map_foreach_node_##KeyType##_##ValueType(node->left, fn, userdata);                                                                                                        \
    fn(&node->key, &node->value, userdata);                                                                                                                                    \
    map_foreach_node_##KeyType##_##ValueType(node->right, fn, userdata);                                                                                                       \
  }                                                                                                                                                                            \
                                                                                                                                                                               \
  static inline void map_foreach_##KeyType##_##ValueType(                                                                                                                      \
      Map_##KeyType##_##ValueType *map,                                                                                                                                        \
      void (*fn)(KeyType * key, ValueType * val, void *userdata),                                                                                                              \
      void *userdata)                                                                                                                                                          \
  {                                                                                                                                                                            \
    map_foreach_node_##KeyType##_##ValueType(map->root, fn, userdata);                                                                                                         \
  }

#define CLIP_REGISTER_MAP_PRINT(KeyType, ValueType, key_fn, val_fn)                               \
  static inline char *map_to_str_##KeyType##_##ValueType##_main(Map_##KeyType##_##ValueType *map) \
  {                                                                                               \
    return map_to_str_##KeyType##_##ValueType##_custom(map, key_fn, val_fn);                      \
  }

#define Map(KeyType, ValueType) Map_##KeyType##_##ValueType
#define Map_init(KeyType, ValueType) init_map_##KeyType##_##ValueType()
#define Map_insert(KeyType, ValueType, map, key, val) map_insert_##KeyType##_##ValueType(map, key, val)
#define Map_get(KeyType, ValueType, map, key) map_get_##KeyType##_##ValueType(map, key)
#define Map_contains(KeyType, ValueType, map, key) map_contains_##KeyType##_##ValueType(map, key)
#define Map_to_str(KeyType, ValueType, map) map_to_str_##KeyType##_##ValueType##_main(map)
#define Map_to_str_custom(KeyType, ValueType, map, keyfn, valfn) map_to_str_##KeyType##_##ValueType##_custom(map, keyfn, valfn)
#define Map_remove(KeyType, ValueType, map, key) map_remove_##KeyType##_##ValueType(map, key)
#define Map_size(KeyType, ValueType, map) map_size_##KeyType##_##ValueType(map)
#define Map_empty(KeyType, ValueType, map) map_empty_##KeyType##_##ValueType(map)
#define Map_clear(KeyType, ValueType, map) map_clear_##KeyType##_##ValueType(map)
#define Map_free(KeyType, ValueType, map) free_map_##KeyType##_##ValueType(map)
#define Map_foreach(KeyType, ValueType, map, fn, userdata) map_foreach_##KeyType##_##ValueType(map, fn, userdata)

#define Map_print(KeyType, map)          \
  do                                     \
  {                                      \
    char *_s = Map_to_str(KeyType, map); \
    if (_s)                              \
    {                                    \
      printf("%s", _s);                  \
      free(_s);                          \
    }                                    \
  } while (0)

#define Map_println(KeyType, map) \
  do                              \
  {                               \
    Map_print(KeyType, map);      \
    printf("\n");                 \
  } while (0)

#endif /* CLIP_MAP_H */