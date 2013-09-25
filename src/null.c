#include "null.h"
#include "util.h"
#include "string.h"

static JSON_Null json_null_;
static bool json_initialized = false;

static void json_cleanup_singleton(void)
{
	if (json_initialized)
	{
		json_value_unref(&json_null_);
		json_initialized = false;
	}
}

static void json_setup_singleton(void)
{
	if (!json_initialized)
	{
		atexit(json_cleanup_singleton);
		json_value_init(JSON_TYPE_NULL, JSON_VALUE(&json_null_));
		json_initialized = true;
	}
}

static JSON_Value *json_null_clone(JSON_Value *n)
{
	assert(JSON_IS_NULL(n));
	return json_value_ref(n);
}

static bool json_null_equal(const JSON_Value *n1, const JSON_Value *n2)
{
	JSON_Null *null1, *null2;

	assert(JSON_IS_NULL(n1));
	assert(JSON_IS_NULL(n2));

	null1 = JSON_NULL(n1);
	null2 = JSON_NULL(n2);

	return (null1 == null2);
}

static JSON_String *json_null_to_string(JSON_Value *n, int indent)
{
	JSON_String *str;
	char *indent_str;

#ifndef NDEBUG
	assert(JSON_IS_NULL(n));
#else
	(void)n;
#endif

	indent_str = json_make_indent_string(indent);
	str = json_string_new_printf("%snull", indent_str);
	json_free(indent_str);

	return str;
}

JSON_Null *json_null(void)
{
	json_setup_singleton();
	return json_value_ref(&json_null_);
}

struct JSON_NullClass
{
	JSON_ValueClass base__;
};

void *json_null_get_class(void)
{
	static struct JSON_NullClass json_null_class = { {
		sizeof(JSON_Null),
		NULL,
		json_null_clone,
		json_null_equal,
		json_null_to_string,
	} };
	return &json_null_class;
}
