#include "string.h"
#include "util.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

static void json_string_free(JSON_Value *str)
{
	assert(str);
	json_free(JSON_STRING(str)->str);
}

static JSON_Value *json_string_clone(JSON_Value *str)
{
	JSON_String *new_str;
	assert(str);
	assert(JSON_IS_STRING(str));
	new_str = json_string_new(json_string_cstr(JSON_STRING(str)));
	return JSON_VALUE(new_str);
}

static bool json_string_equal(const JSON_Value *value1, const JSON_Value *value2)
{
	JSON_String *str1, *str2;

	assert(JSON_IS_STRING(value1));
	assert(JSON_IS_STRING(value2));

	str1 = JSON_STRING(value1);
	str2 = JSON_STRING(value2);

	if (!str1->str || !str2->str)
		return false;

	return json_strequal(str1->str, str2->str);
}

static JSON_String *json_string_to_string(JSON_Value *value, int indent)
{
	JSON_String *str;
	char *indent_str;

	assert(JSON_IS_STRING(value));

	indent_str = json_make_indent_string(indent);
	str = json_string_new_printf("%s\"%s\"", indent_str,
		json_string_cstr(JSON_STRING(value)));
	json_free(indent_str);

	return str;
}

JSON_String *json_string_new(const char *str)
{
	size_t len = 0;
	if (str != NULL)
		len = strlen(str);
	return json_string_new_length(str, len);
}

JSON_String *json_string_new_length(const char *str, size_t len)
{
	JSON_String *s = json_value_alloc(JSON_TYPE_STRING);
	if (s)
	{
		if (!str || !len)
		{
			s->len = 0;
			s->str = json_strdup("");
		}
		else
		{
			s->len = len;
			s->str = json_strndup(str, s->len);
		}
	}
	return s;
}

JSON_String *json_string_new_printf(const char *fmt, ...)
{
	JSON_String *str;
	char *temp;
	va_list ap;
	va_start(ap, fmt);
	temp = json_strvprintf(fmt, ap);
	va_end(ap);
	str = json_string_new(temp);
	json_free(temp);
	return str;
}

JSON_String *json_string_new_vprintf(const char *fmt, va_list ap_in)
{
	JSON_String *str;
	char *temp;
	va_list ap;
	va_copy(ap, ap_in);
	temp = json_strvprintf(fmt, ap);
	va_end(ap);
	str = json_string_new(temp);
	json_free(temp);
	return str;
}

size_t json_string_length(JSON_String *str)
{
	assert(str);
	return str->len;
}

JSON_String *json_string_init(JSON_String *str)
{
	assert(str != NULL);
	json_value_init(JSON_TYPE_STRING, JSON_VALUE(str));
	str->len = 0;
	str->str = json_strdup("");
	return str;
}

const char *json_string_cstr(JSON_String *str)
{
	assert(str);
	return str->str;
}

void json_string_assign(JSON_String *str, const char *s)
{
	size_t len = 0;
	assert(str);
	if (s != NULL)
		len = strlen(s);
	json_string_assign_length(str, s, len);
}

void json_string_assign_length(JSON_String *str, const char *s, size_t len)
{
	char *tmp;
	assert(str);
	if (s && len)
	{
		tmp = json_realloc(str->str, len + 1);
		if (tmp != NULL)
		{
			str->str = tmp;
			str->len = len;
			strncpy(str->str, s, len);
			str->str[str->len] = '\0';
		}
	}
	else
	{
		tmp = json_realloc(str->str, 1);
		if (tmp != NULL)
		{
			str->str = tmp;
			str->len = 0;
			str->str[str->len] = '\0';
		}
	}
}

void json_string_assign_printf(JSON_String *str, const char *fmt, ...)
{
	char *temp;
	va_list ap;
	assert(JSON_IS_STRING(str));
	va_start(ap, fmt);
	temp = json_strvprintf(fmt, ap);
	va_end(ap);
	json_string_assign(str, temp);
	json_free(temp);
}

void json_string_assign_vprintf(JSON_String *str, const char *fmt, va_list ap_in)
{
	char *temp;
	va_list ap;
	assert(JSON_IS_STRING(str));
	va_copy(ap, ap_in);
	temp = json_strvprintf(fmt, ap);
	va_end(ap);
	json_string_assign(str, temp);
	json_free(temp);
}

void json_string_append(JSON_String *str, JSON_String *str2)
{
	assert(str2);
	json_string_append_cstr_length(str, str2->str, str2->len);
}

