// tests/test_json.c
#include "CLIP/Test.h"
#include "CLIP/Parsers/json.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>

// ========== BASIC TYPE TESTS ==========

TEST(json_parse_null) {
    JsonValue *v = Json_parse("null");
    ASSERT_NOT_NULL(v);
    ASSERT_TRUE(v->type == JSON_NULL);
    Json_free(v);
}

TEST(json_parse_bool_true) {
    JsonValue *v = Json_parse("true");
    ASSERT_NOT_NULL(v);
    ASSERT_TRUE(v->type == JSON_BOOL);
    ASSERT_TRUE(v->boolean == 1);
    Json_free(v);
}

TEST(json_parse_bool_false) {
    JsonValue *v = Json_parse("false");
    ASSERT_NOT_NULL(v);
    ASSERT_TRUE(v->type == JSON_BOOL);
    ASSERT_TRUE(v->boolean == 0);
    Json_free(v);
}

TEST(json_parse_number) {
    JsonValue *v = Json_parse("123.45");
    ASSERT_NOT_NULL(v);
    ASSERT_TRUE(v->type == JSON_NUMBER);
    ASSERT_TRUE(fabs(v->number - 123.45) < 1e-9);;
    Json_free(v);
}

TEST(json_parse_string) {
    JsonValue *v = Json_parse("\"hello\"");
    ASSERT_NOT_NULL(v);
    ASSERT_TRUE(v->type == JSON_STRING);
    ASSERT_TRUE(strcmp(v->str, "hello") == 0);
    Json_free(v);
}

// ========== LIST TESTS ==========

TEST(json_parse_empty_list) {
    JsonValue *v = Json_parse("[]");
    ASSERT_NOT_NULL(v);
    ASSERT_TRUE(v->type == JSON_LIST);
    ASSERT_TRUE(v->list.size == 0);
    Json_free(v);
}

TEST(json_parse_simple_list) {
    JsonValue *v = Json_parse("[1, 2, 3]");
    ASSERT_NOT_NULL(v);
    ASSERT_TRUE(v->type == JSON_LIST);
    ASSERT_TRUE(v->list.size == 3);
    ASSERT_TRUE(v->list.data[0]->type == JSON_NUMBER);
    ASSERT_TRUE(fabs(v->list.data[0]->number - 1.0) < 1e-9);
    ASSERT_TRUE(v->list.data[1]->type == JSON_NUMBER);
    ASSERT_TRUE(fabs(v->list.data[1]->number - 2.0) < 1e-9);
    ASSERT_TRUE(v->list.data[2]->type == JSON_NUMBER);
    ASSERT_TRUE(fabs(v->list.data[2]->number - 3.0) < 1e-9);
    Json_free(v);
}

TEST(json_parse_nested_list) {
    JsonValue *v = Json_parse("[[true, false], null]");
    ASSERT_NOT_NULL(v);
    ASSERT_TRUE(v->type == JSON_LIST);
    ASSERT_TRUE(v->list.size == 2);

    JsonValue *inner = v->list.data[0];
    ASSERT_TRUE(inner->type == JSON_LIST);
    ASSERT_TRUE(inner->list.size == 2);
    ASSERT_TRUE(inner->list.data[0]->boolean == 1);
    ASSERT_TRUE(inner->list.data[1]->boolean == 0);

    JsonValue *n = v->list.data[1];
    ASSERT_TRUE(n->type == JSON_NULL);

    Json_free(v);
}

// ========== OBJECT TESTS ==========

TEST(json_parse_empty_object) {
    JsonValue *v = Json_parse("{}");
    ASSERT_NOT_NULL(v);
    ASSERT_TRUE(v->type == JSON_OBJECT);
    ASSERT_TRUE(Map_size(string, JsonValue_ptr, &v->object) == 0);
    Json_free(v);
}

TEST(json_parse_simple_object) {
    JsonValue *v = Json_parse("{\"a\": 1, \"b\": true}");
    ASSERT_NOT_NULL(v);
    ASSERT_TRUE(v->type == JSON_OBJECT);
    ASSERT_TRUE(Map_size(string, JsonValue_ptr, &v->object) == 2);

    JsonValue *va = *Map_get(string, JsonValue_ptr, &v->object, "a");
    ASSERT_NOT_NULL(va);
    ASSERT_TRUE(va->type == JSON_NUMBER);
    ASSERT_TRUE(fabs(va->number - 1.0) < 1e-9);

    JsonValue *vb = *Map_get(string, JsonValue_ptr, &v->object, "b");
    ASSERT_NOT_NULL(vb);
    ASSERT_TRUE(vb->boolean == 1);

    Json_free(v);
}

