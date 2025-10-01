/*
 * @author Eduardo I. Lopez H. - eduardo98m@gmail.com
 * @brief In this file we define a series of macros that generates a **type-safe** stack
 * for any given element type in C.
 *
 * Example:
 * // Assuming no second argument is used for default behavior
 * CLIP_DEFINE_STACK_TYPE(int)
 *
 * Stack(int) stack = Stack_init(int, 4);
 * Stack_push(int, &stack, 42);
 * int value;
 * Stack_pop(int, &stack, &value);
 * Stack_free(int, &stack);
 *
 * The following methods are generated automatically:
 * - init
 * - init_from_array
 * - ensure_capacity
 * - push
 * - pop
 * - peek
 * - peek_ptr
 * - is_empty
 * - size
 * - capacity
 * - clear
 * - reserve
 * - shrink_to_fit
 * - to_str (requires registration via CLIP_REGISTER_STACK_PRINT)
 * - to_str_custom (takes a user-supplied function)
 * - reverse
 * - copy
 * - free
 */
#ifndef CLIP_STACK_H
#define CLIP_STACK_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CLIP_DEFINE_STACK_TYPE(...) \
    CLIP_DEFINE_STACK_TYPE_IMPL(__VA_ARGS__, NULL, 256)

/**
 * @brief Defines the STACK type with a given buffer size
 */
#define CLIP_DEFINE_STACK_TYPE_BUF(Type, BUF) \
    CLIP_DEFINE_STACK_TYPE_IMPL(Type, NULL, BUF)

/**
 * @brief Defines the STACK type with a given free function
 */
#define CLIP_DEFINE_STACK_TYPE_WITH_FREE(Type, FREE_FN) \
    CLIP_DEFINE_STACK_TYPE_IMPL(Type, FREE_FN, 256)

/**
 * @brief Defines the STACK type with a given free function and a custom buffer size for the print
 */
#define CLIP_DEFINE_STACK_TYPE_FULL(Type, FREE_FN, BUF) \
    CLIP_DEFINE_STACK_TYPE_IMPL(Type, FREE_FN, BUF)

/**
 * @brief Define a type-safe stack for the given element type.
 *
 * This macro generates:
 * - A typedef `Stack_<Type>` structure containing `data`, `size`, and `capacity`.
 * - A set of **static inline functions** specialized for `<Type>`:
 *
 * - `init_stack_<Type>`
 * - `init_stack_from_array_<Type>`
 * - `stack_ensure_capacity_<Type>`
 * - `stack_push_<Type>`
 * - `stack_pop_<Type>`
 * - `stack_peek_<Type>`
 * - `stack_peek_ptr_<Type>`
 * - `stack_is_empty_<Type>`
 * - `stack_size_<Type>`
 * - `stack_capacity_<Type>`
 * - `stack_clear_<Type>`
 * - `stack_reserve_<Type>`
 * - `stack_shrink_to_fit_<Type>`
 * - `stack_to_str_<Type>_custom`
 * - `stack_reverse_<Type>`
 * - `stack_copy_<Type>`
 * - `free_stack_<Type>`
 *
 * @param Type The element type (e.g., `int`, `float`, `struct Foo`).
 * @param FREE_FN Function to free th
 * @param BUF_SIZE Optional: Buffer size allocated per element for string conversion (used by `stack_to_str_..._custom`). Default is 256.
 *
 * @note Each instantiation of this macro generates a new independent
 * stack type and associated functions, all scoped within the including file.
 *
 * @note Usage examples:
 * CLIP_DEFINE_STACK_TYPE(int)             // default BUF_SIZE = 256
 * CLIP_DEFINE_STACK_TYPE(Student, 512)   // custom BUF_SIZE = 512
 */
