#include "string.h"
#include "util.h"
#include <string.h>
#include <ctype.h>

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

static int json_string_compare(JSON_Value *value1, JSON_Value *value2)
{
	JSON_String *str1, *str2;

	assert(JSON_IS_STRING(value1));
	assert(JSON_IS_STRING(value2));

	str1 = JSON_STRING(value1);
	str2 = JSON_STRING(value2);

	if (str1->str == NULL && str2->str != NULL)
		return -1;
	else if (str1->str != NULL && str2->str == NULL)
		return 1;
	else if (str1->str == NULL && str2->str == NULL)
		return 0;
	else
		return strcmp(str1->str, str2->str);
}

static JSON_String *json_string_to_string(JSON_Value *value, int indent)
{
	JSON_String *str;
	assert(JSON_IS_STRING(value));
	str = json_string_new("");
	if (indent > 0)
	{
		char *indent_str = json_make_indent_string(indent);
		json_string_append_cstr(str, indent_str);
		json_free(indent_str);
	}
	json_string_append_cstr(str, "\"");
	json_string_append(str, JSON_STRING(value));
	json_string_append_cstr(str, "\"");
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
	JSON_Value *value = json_value_alloc_size(sizeof(JSON_String),
		JSON_VALUE_TYPE_STRING, json_string_free, json_string_clone,
		json_string_compare, json_string_to_string);
	JSON_String *s = JSON_STRING(value);
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

size_t json_string_length(JSON_String *str)
{
	assert(str);
	return str->len;
}

const char *json_string_cstr(JSON_String *str)
{
	assert(str);
	return str->str;
}

void json_string_set_cstr(JSON_String *str, const char *s)
{
	size_t len = 0;
	assert(str);
	if (s != NULL)
		len = strlen(s);
	json_string_set_cstr_length(str, s, len);
}

void json_string_set_cstr_length(JSON_String *str, const char *s, size_t len)
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
