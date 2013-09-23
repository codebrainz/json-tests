#include "boolean.h"
#include "util.h"
#include "string.h"

static JSON_Value *json_boolean_clone(JSON_Value *value)
{
	JSON_Boolean *b;
	assert(JSON_IS_BOOLEAN(value));
	b = json_boolean_new(JSON_BOOLEAN(value)->value);
	return JSON_VALUE(b);
}

static int json_boolean_compare(JSON_Value *val1, JSON_Value *val2)
{
	JSON_Boolean *b1, *b2;
	assert(JSON_IS_BOOLEAN(val1));
	assert(JSON_IS_BOOLEAN(val2));
	b1 = JSON_BOOLEAN(val1);
	b2 = JSON_BOOLEAN(val2);
	return ( (b1->value > b2->value) ? 1 : ((b1->value < b2->value) ? -1 : 0) );
}

static JSON_String *json_boolean_to_string(JSON_Value *value, int indent)
{
	JSON_String *str;
	char *indent_str;

	assert(JSON_IS_BOOLEAN(value));

	indent_str = json_make_indent_string(indent);
	str = json_string_new(indent_str);
	json_free(indent_str);

	if (json_boolean_get(JSON_BOOLEAN(value)))
		json_string_append_cstr(str, "true");
	else
		json_string_append_cstr(str, "false");

	return str;
}

JSON_Boolean *json_boolean_new(bool val)
{
	JSON_Value *value = json_value_alloc_size(sizeof(JSON_Boolean),
		JSON_VALUE_TYPE_BOOLEAN, NULL, json_boolean_clone,
		json_boolean_compare, json_boolean_to_string);
	JSON_Boolean *b = JSON_BOOLEAN(value);
	if (b)
		b->value = val;
	return b;
}

bool json_boolean_get(JSON_Boolean *b)
{
	assert(b);
	return b->value;
}

void json_boolean_set(JSON_Boolean *b, bool value)
{
	assert(b);
	b->value = value;
}
