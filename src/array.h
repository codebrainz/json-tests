#ifndef JSON_ARRAY_H_
#define JSON_ARRAY_H_

#include "value.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	JSON_Value base__;
	JSON_Value **array;
	size_t size;
	size_t reserved__;
}
JSON_Array;

#define JSON_ARRAY(v)    ((JSON_Array*)(v))
#define JSON_IS_ARRAY(v) JSON_LIKELY(((v) != NULL) && (JSON_VALUE(v)->type == JSON_VALUE_TYPE_ARRAY))

JSON_Array *json_array_new(void);
size_t json_array_size(JSON_Array *arr);
JSON_Value *json_array_nth(JSON_Array *arr, size_t n);
bool json_array_insert(JSON_Array *arr, JSON_Value *value, size_t pos);
void json_array_remove_nth(JSON_Array *arr, size_t pos);

#define json_array_prepend(arr, value) \
	json_array_insert(JSON_ARRAY(arr), JSON_VALUE(value), 0)

#define json_array_append(arr, value) \
	json_array_insert(JSON_ARRAY(arr), JSON_VALUE(value), \
		json_array_size(JSON_ARRAY(arr)))

#ifdef __cplusplus
} // extern "C"
#endif

#endif // JSON_ARRAY_H_
