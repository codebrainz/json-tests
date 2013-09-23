#ifndef JSON_NUMBER_H_
#define JSON_NUMBER_H_

#include "value.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	JSON_Value base__;
	double value;
}
JSON_Number;

#define JSON_NUMBER(v)    ((JSON_Number*)(v))
#define JSON_IS_NUMBER(v) JSON_LIKELY(((v) != NULL) && (JSON_VALUE(v)->type == JSON_VALUE_TYPE_NUMBER))

JSON_Number *json_number_new(double value);
double json_number_get(JSON_Number *num);
void json_number_set(JSON_Number *num, double value);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // JSON_NUMBER_H_
