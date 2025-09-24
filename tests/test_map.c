#include "CLIP/Test.h"
#include "CLIP/Map.h"
#include <string.h>
#include <stdlib.h>

typedef const char* string;

// comparator for string keys
int cmp_str(const string *a, const string *b) {
    return strcmp(*a, *b);
}

// comparator for int keys
int cmp_int(const int *a, const int *b) {
    return (*a < *b) ? -1 : ((*a > *b) ? 1 : 0);
}

// key/val -> string
void str_to_str(string s, char *buf, size_t n) {
    snprintf(buf, n, "\"%s\"", s);
}
void int_to_str(int v, char *buf, size_t n) {
    snprintf(buf, n, "%d", v);
}

// define maps
CLIP_DEFINE_MAP_TYPE(string, int, cmp_str);
CLIP_REGISTER_MAP_PRINT(string, int, str_to_str, int_to_str);

CLIP_DEFINE_MAP_TYPE(int, string, cmp_int);
CLIP_REGISTER_MAP_PRINT(int, string, int_to_str, str_to_str);

CLIP_DEFINE_MAP_TYPE(int, int, cmp_int);
CLIP_REGISTER_MAP_PRINT(int, int, int_to_str, int_to_str);

// ========== BASIC FUNCTIONALITY TESTS ==========

TEST(map_init_empty) {
    Map(string, int) m = Map_init(string, int);
    ASSERT_TRUE(Map_size(string, int, &m) == 0);
    ASSERT_TRUE(Map_empty(string, int, &m));
    Map_free(string, int, &m);
}

TEST(map_insert_and_get) {
    Map(string, int) m = Map_init(string, int);
    Map_insert(string, int, &m, "Hello", 4);
    ASSERT_TRUE(*Map_get(string, int, &m, "Hello") == 4);
    Map_free(string, int, &m);
}

TEST(map_overwrite) {
    Map(string, int) m = Map_init(string, int);
    Map_insert(string, int, &m, "A", 1);
    Map_insert(string, int, &m, "A", 2);
    ASSERT_TRUE(*Map_get(string, int, &m, "A") == 2);
    ASSERT_TRUE(Map_size(string, int, &m) == 1); // size should not increase
    Map_free(string, int, &m);
}

TEST(map_contains) {
    Map(string, int) m = Map_init(string, int);
    Map_insert(string, int, &m, "Key", 42);
    ASSERT_TRUE(Map_contains(string, int, &m, "Key"));
    ASSERT_FALSE(Map_contains(string, int, &m, "Other"));
    Map_free(string, int, &m);
}

// ========== REMOVE TESTS ==========

TEST(map_remove_existing) {
    Map(string, int) m = Map_init(string, int);
    Map_insert(string, int, &m, "X", 99);
    ASSERT_TRUE(Map_remove(string, int, &m, "X"));
    ASSERT_FALSE(Map_contains(string, int, &m, "X"));
    ASSERT_TRUE(Map_size(string, int, &m) == 0);
    Map_free(string, int, &m);
}

TEST(map_remove_nonexistent) {
    Map(string, int) m = Map_init(string, int);
    ASSERT_FALSE(Map_remove(string, int, &m, "Nope"));
    Map_free(string, int, &m);
}

TEST(map_remove_from_empty) {
    Map(string, int) m = Map_init(string, int);
    ASSERT_FALSE(Map_remove(string, int, &m, "Nothing"));
    Map_free(string, int, &m);
}

TEST(map_remove_multiple) {
    Map(string, int) m = Map_init(string, int);
    Map_insert(string, int, &m, "A", 1);
    Map_insert(string, int, &m, "B", 2);
    Map_insert(string, int, &m, "C", 3);
    
    ASSERT_TRUE(Map_remove(string, int, &m, "B"));
    ASSERT_TRUE(Map_size(string, int, &m) == 2);
    ASSERT_TRUE(Map_contains(string, int, &m, "A"));
    ASSERT_FALSE(Map_contains(string, int, &m, "B"));
    ASSERT_TRUE(Map_contains(string, int, &m, "C"));
    
    Map_free(string, int, &m);
}

