#ifndef JSON_OBJECT_H_
#define JSON_OBJECT_H_

#include "value.h"
#include "string.h"
#include "array.h"

#ifdef __cplusplus
extern "C" {
#endif

struct JSON_BucketLink
{
	char *key;
	JSON_Value *value;
	struct JSON_BucketLink *next;
};

typedef struct
{
	JSON_Value base__;
	struct JSON_BucketLink **buckets;
	size_t num_buckets;
	size_t num_elements;
}
JSON_Object;

#define JSON_OBJECT(v)    ((JSON_Object*)(v))
#define JSON_TYPE_OBJECT  json_object_get_class()
#define JSON_IS_OBJECT(v) JSON_LIKELY(((v) != NULL) && (JSON_VALUE_CLASS(v) == JSON_TYPE_OBJECT))

void *json_object_get_class(void);
JSON_Object *json_object_new(void);
JSON_Object *json_object_init(JSON_Object *obj);

JSON_Value *json_object_get(JSON_Object *obj, const char *key);
bool json_object_set_value(JSON_Object *obj, const char *key, JSON_Value *value);
bool json_object_del(JSON_Object *obj, const char *key);

#define json_object_set(obj, key, value) \
	json_object_set_value(JSON_OBJECT(obj), key, JSON_VALUE(value))

bool json_object_rehash(JSON_Object *obj);

// TODO: remove this
void json_object_debug_hash(JSON_Object *obj);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // JSON_OBJECT_H_
