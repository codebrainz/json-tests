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
#define JSON_IS_BOOLEAN(v) JSON_LIKELY(((v) != NULL) && (JSON_VALUE(v)->type == JSON_VALUE_TYPE_BOOLEAN))

JSON_Boolean *json_boolean_new(bool value);
bool json_boolean_get(JSON_Boolean *b);
void json_boolean_set(JSON_Boolean *b, bool value);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // JSON_BOOLEAN_H_