TEST(map_remove_root_node) {
    Map(string, int) m = Map_init(string, int);
    Map_insert(string, int, &m, "M", 1);
    Map_insert(string, int, &m, "A", 2);
    Map_insert(string, int, &m, "Z", 3);
    
    ASSERT_TRUE(Map_remove(string, int, &m, "M")); // remove root
    ASSERT_TRUE(Map_size(string, int, &m) == 2);
    ASSERT_FALSE(Map_contains(string, int, &m, "M"));
    ASSERT_TRUE(Map_contains(string, int, &m, "A"));
    ASSERT_TRUE(Map_contains(string, int, &m, "Z"));
    
    Map_free(string, int, &m);
}

// ========== CLEAR TESTS ==========

TEST(map_clear) {
    Map(string, int) m = Map_init(string, int);
    Map_insert(string, int, &m, "One", 1);
    Map_insert(string, int, &m, "Two", 2);
    ASSERT_TRUE(Map_size(string, int, &m) == 2);
    Map_clear(string, int, &m);
    ASSERT_TRUE(Map_size(string, int, &m) == 0);
    ASSERT_TRUE(Map_empty(string, int, &m));
    Map_free(string, int, &m);
}

TEST(map_clear_empty) {
    Map(string, int) m = Map_init(string, int);
    Map_clear(string, int, &m);
    ASSERT_TRUE(Map_empty(string, int, &m));
    Map_free(string, int, &m);
}

TEST(map_clear_and_reuse) {
    Map(string, int) m = Map_init(string, int);
    Map_insert(string, int, &m, "A", 1);
    Map_clear(string, int, &m);
    
    Map_insert(string, int, &m, "B", 2);
    ASSERT_TRUE(Map_size(string, int, &m) == 1);
    ASSERT_TRUE(*Map_get(string, int, &m, "B") == 2);
    ASSERT_FALSE(Map_contains(string, int, &m, "A"));
    
    Map_free(string, int, &m);
}

// ========== GET AND NULL TESTS ==========

TEST(map_get_null_for_nonexistent) {
    Map(string, int) m = Map_init(string, int);
    Map_insert(string, int, &m, "exists", 42);
    
    int *val = Map_get(string, int, &m, "exists");
    ASSERT_NOT_NULL(val);
    ASSERT_TRUE(*val == 42);
    
    val = Map_get(string, int, &m, "nonexistent");
    ASSERT_NULL(val);
    
    Map_free(string, int, &m);
}

TEST(map_get_from_empty) {
    Map(string, int) m = Map_init(string, int);
    int *val = Map_get(string, int, &m, "anything");
    ASSERT_NULL(val);
    Map_free(string, int, &m);
}

// ========== STRESS AND EDGE CASE TESTS ==========

TEST(map_large) {
    Map(string, int) m = Map_init(string, int);
    char **keys = malloc(100 * sizeof(char*));
    
    // Insert 100 elements
    for (int i = 0; i < 100; i++) {
        keys[i] = malloc(32);
        snprintf(keys[i], 32, "K%d", i);
        Map_insert(string, int, &m, keys[i], i * 2);
    }
    
    ASSERT_TRUE(Map_size(string, int, &m) == 100);
    
    // Check all elements exist
    for (int i = 0; i < 100; i++) {
        ASSERT_TRUE(Map_contains(string, int, &m, keys[i]));
        int *val = Map_get(string, int, &m, keys[i]);
        ASSERT_NOT_NULL(val);
        ASSERT_TRUE(*val == i * 2);
    }
    
    // Remove every other element
    for (int i = 0; i < 100; i += 2) {
        ASSERT_TRUE(Map_remove(string, int, &m, keys[i]));
    }
    
    ASSERT_TRUE(Map_size(string, int, &m) == 50);
    
    // Check remaining elements
    for (int i = 1; i < 100; i += 2) {
        ASSERT_TRUE(Map_contains(string, int, &m, keys[i]));
    }
    
    // Cleanup
    for (int i = 0; i < 100; i++) {
        free(keys[i]);
    }
    free(keys);
    Map_free(string, int, &m);
}