TEST(json_parse_nested_object) {
    JsonValue *v = Json_parse("{\"outer\": {\"inner\": [\"x\", \"y\"]}}");
    ASSERT_NOT_NULL(v);
    ASSERT_TRUE(v->type == JSON_OBJECT);

    JsonValue *inner_obj = *Map_get(string, JsonValue_ptr, &v->object, "outer");
    ASSERT_NOT_NULL(inner_obj);
    ASSERT_TRUE(inner_obj->type == JSON_OBJECT);

    JsonValue *inner_list = *Map_get(string, JsonValue_ptr, &inner_obj->object, "inner");
    ASSERT_NOT_NULL(inner_list);
    ASSERT_TRUE(inner_list->type == JSON_LIST);
    ASSERT_TRUE(inner_list->list.size == 2);
    ASSERT_TRUE(strcmp(inner_list->list.data[0]->str, "x") == 0);
    ASSERT_TRUE(strcmp(inner_list->list.data[1]->str, "y") == 0);

    Json_free(v);
}

// ========== MIXED COMPLEX TESTS ==========

TEST(json_parse_complex) {
    const char *json = "{ \"name\": \"Eduardo\", \"age\": 27, "
                       "\"skills\": [\"C\", \"C++\", \"Python\"], "
                       "\"active\": true, \"address\": null }";
    JsonValue *v = Json_parse(json);
    ASSERT_NOT_NULL(v);
    ASSERT_TRUE(v->type == JSON_OBJECT);

    JsonValue *name = *Map_get(string, JsonValue_ptr, &v->object, "name");
    ASSERT_NOT_NULL(name);
    ASSERT_TRUE(name->type == JSON_STRING);
    ASSERT_TRUE(strcmp(name->str, "Eduardo") == 0);

    JsonValue *age = *Map_get(string, JsonValue_ptr, &v->object, "age");
    ASSERT_NOT_NULL(age);
    ASSERT_TRUE(age->type == JSON_NUMBER);
    ASSERT_TRUE(fabs(age->number - 27.0) < 1e-9);

    JsonValue *skills = *Map_get(string, JsonValue_ptr, &v->object, "skills");
    ASSERT_NOT_NULL(skills);
    ASSERT_TRUE(skills->type == JSON_LIST);
    ASSERT_TRUE(skills->list.size == 3);
    ASSERT_TRUE(skills->list.data[0]->type == JSON_STRING);
    ASSERT_TRUE(strcmp(skills->list.data[0]->str, "C") == 0);
    ASSERT_TRUE(strcmp(skills->list.data[1]->str, "C++") == 0);
    ASSERT_TRUE(strcmp(skills->list.data[2]->str, "Python") == 0);

    JsonValue *active = *Map_get(string, JsonValue_ptr, &v->object, "active");
    ASSERT_NOT_NULL(active);
    ASSERT_TRUE(active->type == JSON_BOOL);
    ASSERT_TRUE(active->boolean == 1);

    JsonValue *addr = *Map_get(string, JsonValue_ptr, &v->object, "address");
    ASSERT_NOT_NULL(addr);
    ASSERT_TRUE(addr->type == JSON_NULL);

    Json_free(v);
}



// ========== TEST SUITE DEFINITION ==========

TEST_SUITE(
    RUN_TEST(json_parse_null),
    RUN_TEST(json_parse_bool_true),
    RUN_TEST(json_parse_bool_false),
    RUN_TEST(json_parse_number),
    RUN_TEST(json_parse_string),

    RUN_TEST(json_parse_empty_list),
    RUN_TEST(json_parse_simple_list),
    RUN_TEST(json_parse_nested_list),

    RUN_TEST(json_parse_empty_object),
    RUN_TEST(json_parse_simple_object),
    RUN_TEST(json_parse_nested_object),

    RUN_TEST(json_parse_complex)
)
