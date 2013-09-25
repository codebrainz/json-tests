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
#define JSON_TYPE_NUMBER  json_number_get_class()
#define JSON_IS_NUMBER(v) JSON_LIKELY(((v) != NULL) && (JSON_VALUE_CLASS(v) == JSON_TYPE_NUMBER))

void *json_number_get_class(void);
JSON_Number *json_number_new(double value);
JSON_Number *json_number_init(JSON_Number *num);
double json_number_get(JSON_Number *num);
void json_number_set(JSON_Number *num, double value);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // JSON_NUMBER_H_
