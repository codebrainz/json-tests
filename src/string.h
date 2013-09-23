#ifndef JSON_STRING_H_
#define JSON_STRING_H_

#include "value.h"

#ifdef __cplusplus
extern "C" {
#endif

struct JSON_String_
{
	JSON_Value base__;
	char *str;
	size_t len;
	size_t reserved__;
};

#define JSON_STRING(v)    ((JSON_String*)(v))
#define JSON_IS_STRING(v) JSON_LIKELY(((v) != NULL) && (JSON_VALUE(v)->type == JSON_VALUE_TYPE_STRING))

JSON_String *json_string_new(const char *str);
JSON_String *json_string_new_length(const char *str, size_t len);

size_t json_string_length(JSON_String *str);
const char *json_string_cstr(JSON_String *str);
void json_string_set_cstr(JSON_String *str, const char *s);
void json_string_set_cstr_length(JSON_String *str, const char *s, size_t len);

void json_string_append(JSON_String *str, JSON_String *str2);
void json_string_append_cstr(JSON_String *str, const char *str2);
void json_string_append_cstr_length(JSON_String *str, const char *str2, size_t len);
void json_string_prepend(JSON_String *str, JSON_String *str2);
void json_string_prepend_cstr(JSON_String *str, const char *str2);
void json_string_prepend_cstr_length(JSON_String *str, const char *str2, size_t len);

JSON_String *json_string_lstrip(JSON_String *str);
JSON_String *json_string_rstrip(JSON_String *str);

#define json_string_strip(str) \
	json_string_rstrip(json_string_lstrip(JSON_STRING(str)))

#ifdef __cplusplus
} // extern "C"
#endif

#endif // JSON_STRING_H_
