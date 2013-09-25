#include "boolean.h"
#include "util.h"
#include "string.h"

static JSON_Boolean json_true_;
static JSON_Boolean json_false_;
static bool json_initialized = false;

static void json_cleanup_singleton(void)
{
	if (json_initialized)
	{
		json_value_unref(&json_true_);
		json_value_unref(&json_false_);
		json_initialized = false;
	}
}

static void json_setup_singleton(void)
{
	if (!json_initialized)
	{
		atexit(json_cleanup_singleton);
		json_value_init(JSON_TYPE_BOOLEAN, JSON_VALUE(&json_true_));
		json_value_init(JSON_TYPE_BOOLEAN, JSON_VALUE(&json_false_));
		json_true_.value = true;
		json_false_.value = false;
		json_initialized = true;
	}
}

static JSON_Value *json_boolean_clone(JSON_Value *value)
{
	assert(JSON_IS_BOOLEAN(value));
	return json_value_ref(value);
}

static bool json_boolean_equal(const JSON_Value *val1, const JSON_Value *val2)
{
	JSON_Boolean *b1, *b2;
	assert(JSON_IS_BOOLEAN(val1));
	assert(JSON_IS_BOOLEAN(val2));
	b1 = JSON_BOOLEAN(val1);
	b2 = JSON_BOOLEAN(val2);
	return (b1->value == b2->value);
}

static JSON_String *json_boolean_to_string(JSON_Value *value, int indent)
{
	JSON_String *str;
	char *indent_str;

	assert(JSON_IS_BOOLEAN(value));

	indent_str = json_make_indent_string(indent);
	str = json_string_new_printf("%s%s", indent_str,
		json_boolean_get(JSON_BOOLEAN(value)) ? "true" : "false");
	json_free(indent_str);

	return str;
}

JSON_Boolean *json_boolean_true(void)
{
	json_setup_singleton();
	return json_value_ref(&json_true_);
}

JSON_Boolean *json_boolean_false(void)
{
	json_setup_singleton();
	return json_value_ref(&json_false_);
}

bool json_boolean_get(JSON_Boolean *b)
{
	assert(b);
	return b->value;
}

struct JSON_BooleanClass
{
	JSON_ValueClass base__;
};

void *json_boolean_get_class(void)
{
	static struct JSON_BooleanClass json_boolean_class = { {
		sizeof(JSON_Boolean),
		NULL,
		json_boolean_clone,
		json_boolean_equal,
		json_boolean_to_string,
	} };
	return &json_boolean_class;
}