TEST(map_stress_insert_remove) {
    Map(int, int) m = Map_init(int, int);
    
    // Insert elements 0-99
    for (int i = 0; i < 100; i++) {
        Map_insert(int, int, &m, i, i * i);
    }
    
    // Remove elements 25-74
    for (int i = 25; i < 75; i++) {
        ASSERT_TRUE(Map_remove(int, int, &m, i));
    }
    
    ASSERT_TRUE(Map_size(int, int, &m) == 50);
    
    // Check remaining elements
    for (int i = 0; i < 25; i++) {
        ASSERT_TRUE(Map_contains(int, int, &m, i));
    }
    for (int i = 75; i < 100; i++) {
        ASSERT_TRUE(Map_contains(int, int, &m, i));
    }
    
    // Ensure removed elements are gone
    for (int i = 25; i < 75; i++) {
        ASSERT_FALSE(Map_contains(int, int, &m, i));
    }
    
    Map_free(int, int, &m);
}

// ========== MULTIPLE DATA TYPE TESTS ==========

TEST(map_int_to_string) {
    Map(int, string) m = Map_init(int, string);
    
    Map_insert(int, string, &m, 1, "one");
    Map_insert(int, string, &m, 2, "two");
    Map_insert(int, string, &m, 100, "hundred");
    
    string *val = Map_get(int, string, &m, 2);
    ASSERT_NOT_NULL(val);
    ASSERT_TRUE(strcmp(*val, "two") == 0);
    
    ASSERT_TRUE(Map_contains(int, string, &m, 100));
    ASSERT_FALSE(Map_contains(int, string, &m, 99));
    
    Map_free(int, string, &m);
}

TEST(map_multiple_types_coexist) {
    Map(string, int) m1 = Map_init(string, int);
    Map(int, string) m2 = Map_init(int, string);
    
    Map_insert(string, int, &m1, "key", 42);
    Map_insert(int, string, &m2, 42, "value");
    
    ASSERT_TRUE(*Map_get(string, int, &m1, "key") == 42);
    ASSERT_TRUE(strcmp(*Map_get(int, string, &m2, 42), "value") == 0);
    
    Map_free(string, int, &m1);
    Map_free(int, string, &m2);
}

// ========== STRING REPRESENTATION TESTS ==========

TEST(map_print_registered) {
    Map(string, int) m = Map_init(string, int);
    Map_insert(string, int, &m, "Hello", 4);
    Map_insert(string, int, &m, "World", 7);
    
    char *s = Map_to_str(string, int, &m);
    ASSERT_NOT_NULL(s);
    ASSERT_TRUE(strstr(s, "Hello") != NULL);
    ASSERT_TRUE(strstr(s, "World") != NULL);
    ASSERT_TRUE(strstr(s, "4") != NULL);
    ASSERT_TRUE(strstr(s, "7") != NULL);
    
    free(s);
    Map_free(string, int, &m);
}

TEST(map_to_str_custom) {
    Map(string, int) m = Map_init(string, int);
    Map_insert(string, int, &m, "X", 100);
    Map_insert(string, int, &m, "Y", 200);
    
    char *s = Map_to_str_custom(string, int, &m, str_to_str, int_to_str);
    ASSERT_NOT_NULL(s);
    ASSERT_TRUE(strstr(s, "\"X\"") != NULL);
    ASSERT_TRUE(strstr(s, "100") != NULL);
    ASSERT_TRUE(strstr(s, "\"Y\"") != NULL);
    ASSERT_TRUE(strstr(s, "200") != NULL);
    
    free(s);
    Map_free(string, int, &m);
}

TEST(map_empty_to_str) {
    Map(string, int) m = Map_init(string, int);
    char *s = Map_to_str(string, int, &m);
    ASSERT_NOT_NULL(s);
    ASSERT_TRUE(strcmp(s, "{}") == 0);
    free(s);
    Map_free(string, int, &m);
}

