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

typedef struct JSON_Value_ JSON_Value;
typedef struct JSON_String_ JSON_String;
typedef void (*JSON_FreeFunc)(JSON_Value*);
typedef JSON_Value* (*JSON_CloneFunc)(JSON_Value*);
typedef bool (*JSON_EqualFunc)(const JSON_Value*, const JSON_Value*);
typedef JSON_String* (*JSON_ToStringFunc)(JSON_Value*, int);

enum JSON_ValueFlag
{
	JSON_VALUE_FLAG_NONE     = (1<<0),
	JSON_VALUE_FLAG_FLOATING = (1<<1),
	JSON_VALUE_FLAG_ON_HEAP  = (1<<2),
};

struct JSON_Value_
{
	void *class__;
	int flags;
	uint32_t ref_count;
};

typedef struct
{
	size_t size;
	JSON_FreeFunc free;
	JSON_CloneFunc clone;
	JSON_EqualFunc equal;
	JSON_ToStringFunc to_string;
}
JSON_ValueClass;

#define JSON_VALUE(v)       ((JSON_Value*)(v))
#define JSON_VALUE_CLASS(v) ((JSON_ValueClass*)(JSON_VALUE(v)->class__))

JSON_INTERNAL_FUNC
void *json_value_alloc(void *class_);

void *json_value_init(void *class_, JSON_Value *value);

void *json_value_clone(const void *v);
bool json_value_equal(const void *v1, const void *v2);
JSON_String *json_value_to_string(const void *v, int indent);

void *json_value_ref(void *v);
void *json_value_ref_sink(void *v);
void *json_value_unref(void *v);

void json_value_unref_many(void *v, ...);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // JSON_VALUE_H_