void json_string_append_cstr(JSON_String *str, const char *str2)
{
	assert(str2);
	json_string_append_cstr_length(str, str2, strlen(str2));
}

void json_string_append_cstr_length(JSON_String *str, const char *str2, size_t len)
{
	size_t new_len;
	char *tmp;

	assert(JSON_IS_STRING(str));
	assert(str2);

	if (len == 0)
		return;

	new_len = str->len + len;
	tmp = json_realloc(str->str, new_len + 1);
	if (tmp != NULL)
	{
		str->str = tmp;
		strncpy(str->str + str->len, str2, len);
		str->len = new_len;
		str->str[str->len] = '\0';
	}
}

void json_string_append_char(JSON_String *str, char c)
{
	char temp[2] = {0};
	temp[0] = c;
	json_string_append_cstr_length(str, temp, 1);
}

void json_string_append_printf(JSON_String *str, const char *fmt, ...)
{
	char *temp;
	va_list ap;
	assert(JSON_IS_STRING(str));
	va_start(ap, fmt);
	temp = json_strvprintf(fmt, ap);
	va_end(ap);
	json_string_append_cstr(str, temp);
	json_free(temp);
}

void json_string_append_vprintf(JSON_String *str, const char *fmt, va_list ap_in)
{
	char *temp;
	va_list ap;
	assert(JSON_IS_STRING(str));
	va_copy(ap, ap_in);
	temp = json_strvprintf(fmt, ap);
	va_end(ap);
	json_string_append_cstr(str, temp);
	json_free(temp);
}

void json_string_prepend(JSON_String *str, JSON_String *str2)
{
	assert(str2);
	json_string_prepend_cstr_length(str, str2->str, str2->len);
}

void json_string_prepend_cstr(JSON_String *str, const char *str2)
{
	assert(str2);
	json_string_prepend_cstr_length(str, str2, strlen(str2));
}

void json_string_prepend_cstr_length(JSON_String *str, const char *str2, size_t len)
{
	size_t new_len;
	char *tmp;

	assert(JSON_IS_STRING(str));
	assert(str2);

	if (len == 0)
		return;

	new_len = str->len + len;
	tmp = json_realloc(str->str, new_len + 1);
	if (tmp != NULL)
	{
		str->str = tmp;
		memmove(str->str + len, str->str, new_len - len);
		memcpy(str->str, str2, len);
		str->len = new_len;
		str->str[str->len] = '\0';
	}
}

void json_string_prepend_char(JSON_String *str, char c)
{
	char temp[2] = {0};
	temp[0] = c;
	json_string_prepend_cstr_length(str, temp, 1);
}

void json_string_prepend_printf(JSON_String *str, const char *fmt, ...)
{
	char *temp;
	va_list ap;
	assert(JSON_IS_STRING(str));
	va_start(ap, fmt);
	temp = json_strvprintf(fmt, ap);
	va_end(ap);
	json_string_prepend_cstr(str, temp);
	json_free(temp);
}

void json_string_prepend_vprintf(JSON_String *str, const char *fmt, va_list ap_in)
{
	char *temp;
	va_list ap;
	assert(JSON_IS_STRING(str));
	va_copy(ap, ap_in);
	temp = json_strvprintf(fmt, ap);
	va_end(ap);
	json_string_prepend_cstr(str, temp);
	json_free(temp);
}

JSON_String *json_string_lstrip(JSON_String *str)
{
	char *start;

	assert(JSON_IS_STRING(str));

	for (start = str->str; *start && isspace(*start); start++)
		;

	str->len = strlen(start);
	memmove(str->str, start, str->len + 1);

	return str;
}

JSON_String *json_string_rstrip(JSON_String *str)
{
	size_t len;
	char *temp;

	assert(JSON_IS_STRING(str));

	len = str->len;
	while (--len)
	{
		if (isspace(str->str[len]))
			str->str[len] = '\0';
		else
			break;
	}

	temp = json_realloc(str->str, len + 1);
	if (temp != NULL)
	{
		str->str = temp;
		str->len = len;
		str->str[str->len] = '\0';
	}

	return str;
}

struct JSON_StringClass
{
	JSON_ValueClass base__;
};

void *json_string_get_class(void)
{
	static struct JSON_StringClass json_string_class = { {
		sizeof(JSON_String),
		json_string_free,
		json_string_clone,
		json_string_equal,
		json_string_to_string,
	} };
	return &json_string_class;
}
