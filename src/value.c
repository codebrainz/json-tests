#include "value.h"
#include "util.h"
#include "string.h"
#include <stdarg.h>

void *json_value_alloc_size(size_t sz, JSON_ValueType type,
	JSON_FreeFunc free_func, JSON_CloneFunc clone_func,
	JSON_CompareFunc compare_func, JSON_ToStringFunc to_string_func)
{
	JSON_Value *value = json_malloc(sz);
	if (value != NULL)
	{
		value->type = type;
		value->ref_count = 1;
		value->free = free_func;
		value->clone = clone_func;
		value->compare = compare_func;
		value->to_string = to_string_func;
	}
	return value;
}

static void json_value_free(void *v)
{
	assert(JSON_IS_VALUE(v));
	if (JSON_VALUE(v)->free)
		JSON_VALUE(v)->free(JSON_VALUE(v));
	json_free(v);
}

void *json_value_clone(const void *v)
{
	assert(JSON_IS_VALUE(v));
	if (JSON_VALUE(v)->clone)
		return JSON_VALUE(v)->clone(JSON_VALUE(v));
	return NULL;
}

int json_value_compare(const void *v1, const void *v2)
{
	assert(JSON_IS_VALUE(v1));
	assert(JSON_IS_VALUE(v2));
	if (JSON_VALUE(v1)->compare)
		return JSON_VALUE(v1)->compare(JSON_VALUE(v1), JSON_VALUE(v2));
	return 0;
}

JSON_String *json_value_to_string(const void *v, int indent)
{
	assert(JSON_IS_VALUE(v));
	if (JSON_VALUE(v)->to_string)
		return JSON_VALUE(v)->to_string(JSON_VALUE(v), indent);
	return json_string_new("");
}

void *json_value_ref(void *v)
{
	assert(JSON_IS_VALUE(v));
	assert(JSON_VALUE(v)->ref_count < ((size_t)-1));
	JSON_VALUE(v)->ref_count++;
	return v;
}

void *json_value_unref(void *v)
{
	assert(JSON_IS_VALUE(v));
	assert(JSON_VALUE(v)->ref_count > 0);
	if (JSON_VALUE(v)->ref_count > 1)
		JSON_VALUE(v)->ref_count--;
	else
	{
		JSON_VALUE(v)->ref_count = 0;
		json_value_free(JSON_VALUE(v));
		v = NULL;
	}
	return v;
}

void json_value_unref_many(void *v, ...)
{
	va_list ap;
	void *it;

	if (!v)
		return;

	va_start(ap, v);
	json_value_unref(v);
	while ( (it = va_arg(ap, JSON_Value*)) != NULL )
		json_value_unref(it);
	va_end(ap);
}
