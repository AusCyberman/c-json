#ifndef JSON_H
#define JSON_H
#include <stdint.h>
#include <sys/types.h>

typedef struct JsonArray JsonArray;
typedef struct JsonValue JsonValue;
typedef struct JsonString JsonString;
typedef struct JsonObject JsonObject;
enum json_type {
    JSON_OBJECT,
    JSON_ARRAY,
    JSON_STRING,
    JSON_NUMBER,
    JSON_BOOL,
    JSON_NULL
};

struct JsonValue {
    enum json_type tag;
    union {
        JsonObject *obj;
        JsonArray *array;
        JsonString *str;
        uint64_t number;
        char bool;
        char null;
    } value;
};

struct ObjectNode {
    char *key;
    JsonValue value;
};

struct JsonObject {
    size_t size;
    size_t cap;
    struct ObjectNode **buckets;
};

struct JsonString {
    size_t length;
    char *str;
};

struct JsonArray {
    size_t length;
    JsonValue *values;
};

JsonValue json_value_create();

JsonObject json_object_create();

#endif
