#ifndef JSON_STRING_H_
#define JSON_STRING_H_

#include "value.h"
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

struct JSON_String_
{
	JSON_Value base__;
	char *str;
	uint32_t len;
	uint32_t reserved__;
};

#define JSON_STRING(v)    ((JSON_String*)(v))
#define JSON_TYPE_STRING  json_string_get_class()
#define JSON_IS_STRING(v) JSON_LIKELY(((v) != NULL) && (JSON_VALUE_CLASS(v) == JSON_TYPE_STRING))

void *json_string_get_class(void);
JSON_String *json_string_new(const char *str);
JSON_String *json_string_new_length(const char *str, size_t len);
JSON_String *json_string_new_printf(const char *fmt, ...);
JSON_String *json_string_new_vprintf(const char *fmt, va_list ap);
JSON_String *json_string_init(JSON_String *str);

size_t json_string_length(JSON_String *str);
const char *json_string_cstr(JSON_String *str);
void json_string_assign(JSON_String *str, const char *s);
void json_string_assign_length(JSON_String *str, const char *s, size_t len);
void json_string_assign_printf(JSON_String *str, const char *fmt, ...);
void json_string_assign_vprintf(JSON_String *str, const char *fmt, va_list ap);

void json_string_append(JSON_String *str, JSON_String *str2);
void json_string_append_cstr(JSON_String *str, const char *str2);
void json_string_append_cstr_length(JSON_String *str, const char *str2, size_t len);
void json_string_append_char(JSON_String *str, char c);
void json_string_append_printf(JSON_String *str, const char *fmt, ...);
void json_string_append_vprintf(JSON_String *str, const char *fmt, va_list ap);

void json_string_prepend(JSON_String *str, JSON_String *str2);
void json_string_prepend_cstr(JSON_String *str, const char *str2);
void json_string_prepend_cstr_length(JSON_String *str, const char *str2, size_t len);
void json_string_prepend_char(JSON_String *str, char c);
void json_string_prepend_printf(JSON_String *str, const char *fmt, ...);
void json_string_prepend_vprintf(JSON_String *str, const char *fmt, va_list ap);

JSON_String *json_string_lstrip(JSON_String *str);
JSON_String *json_string_rstrip(JSON_String *str);

#define json_string_strip(str) \
	json_string_rstrip(json_string_lstrip(JSON_STRING(str)))

#define json_string_endswith(str, chr) \
	((chr) == JSON_STRING(str)->str[JSON_STRING(str)->len - 1])

#ifdef __cplusplus
} // extern "C"
#endif

#endif // JSON_STRING_H_