TEST(map_single_element_to_str) {
    Map(string, int) m = Map_init(string, int);
    Map_insert(string, int, &m, "only", 1);
    
    char *s = Map_to_str(string, int, &m);
    ASSERT_NOT_NULL(s);
    ASSERT_TRUE(strstr(s, "only") != NULL);
    ASSERT_TRUE(strstr(s, "1") != NULL);
    ASSERT_TRUE(s[0] == '{' && s[strlen(s)-1] == '}');
    
    free(s);
    Map_free(string, int, &m);
}

// ========== BOUNDARY AND ERROR TESTS ==========

TEST(map_zero_values) {
    Map(string, int) m = Map_init(string, int);
    Map_insert(string, int, &m, "zero", 0);
    
    ASSERT_TRUE(Map_contains(string, int, &m, "zero"));
    int *val = Map_get(string, int, &m, "zero");
    ASSERT_NOT_NULL(val);
    ASSERT_TRUE(*val == 0);
    
    Map_free(string, int, &m);
}

TEST(map_negative_values) {
    Map(int, int) m = Map_init(int, int);
    Map_insert(int, int, &m, -5, -100);
    Map_insert(int, int, &m, 0, 0);
    Map_insert(int, int, &m, 5, 100);
    
    ASSERT_TRUE(*Map_get(int, int, &m, -5) == -100);
    ASSERT_TRUE(*Map_get(int, int, &m, 0) == 0);
    ASSERT_TRUE(*Map_get(int, int, &m, 5) == 100);
    
    Map_free(int, int, &m);
}

TEST(map_duplicate_insert_behavior) {
    Map(string, int) m = Map_init(string, int);
    
    bool inserted1 = Map_insert(string, int, &m, "key", 1);
    bool inserted2 = Map_insert(string, int, &m, "key", 2);
    
    // First insertion should add new node, second should update existing
    ASSERT_TRUE(inserted1);   // new insertion
    ASSERT_FALSE(inserted2);  // update existing
    
    ASSERT_TRUE(Map_size(string, int, &m) == 1);
    ASSERT_TRUE(*Map_get(string, int, &m, "key") == 2);
    
    Map_free(string, int, &m);
}

// ========== RED-BLACK TREE PROPERTY TESTS ==========

// Validate red-black tree invariants
int validate_rb_map(MapNode_int_int *node, int black_count, int *expected_black_height)
{
    if (!node) {
        if (*expected_black_height == -1)
            *expected_black_height = black_count;
        return black_count == *expected_black_height;
    }
    
    if (node->color == MAP_BLACK_int_int)
        black_count++;
    
    // No red parent with red child
    if (node->color == MAP_RED_int_int) {
        if ((node->left && node->left->color == MAP_RED_int_int) ||
            (node->right && node->right->color == MAP_RED_int_int)) {
            return 0;
        }
    }
    
    return validate_rb_map(node->left, black_count, expected_black_height) &&
           validate_rb_map(node->right, black_count, expected_black_height);
}

TEST(red_black_invariants) {
    Map(int, int) m = Map_init(int, int);
    
    // Insert many random values
    srand(42); // Fixed seed for reproducible tests
    for (int i = 0; i < 200; i++) {
        Map_insert(int, int, &m, rand() % 1000, i);
    }
    
    // Remove some random values
    srand(42); // Reset seed
    for (int i = 0; i < 100; i++) {
        Map_remove(int, int, &m, rand() % 1000);
    }
    
    // Validate RB properties
    // 1. Root is black (or NULL)
    ASSERT_TRUE(m.root == NULL || m.root->color == MAP_BLACK_int_int);
    
    // 2. No red node has red children, and all paths have same black height
    int expected_black_height = -1;
    ASSERT_TRUE(validate_rb_map(m.root, 0, &expected_black_height));
    
    Map_free(int, int, &m);
}

