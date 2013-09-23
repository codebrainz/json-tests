#include "null.h"
#include "util.h"
#include "string.h"

static JSON_Value *json_null_clone(JSON_Value *n)
{
	assert(n);
#ifdef NDEBUG
	(void)n;
#endif
	return JSON_VALUE(json_null_new());
}

static int json_null_compare(JSON_Value *n1, JSON_Value *n2)
{ // FIXME
	assert(JSON_IS_NULL(n1));
	assert(JSON_IS_NULL(n2));
#ifdef NDEBUG
	(void)n1;
	(void)n2;
#endif
	return 0;
}

static JSON_String *json_null_to_string(JSON_Value *n, int indent)
{
	JSON_String *str;
	char *indent_str;

	assert(JSON_IS_NULL(n));
	(void)n;

	indent_str = json_make_indent_string(indent);
	str = json_string_new(indent_str);
	json_free(indent_str);

	json_string_append_cstr(str, "null");

	return str;
}

JSON_Null *json_null_new(void)
{
	JSON_Value *value = json_value_alloc_size(sizeof(JSON_Null),
		JSON_VALUE_TYPE_NULL, NULL, json_null_clone, json_null_compare,
		json_null_to_string);
	return JSON_NULL(value);
}
