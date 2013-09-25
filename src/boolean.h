#ifndef JSON_BOOLEAN_H_
#define JSON_BOOLEAN_H_

#include "value.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	JSON_Value base__;
	bool value;
}
JSON_Boolean;

#define JSON_BOOLEAN(v)    ((JSON_Boolean*)(v))
#define JSON_TYPE_BOOLEAN  json_boolean_get_class()
#define JSON_IS_BOOLEAN(v) JSON_LIKELY(((v) != NULL) && (JSON_VALUE_CLASS(v) == JSON_TYPE_BOOLEAN))

void *json_boolean_get_class(void);
JSON_Boolean *json_boolean_true(void);
JSON_Boolean *json_boolean_false(void);
bool json_boolean_get(JSON_Boolean *b);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // JSON_BOOLEAN_H_
