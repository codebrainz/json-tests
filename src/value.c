#include "value.h"
#include "util.h"
#include "string.h"
#include <stdarg.h>

void *json_value_init(void *class_, JSON_Value *value)
{
	JSON_ValueClass *value_class = class_;
	assert(class_ != NULL);
	assert(value != NULL);
	memset(value, 0, value_class->size);
	value->class__ = class_;
	value->flags = JSON_VALUE_FLAG_FLOATING;
	value->ref_count = 1;
	return value;
}

void *json_value_alloc(void *class_)
{
	JSON_ValueClass *value_class = class_;
	JSON_Value *value;

	assert(value_class != NULL);

	value = json_malloc(value_class->size);
	if (value != NULL)
	{
		json_value_init(class_, value);
		value->flags |= JSON_VALUE_FLAG_ON_HEAP;
	}

	return value;
}

static void json_value_free(void *v)
{
	JSON_Value *value = v;
	JSON_ValueClass *value_class;
	assert(v != NULL);
	value_class = JSON_VALUE_CLASS(v);
	assert(value_class != NULL);
	if (value_class->free)
		value_class->free(JSON_VALUE(v));
	if (value->flags & JSON_VALUE_FLAG_ON_HEAP)
		json_free(v);
	else
		memset(value, 0, value_class->size);
}

void *json_value_clone(const void *v)
{
	JSON_ValueClass *value_class;
	assert(v != NULL);
	value_class = JSON_VALUE_CLASS(v);
	assert(value_class != NULL);
	if (value_class->clone)
		return value_class->clone(JSON_VALUE(v));
	return NULL;
}

bool json_value_equal(const void *v1, const void *v2)
{
	JSON_ValueClass *value_class1, *value_class2;
	assert(v1 != NULL);
	assert(v2 != NULL);
	value_class1 = JSON_VALUE_CLASS(v1);
	value_class2 = JSON_VALUE_CLASS(v2);
	if (!value_class1 || !value_class2 || (value_class1 != value_class2))
		return false;
	assert(value_class1 != NULL);
	assert(value_class2 != NULL);
	if (value_class1->equal)
		return value_class1->equal(JSON_VALUE(v1), JSON_VALUE(v2));
	return false;
}

JSON_String *json_value_to_string(const void *v, int indent)
{
	JSON_ValueClass *value_class;
	assert(v != NULL);
	value_class = JSON_VALUE_CLASS(v);
	assert(value_class != NULL);
	if (value_class->to_string)
		return value_class->to_string(JSON_VALUE(v), indent);
	return json_string_new("");
}

void *json_value_ref(void *v)
{
	assert(v != NULL);
	assert(JSON_VALUE(v)->ref_count < ((uint32_t)-1));
	JSON_VALUE(v)->ref_count++;
	return v;
}

void *json_value_ref_sink(void *v)
{
	JSON_Value *value = v;
	assert(v != NULL);
	if (value->flags & JSON_VALUE_FLAG_FLOATING)
	{
		value->flags &= ~JSON_VALUE_FLAG_FLOATING;
		return value;
	}
	else
		return json_value_ref(value);
}

void *json_value_unref(void *v)
{
	assert(v != NULL);
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
