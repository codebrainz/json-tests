#include "number.h"
#include "util.h"
#include "string.h"
#include <stdio.h>

static JSON_Value *json_number_clone(JSON_Value *value)
{
	JSON_Number *num;
	assert(JSON_IS_NUMBER(value));
	num = json_number_new(JSON_NUMBER(value)->value);
	return JSON_VALUE(num);
}

static int json_number_compare(JSON_Value *v1, JSON_Value *v2)
{
	JSON_Number *n1, *n2;
	assert(JSON_IS_NUMBER(v1));
	assert(JSON_IS_NUMBER(v2));
	n1 = JSON_NUMBER(v1);
	n2 = JSON_NUMBER(v2);
	return ( (n1 > n2) ? 1 : ((n1 < n2) ? -1 : 0) );
}

static JSON_String *json_number_to_string(JSON_Value *value, int indent)
{
	char temp[64] = { 0 };
	char *indent_str;
	JSON_String *str;

	assert(JSON_IS_NUMBER(value));

	indent_str = json_make_indent_string(indent);
	str = json_string_new(indent_str);
	json_free(indent_str);

	snprintf(temp, 64, "%f", json_number_get(JSON_NUMBER(value)));
	json_string_append_cstr(str, temp);

	return str;
}

JSON_Number *json_number_new(double val)
{
	JSON_Value *value = json_value_alloc_size(sizeof(JSON_Number),
		JSON_VALUE_TYPE_NUMBER, NULL, json_number_clone,
		json_number_compare, json_number_to_string);
	JSON_Number *n = JSON_NUMBER(value);
	if (n)
		n->value = val;
	return n;
}

double json_number_get(JSON_Number *num)
{
	assert(num);
	return num->value;
}

void json_number_set(JSON_Number *num, double value)
{
	assert(num);
	num->value = value;
}
