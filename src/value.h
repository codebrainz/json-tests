#ifndef JSON_VALUE_H_
#define JSON_VALUE_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __GNUC__
# define JSON_INTERNAL_FUNC __attribute__((visibility("internal")))
# define JSON_API_FUNC      __attribute__((visibility("default")))
# define JSON_LIKELY(x)     __builtin_expect(!!(x), 1)
# define JSON_UNLIKELY(x)   __builtin_expect(!!(x), 0)
#else // TODO: hand dllexport or whatever else
# define JSON_INTERNAL_FUNC
# define JSON_API_FUNC
# define JSON_LIKELY(x) (x)
# define JSON_UNLIKELY(x) (x)
#endif

#define JSON_INDENT_WIDTH 2

typedef enum
{
	// Actual JSON values
	JSON_VALUE_TYPE_NULL=0,
	JSON_VALUE_TYPE_BOOLEAN,
	JSON_VALUE_TYPE_NUMBER,
	JSON_VALUE_TYPE_STRING,
	JSON_VALUE_TYPE_ARRAY,
	JSON_VALUE_TYPE_OBJECT,
	// Internal stuff
	JSON_VALUE_TYPE_LEXER,
	// Add more here ...
	JSON_VALUE_MAX_TYPE
}
JSON_ValueType;

typedef struct JSON_Value_ JSON_Value;
typedef struct JSON_String_ JSON_String;
typedef void (*JSON_FreeFunc)(JSON_Value*);
typedef JSON_Value* (*JSON_CloneFunc)(JSON_Value*);
typedef int (*JSON_CompareFunc)(JSON_Value*, JSON_Value*);
typedef JSON_String* (*JSON_ToStringFunc)(JSON_Value*, int);

struct JSON_Value_
{
	JSON_ValueType type;
	size_t ref_count;
	JSON_FreeFunc free;
	JSON_CloneFunc clone;
	JSON_CompareFunc compare;
	JSON_ToStringFunc to_string;
};

#define JSON_VALUE(v)      ((JSON_Value*)(v))
#define JSON_VALUE_TYPE(v) (JSON_LIKELY((v) != NULL) ? JSON_VALUE(v)->type : JSON_VALUE_TYPE_NULL)

#define JSON_IS_VALUE(v) (JSON_LIKELY((v) != NULL) && \
	((int)JSON_VALUE(v)->type >= (int)JSON_VALUE_TYPE_NULL || \
	 (int)JSON_VALUE(v)->type < (int)JSON_VALUE_MAX_TYPE))

JSON_INTERNAL_FUNC
void* json_value_alloc_size(size_t sz, JSON_ValueType type,
	JSON_FreeFunc free_func, JSON_CloneFunc clone_func,
	JSON_CompareFunc compare_func, JSON_ToStringFunc to_string_func);

void *json_value_clone(const void *v);
int json_value_compare(const void *v1, const void *v2);
JSON_String *json_value_to_string(const void *v, int indent);

void *json_value_ref(void *v);
void *json_value_unref(void *v);

void json_value_unref_many(void *v, ...);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // JSON_VALUE_H_