TEST(map_ordered_insertion) {
    Map(int, int) m = Map_init(int, int);
    
    // Insert in ascending order (worst case for unbalanced BST)
    for (int i = 1; i <= 15; i++) {
        Map_insert(int, int, &m, i, i * 10);
    }
    
    // Should still be efficient due to red-black tree balancing
    ASSERT_TRUE(Map_size(int, int, &m) == 15);
    
    // Validate RB properties after sequential insertion
    ASSERT_TRUE(m.root->color == MAP_BLACK_int_int);
    int expected_black_height = -1;
    ASSERT_TRUE(validate_rb_map(m.root, 0, &expected_black_height));
    
    // Check all values are accessible
    for (int i = 1; i <= 15; i++) {
        int *val = Map_get(int, int, &m, i);
        ASSERT_NOT_NULL(val);
        ASSERT_TRUE(*val == i * 10);
    }
    
    Map_free(int, int, &m);
}

TEST(map_reverse_ordered_insertion) {
    Map(int, int) m = Map_init(int, int);
    
    // Insert in descending order
    for (int i = 15; i >= 1; i--) {
        Map_insert(int, int, &m, i, i * 10);
    }
    
    ASSERT_TRUE(Map_size(int, int, &m) == 15);
    
    // Validate RB properties after reverse sequential insertion
    ASSERT_TRUE(m.root->color == MAP_BLACK_int_int);
    int expected_black_height = -1;
    ASSERT_TRUE(validate_rb_map(m.root, 0, &expected_black_height));
    
    // Check all values
    for (int i = 1; i <= 15; i++) {
        ASSERT_TRUE(Map_contains(int, int, &m, i));
    }
    
    Map_free(int, int, &m);
}

TEST(map_rb_properties_after_removals) {
    Map(int, int) m = Map_init(int, int);
    
    // Insert elements
    for (int i = 1; i <= 31; i++) {
        Map_insert(int, int, &m, i, i);
    }
    
    // Remove every third element
    for (int i = 3; i <= 31; i += 3) {
        Map_remove(int, int, &m, i);
    }
    
    // Validate RB properties are maintained after removals
    ASSERT_TRUE(m.root == NULL || m.root->color == MAP_BLACK_int_int);
    int expected_black_height = -1;
    ASSERT_TRUE(validate_rb_map(m.root, 0, &expected_black_height));
    
    Map_free(int, int, &m);
}

// ========== TEST SUITE DEFINITION ==========

TEST_SUITE(
    // Basic functionality
    RUN_TEST(map_init_empty),
    RUN_TEST(map_insert_and_get),
    RUN_TEST(map_overwrite),
    RUN_TEST(map_contains),
    
    // Remove operations
    RUN_TEST(map_remove_existing),
    RUN_TEST(map_remove_nonexistent),
    RUN_TEST(map_remove_from_empty),
    RUN_TEST(map_remove_multiple),
    RUN_TEST(map_remove_root_node),
    
    // Clear operations
    RUN_TEST(map_clear),
    RUN_TEST(map_clear_empty),
    RUN_TEST(map_clear_and_reuse),
    
    // Get and null tests
    RUN_TEST(map_get_null_for_nonexistent),
    RUN_TEST(map_get_from_empty),
    
    // Stress tests
    RUN_TEST(map_large),
    RUN_TEST(map_stress_insert_remove),
    
    // Multiple data types
    RUN_TEST(map_int_to_string),
    RUN_TEST(map_multiple_types_coexist),
    
    // String representation
    RUN_TEST(map_print_registered),
    RUN_TEST(map_to_str_custom),
    RUN_TEST(map_empty_to_str),
    RUN_TEST(map_single_element_to_str),
    
    // Boundary cases
    RUN_TEST(map_zero_values),
    RUN_TEST(map_negative_values),
    RUN_TEST(map_duplicate_insert_behavior),
    
    // Red-black tree properties
    RUN_TEST(red_black_invariants),
    RUN_TEST(map_ordered_insertion),
    RUN_TEST(map_reverse_ordered_insertion),
    RUN_TEST(map_rb_properties_after_removals)
)