#ifndef JSON_NULL_H_
#define JSON_NULL_H_

#include "value.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	JSON_Value base__;
}
JSON_Null;

#define JSON_NULL(v)    ((JSON_Null*)(v))
#define JSON_IS_NULL(v) JSON_LIKELY(((v) != NULL) && (JSON_VALUE(v)->type == JSON_VALUE_TYPE_NULL))

JSON_Null *json_null_new(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // JSON_NULL_H_
