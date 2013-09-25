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

static bool json_number_equal(const JSON_Value *v1, const JSON_Value *v2)
{
	JSON_Number *n1, *n2;
	assert(JSON_IS_NUMBER(v1));
	assert(JSON_IS_NUMBER(v2));
	n1 = JSON_NUMBER(v1);
	n2 = JSON_NUMBER(v2);
	return (n1->value == n2->value);
}

static JSON_String *json_number_to_string(JSON_Value *value, int indent)
{
	char *indent_str;
	JSON_String *str;

	assert(JSON_IS_NUMBER(value));

	indent_str = json_make_indent_string(indent);
	str = json_string_new_printf("%s%f", indent_str,
		json_number_get(JSON_NUMBER(value)));
	json_free(indent_str);

	return str;
}

JSON_Number *json_number_new(double val)
{
	JSON_Number *n = json_value_alloc(JSON_TYPE_NUMBER);
	if (n)
		n->value = val;
	return n;
}

JSON_Number *json_number_init(JSON_Number *num)
{
	assert(num != NULL);
	json_value_init(JSON_TYPE_NUMBER, JSON_VALUE(num));
	num->value = 0.0;
	return num;
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

struct JSON_NumberClass
{
	JSON_ValueClass base__;
};

void *json_number_get_class(void)
{
	static struct JSON_NumberClass json_number_class = { {
		sizeof(JSON_Number),
		NULL,
		json_number_clone,
		json_number_equal,
		json_number_to_string,
	} };
	return &json_number_class;
}
