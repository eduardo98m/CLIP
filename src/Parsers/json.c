// src/Parsers/Json.c
#include <CLIP/Parsers/json.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

// --- Utility ---
static void skip_ws(string *s)
{
    while (isspace((unsigned char)**s))
        (*s)++;
}

// --- Constructors ---
static JsonValue *new_value(JsonType t)
{
    JsonValue *v = malloc(sizeof(JsonValue));
    v->type = t;
    return v;
}

// --- Parse entrypoint ---
JsonValue *Json_parse(const string input)
{
    string s = input;
    skip_ws(&s);
    return parse_value(&s);
}

// --- Parse value ---
static JsonValue *parse_value(string *s)
{
    skip_ws(s);
    if (**s == '{')
        return parse_object(s);
    if (**s == '[')
        return parse_array(s);
    if (**s == '"')
        return parse_string(s);
    if (strncmp(*s, "true", 4) == 0)
        return parse_literal(s, "true", JSON_BOOL, 1);
    if (strncmp(*s, "false", 5) == 0)
        return parse_literal(s, "false", JSON_BOOL, 0);
    if (strncmp(*s, "null", 4) == 0)
        return parse_literal(s, "null", JSON_NULL, 0);
    return parse_number(s);
}

// --- Object ---
static JsonValue *parse_object(string *s)
{
    (*s)++; // skip '{'
    JsonValue *obj = new_value(JSON_OBJECT);
    obj->object = Map_init(string, JsonValue_ptr);

    skip_ws(s);
    if (**s == '}')
    {
        (*s)++;
        return obj;
    }

    while (**s)
    {
        skip_ws(s);
        JsonValue *key = parse_string(s);
        skip_ws(s);
        if (**s != ':')
        {
            fprintf(stderr, "Expected ':'\n");
            exit(1);
        }
        (*s)++;
        JsonValue *val = parse_value(s);
        Map_insert(string, JsonValue_ptr, &obj->object, key->str, val);
        free(key); // free temporary key JsonValue (string memory is now owned by Map)

        skip_ws(s);
        if (**s == '}')
        {
            (*s)++;
            break;
        }
        if (**s == ',')
        {
            (*s)++;
            continue;
        }
        fprintf(stderr, "Expected ',' or '}'\n");
        exit(1);
    }
    return obj;
}

// --- Array ---
static JsonValue *parse_array(string *s)
{
    (*s)++; // skip '['
    JsonValue *arr = new_value(JSON_ARRAY);
    arr->array = List_init(JsonValue_ptr, 4);

    skip_ws(s);
    if (**s == ']')
    {
        (*s)++;
        return arr;
    }

    while (**s)
    {
        JsonValue *val = parse_value(s);
        List_append(JsonValue_ptr, &arr->array, val);
        skip_ws(s);
        if (**s == ']')
        {
            (*s)++;
            break;
        }
        if (**s == ',')
        {
            (*s)++;
            continue;
        }
        fprintf(stderr, "Expected ',' or ']'\n");
        exit(1);
    }
    return arr;
}

// --- String ---
static JsonValue *parse_string(string *s)
{
    (*s)++; // skip opening "
    string start = *s;
    while (**s && **s != '"')
    {
        if (**s == '\\')
            (*s)++; // skip escaped char
        (*s)++;
    }
    size_t len = *s - start;
    char *buf = malloc(len + 1);
    strncpy(buf, start, len);
    buf[len] = '\0';
    if (**s == '"')
        (*s)++;

    JsonValue *strv = new_value(JSON_STRING);
    strv->str = buf;
    return strv;
}

// --- Number ---
static JsonValue *parse_number(string *s)
{
    string start = *s;
    while (isdigit((unsigned char)**s) || **s == '.' || **s == '-' || **s == '+' || **s == 'e' || **s == 'E')
    {
        (*s)++;
    }

    size_t len = *s - start;
    char buf[128]; // enough for most numbers, avoids heap alloc
    if (len >= sizeof(buf))
    {
        // fallback if number too long
        char *heapbuf = strndup(start, len);
        JsonValue *num = new_value(JSON_NUMBER);
        num->number = strtod(heapbuf, NULL);
        free(heapbuf);
        return num;
    }

    memcpy(buf, start, len);
    buf[len] = '\0';

    JsonValue *num = new_value(JSON_NUMBER);
    num->number = strtod(buf, NULL);
    return num;
}

// --- Literal ---
static JsonValue *parse_literal(string *s, string literal, JsonType type, int bool_val)
{
    size_t len = strlen(literal);
    *s += len;
    JsonValue *v = new_value(type);
    if (type == JSON_BOOL)
        v->boolean = bool_val;
    return v;
}

// --- Free helpers ---
static void free_object_entry(string *k, JsonValue_ptr *v, void *ud)
{
    (void)ud;
    free((char *)*k); // free the strdup’d key
    Json_free(*v);    // free the value recursively
}

void Json_free(JsonValue* v) {
    if (!v) return;
    switch (v->type) {
        case JSON_STRING:
            free((char*)v->str);
            break;
        case JSON_OBJECT:
            Map_foreach(string, JsonValue_ptr, &v->object, free_object_entry, NULL);
            Map_free(string, JsonValue_ptr, &v->object);
            break;
        case JSON_ARRAY:
            for (size_t i = 0; i < v->array.size; i++) {
                Json_free(List_get(JsonValue_ptr, &v->array, i));
            }
            List_free(JsonValue_ptr, &v->array);
            break;
        default:
            break;
    }
    free(v);
}
