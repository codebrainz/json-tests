#ifndef JSON_OBJECT_H_
#define JSON_OBJECT_H_

#include "value.h"
#include "string.h"
#include "array.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	JSON_Value base__;

	// For now just using two parallel arrays
	JSON_Array *keys;
	JSON_Array *values;

	// Save room for doing a proper hash table or RB tree or some such
	void *reserved0;
	void *reserved1;
	void *reserved2;
	void *reserved3;
	void *reserved4;
	void *reserved5;
	void *reserved6;
	void *reserved7;
}
JSON_Object;

#define JSON_OBJECT(v)    ((JSON_Object*)(v))
#define JSON_IS_OBJECT(v) JSON_LIKELY(((v) != NULL) && (JSON_VALUE(v)->type == JSON_VALUE_TYPE_OBJECT))

JSON_Object *json_object_new(void);

JSON_Value *json_object_find(JSON_Object *obj, const char *key);
void json_object_insert(JSON_Object *obj, const char *key, JSON_Value *value);
bool json_object_remove(JSON_Object *obj, const char *key);

#define json_object_get(obj, key)        ((void*) json_object_find(JSON_OBJECT(obj), key))
#define json_object_set(obj, key, value) json_object_insert(JSON_OBJECT(obj), key, JSON_VALUE(value))
#define json_object_del(obj, key)        json_object_remove(JSON_OBJECT(obj), key)

#ifdef __cplusplus
} // extern "C"
#endif

#endif // JSON_OBJECT_H_