#define CLIP_DEFINE_STACK_TYPE_IMPL(Type, FREE_FN, BUF_SIZE, ...)               \
    typedef struct                                                              \
    {                                                                           \
        Type *data;                                                             \
        int size;                                                               \
        int capacity;                                                           \
    } Stack_##Type;                                                             \
                                                                                \
    static inline Stack_##Type init_stack_##Type(int capacity)                  \
    {                                                                           \
        Stack_##Type stack;                                                     \
        stack.data = malloc(capacity * sizeof(Type));                           \
        if (!stack.data)                                                        \
        {                                                                       \
            fprintf(stderr, "Memory allocation failed!\n");                     \
            exit(EXIT_FAILURE);                                                 \
        }                                                                       \
        stack.size = 0;                                                         \
        stack.capacity = capacity;                                              \
        return stack;                                                           \
    }                                                                           \
                                                                                \
    static inline Stack_##Type init_stack_from_array_##Type(const Type *arr,    \
                                                            int n)              \
    {                                                                           \
        Stack_##Type stack;                                                     \
        stack.data = malloc(n * sizeof(Type));                                  \
        if (!stack.data && n > 0)                                               \
        {                                                                       \
            fprintf(stderr, "Memory allocation failed!\n");                     \
            exit(EXIT_FAILURE);                                                 \
        }                                                                       \
        memcpy(stack.data, arr, n * sizeof(Type));                              \
        stack.size = n;                                                         \
        stack.capacity = n;                                                     \
        return stack;                                                           \
    }                                                                           \
                                                                                \
    static inline bool stack_ensure_capacity_##Type(Stack_##Type *stack,        \
                                                    int needed)                 \
    {                                                                           \
        if (stack->size + needed > stack->capacity)                             \
        {                                                                       \
            int new_capacity = stack->capacity ? stack->capacity * 2 : 1;       \
            while (new_capacity < stack->size + needed)                         \
                new_capacity *= 2;                                              \
            Type *new_data = realloc(stack->data, new_capacity * sizeof(Type)); \
            if (!new_data)                                                      \
                return false;                                                   \
            stack->data = new_data;                                             \
            stack->capacity = new_capacity;                                     \
        }                                                                       \
        return true;                                                            \
    }                                                                           \
                                                                                \
    static inline bool stack_push_##Type(Stack_##Type *stack, Type value)       \
    {                                                                           \
        if (!stack_ensure_capacity_##Type(stack, 1))                            \
            return false;                                                       \
        stack->data[stack->size++] = value;                                     \
        return true;                                                            \
    }                                                                           \
                                                                                \
    static inline bool stack_pop_##Type(Stack_##Type *stack, Type *out)         \
    {                                                                           \
        if (stack->size == 0)                                                   \
            return false;                                                       \
        if (out)                                                                \
            *out = stack->data[stack->size - 1];                                \
        stack->size--;                                                          \
        return true;                                                            \
    }                                                                           \
                                                                                \
    static inline bool stack_peek_##Type(Stack_##Type *stack, Type *out)        \
    {                                                                           \
        if (stack->size == 0)                                                   \
            return false;                                                       \
        if (out)                                                                \
            *out = stack->data[stack->size - 1];                                \
        return true;                                                            \
    }                                                                           \
                                                                                \
    static inline Type *stack_peek_ptr_##Type(Stack_##Type *stack)              \
    {                                                                           \
        if (stack->size == 0)                                                   \
            return NULL;                                                        \
        return &stack->data[stack->size - 1];                                   \
    }                                                                           \
                                                                                \
    static inline bool stack_is_empty_##Type(Stack_##Type *stack)               \
    {                                                                           \
        return stack->size == 0;                                                \
    }                                                                           \
                                                                                \
    static inline int stack_size_##Type(Stack_##Type *stack)                    \
    {                                                                           \
        return stack->size;                                                     \
    }                                                                           \
                                                                                \
    static inline int stack_capacity_##Type(Stack_##Type *stack)                \
    {                                                                           \
        return stack->capacity;                                                 \
    }                                                                           \
                                                                                \
    static inline void stack_clear_##Type(Stack_##Type *stack)                  \
    {                                                                           \
        stack->size = 0;                                                        \
    }                                                                           \
                                                                                \
    static inline bool stack_reserve_##Type(Stack_##Type *stack, int capacity)  \
    {                                                                           \
        if (capacity <= stack->capacity)                                        \
            return true;                                                        \
        Type *new_data = realloc(stack->data, capacity * sizeof(Type));         \
        if (!new_data)                                                          \
            return false;                                                       \
        stack->data = new_data;                                                 \
        stack->capacity = capacity;                                             \
        return true;                                                            \
    }                                                                           \
                                                                                \
    static inline bool stack_shrink_to_fit_##Type(Stack_##Type *stack)          \
    {                                                                           \
        if (stack->size == stack->capacity)                                     \
            return true;                                                        \
        Type *new_data = realloc(stack->data, stack->size * sizeof(Type));      \
        if (!new_data && stack->size > 0)                                       \
            return false;                                                       \
        stack->data = new_data;                                                 \
        stack->capacity = stack->size;                                          \
        return true;                                                            \
    }                                                                           \
                                                                                \
    static inline char *stack_to_str_##Type##_custom(                           \
        Stack_##Type *stack, void (*elem_to_str)(Type, char *, size_t))         \
    {                                                                           \
        if (!stack)                                                             \
            return NULL;                                                        \
        size_t bufsize = stack->size * BUF_SIZE + 16;                           \
        char *buffer = malloc(bufsize);                                         \
        if (!buffer)                                                            \
            return NULL;                                                        \
        strcpy(buffer, "[top: ");                                               \
        char elem[BUF_SIZE];                                                    \
        for (int i = stack->size - 1; i >= 0; i--)                              \
        {                                                                       \
            elem_to_str(stack->data[i], elem, sizeof(elem));                    \
            strcat(buffer, elem);                                               \
            if (i > 0)                                                          \
                strcat(buffer, ", ");                                           \
        }                                                                       \
        strcat(buffer, " :bottom]");                                            \
        return buffer;                                                          \
    }                                                                           \
                                                                                \
    static inline void stack_reverse_##Type(Stack_##Type *stack)                \
    {                                                                           \
        if (stack->size < 2)                                                    \
            return;                                                             \
        for (int i = 0; i < stack->size / 2; i++)                               \
        {                                                                       \
            Type temp = stack->data[i];                                         \
            stack->data[i] = stack->data[stack->size - 1 - i];                  \
            stack->data[stack->size - 1 - i] = temp;                            \
        }                                                                       \
    }                                                                           \
                                                                                \
    static inline Stack_##Type stack_copy_##Type(const Stack_##Type *src)       \
    {                                                                           \
        Stack_##Type copy;                                                      \
        copy.data = malloc(src->capacity * sizeof(Type));                       \
        if (!copy.data && src->capacity > 0)                                    \
        {                                                                       \
            fprintf(stderr, "Memory allocation failed!\n");                     \
            exit(EXIT_FAILURE);                                                 \
        }                                                                       \
        memcpy(copy.data, src->data, src->size * sizeof(Type));                 \
        copy.size = src->size;                                                  \
        copy.capacity = src->capacity;                                          \
        return copy;                                                            \
    }                                                                           \
                                                                                \
    static inline void free_stack_##Type(Stack_##Type *stack)                   \
    {                                                                           \
        void (*free_fn)(Type *) = FREE_FN;                                      \
        if (free_fn)                                                            \
        {                                                                       \
            for (int i = 0; i < stack->size; i++)                               \
            {                                                                   \
                                                                                \
                (free_fn)(&stack->data[i]);                                     \
            }                                                                   \
        }                                                                       \
        free(stack->data);                                                      \
        stack->data = NULL;                                                     \
        stack->size = 0;                                                        \
        stack->capacity = 0;                                                    \
    }

/**
 * @brief Registers the default print function for a specific stack type.
 *
 * This macro **defines** the `stack_to_str_<Type>_main` function, which is used
 * by convenience macros like `Stack_to_str` and `Stack_print`.
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
 * CLIP_DEFINE_STACK_TYPE(int);
 * CLIP_REGISTER_STACK_PRINT(int, int_to_str);
 *
 * Stack(int) stack = Stack_init(int, 4);
 * Stack_push(int, &stack, 42);
 * Stack_print(int, &stack);  // Uses registered function!
 * ```
 */
#define CLIP_REGISTER_STACK_PRINT(Type, print_fn)                                  \
    static inline char *stack_to_str_##Type##_main(Stack_##Type *stack)            \
    {                                                                              \
        if (!print_fn)                                                             \
        {                                                                          \
            fprintf(stderr, "No print function registered for type " #Type "!\n"); \
            return NULL;                                                           \
        }                                                                          \
        return stack_to_str_##Type##_custom(stack, print_fn);                      \
    }

/**
 * @def Stack(Type)
 * @brief Alias to the generated `Stack_<Type>` struct.
 *
 * Example:
 * ```c
 * Stack(int) stack = Stack_init(int, 8);
 * ```
 */
#define Stack(Type) Stack_##Type

/**
 * @def Stack_init(Type, cap)
 * @brief Initialize a stack of the given type with the given initial capacity.
 */
#define Stack_init(Type, cap) init_stack_##Type(cap)

/**
 * @def Stack_init_from_array(Type, arr, n)
 * @brief Initialize a stack with elements copied from an existing C array.
 * Elements are pushed in array order (arr[0] is at bottom, arr[n-1] is at top).
 *
 * Example:
 * ```c
 * int values[] = {1, 2, 3, 4};
 * Stack(int) stack = Stack_init_from_array(int, values, 4);
 * // Stack top to bottom: 4, 3, 2, 1
 *
 * // or using a compound literal:
 * Stack(int) stack2 = Stack_init_from_array(int, (int[]){10, 20, 30}, 3);
 * // Stack top to bottom: 30, 20, 10
 * ```
 *
 * @param Type The element type (e.g., `int`, `float`).
 * @param arr  Pointer to the source C array.
 * @param n    Number of elements in the source array.
 * @return A fully initialized stack containing the copied elements.
 */
#define Stack_init_from_array(Type, arr, n) init_stack_from_array_##Type(arr, n)

/**
 * @def Stack_init_from_static_array(Type, arr)
 * @brief Initialize a stack from a compile-time array.
 *
 * This macro automatically infers the length of a static (stack-allocated)
 * array and calls the generated `init_stack_from_array_<Type>` function.
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
 * Stack(int) stack = Stack_init_from_static_array(int, xs);
 * // Stack top to bottom: 3, 2, 1
 * ```
 */
#define Stack_init_from_static_array(Type, arr) \
    init_stack_from_array_##Type((arr), (int)(sizeof(arr) / sizeof((arr)[0])))

/**
 * @def Stack_push(Type, stack, val)
 * @brief Push a value onto the top of the stack (resizing if needed).
 */
#define Stack_push(Type, stack, val) stack_push_##Type(stack, val)

/**
 * @def Stack_pop(Type, stack, val)
 * @brief Pop a value from the top of the stack.
 * @param val Pointer to store the popped value (can be NULL if you don't need the value).
 * @return true if successful, false if stack is empty.
 */
#define Stack_pop(Type, stack, val) stack_pop_##Type(stack, val)

/**
 * @def Stack_peek(Type, stack, val)
 * @brief Look at the top value without removing it.
 * @param val Pointer to store the top value (can be NULL if you just want to check if stack is non-empty).
 * @return true if successful, false if stack is empty.
 */
#define Stack_peek(Type, stack, val) stack_peek_##Type(stack, val)

/**
 * @def Stack_peek_ptr(Type, stack)
 * @brief Get a pointer to the top element without removing it.
 * @return Pointer to the top element, or NULL if stack is empty.
 */
#define Stack_peek_ptr(Type, stack) stack_peek_ptr_##Type(stack)

/**
 * @def Stack_is_empty(Type, stack)
 * @brief Check if the stack is empty.
 */
#define Stack_is_empty(Type, stack) stack_is_empty_##Type(stack)

/**
 * @def Stack_size(Type, stack)
 * @brief Get the current number of elements in the stack.
 */
#define Stack_size(Type, stack) stack_size_##Type(stack)

/**
 * @def Stack_capacity(Type, stack)
 * @brief Get the current capacity of the stack.
 */
#define Stack_capacity(Type, stack) stack_capacity_##Type(stack)

/**
 * @def Stack_clear(Type, stack)
 * @brief Remove all elements from the stack without freeing memory.
 */
#define Stack_clear(Type, stack) stack_clear_##Type(stack)

/**
 * @def Stack_reserve(Type, stack, cap)
 * @brief Ensure the stack has at least `cap` capacity.
 */
#define Stack_reserve(Type, stack, cap) stack_reserve_##Type(stack, cap)

/**
 * @def Stack_shrink_to_fit(Type, stack)
 * @brief Shrink the stack's capacity down to its current size.
 */
#define Stack_shrink_to_fit(Type, stack) stack_shrink_to_fit_##Type(stack)

/**
 * @def Stack_to_str(Type, stack)
 * @brief Convert stack contents to a string using the registered
 * print function for this type. The returned string must be freed by the caller.
 *
 * @note You must call `CLIP_REGISTER_STACK_PRINT(Type, fn)` for this type first.
 */
#define Stack_to_str(Type, stack) stack_to_str_##Type##_main(stack)

/**
 * @def Stack_to_str_custom(Type, stack, fn)
 * @brief Convert stack contents to a string using a user-supplied
 *        element-to-string function.
 */
#define Stack_to_str_custom(Type, stack, fn) stack_to_str_##Type##_custom(stack, fn)

/**
 * @def Stack_reverse(Type, stack)
 * @brief Reverses the given stack (top becomes bottom, bottom becomes top).
 */
#define Stack_reverse(Type, stack) stack_reverse_##Type(stack)

/**
 * @def Stack_copy(Type, src)
 * @brief Create a deep copy of the stack.
 */
#define Stack_copy(Type, src) stack_copy_##Type(src)

/**
 * @def Stack_free(Type, stack)
 * @brief Free all memory held by the stack and reset its fields.
 */
#define Stack_free(Type, stack) free_stack_##Type(stack)

/**
 * @def Stack_print(Type, stack)
 * @brief Pretty-print a stack directly to stdout using the registered
 * print function. (Does not print a trailing newline)
 *
 * @note You must call `CLIP_REGISTER_STACK_PRINT(Type, fn)` for this type first.
 *
 * Example:
 * ```c
 * CLIP_REGISTER_STACK_PRINT(int, int_to_str);
 * Stack_print(int, &my_stack);
 * ```
 */
#define Stack_print(Type, stack)                    \
    do                                              \
    {                                               \
        char *_tmp_str = Stack_to_str(Type, stack); \
        if (_tmp_str)                               \
        {                                           \
            printf("%s", _tmp_str);                 \
            free(_tmp_str);                         \
        }                                           \
    } while (0)

/**
 * @def Stack_print_custom(Type, stack, fn)
 * @brief Pretty-print a stack directly to stdout using the given
 * element-to-string function. (Does not print the end-line character)
 *
 * Example:
 * ```c
 * Stack_print_custom(Student, &student_stack, Student_to_str);
 * ```
 */
#define Stack_print_custom(Type, stack, fn)                    \
    do                                                         \
    {                                                          \
        char *_tmp_str = Stack_to_str_custom(Type, stack, fn); \
        if (_tmp_str)                                          \
        {                                                      \
            printf("%s", _tmp_str);                            \
            free(_tmp_str);                                    \
        }                                                      \
    } while (0)

/**
 * @def Stack_println(Type, stack)
 * @brief Pretty-print a stack directly to stdout using the registered
 * print function, followed by a newline.
 *
 * @note You must call `CLIP_REGISTER_STACK_PRINT(Type, fn)` for this type first.
 */
#define Stack_println(Type, stack) \
    do                             \
    {                              \
        Stack_print(Type, stack);  \
        printf("\n");              \
    } while (0)

/**
 * @def Stack_println_custom(Type, stack, fn)
 * @brief Pretty-print a stack directly to stdout using a custom
 * print function, followed by a newline.
 */
#define Stack_println_custom(Type, stack, fn) \
    do                                        \
    {                                         \
        Stack_print_custom(Type, stack, fn);  \
        printf("\n");                         \
    } while (0)

#endif /* CLIP_STACK_H */