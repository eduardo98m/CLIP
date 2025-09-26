// include/CLIP/Parsers/Json.h
#ifndef CLIP_PARSERS_JSON_H
#define CLIP_PARSERS_JSON_H
#include <CLIP/Map.h>
#include <CLIP/List.h>

typedef const char *string;
typedef struct JsonValue JsonValue;
typedef JsonValue *JsonValue_ptr;

typedef enum
{
    JSON_NULL,
    JSON_BOOL,
    JSON_NUMBER,
    JSON_STRING,
    JSON_OBJECT,
    JSON_ARRAY
} JsonType;

CLIP_DEFINE_LIST_TYPE(JsonValue_ptr);

static inline int clip_json_cmp_str(const string *a, const string *b)
{
    return strcmp(*a, *b);
};

CLIP_DEFINE_MAP_TYPE(string, JsonValue_ptr, clip_json_cmp_str);

struct JsonValue
{
    JsonType type;
    union
    {
        string str;                        // for JSON_STRING, JSON_NUMBER (kept as string)
        int boolean;                       // for JSON_BOOL
        Map(string, JsonValue_ptr) object; // for JSON_OBJECT
        List(JsonValue_ptr) array;         // for JSON_ARRAY
    };
};

JsonValue_ptr Json_parse(const string input);
void Json_free(JsonValue_ptr v);
static JsonValue_ptr parse_value(string *s);
static JsonValue_ptr parse_object(string *s);
static JsonValue_ptr parse_array(string *s);
static JsonValue_ptr parse_string(string *s);
static JsonValue_ptr parse_number(string *s);
static JsonValue_ptr parse_literal(string *s, string literal, JsonType type, int bool_val);

#endif // CLIP_PARSERS_JSON_H
