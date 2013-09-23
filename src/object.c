#include "object.h"
#include "util.h"
#include "string.h"

static void json_object_free(JSON_Value *value)
{
	assert(JSON_IS_OBJECT(value));
	json_value_unref(JSON_OBJECT(value)->keys);
	json_value_unref(JSON_OBJECT(value)->values);
}

static JSON_Value *json_object_clone(JSON_Value *value)
{
	JSON_Object *obj = JSON_OBJECT(value);
	JSON_Object *new_obj;
	assert(JSON_IS_OBJECT(value));
	new_obj = json_object_new();
	if (new_obj != NULL)
	{
		new_obj->keys = json_value_clone(obj->keys);
		new_obj->values = json_value_clone(obj->values);
	}
	return JSON_VALUE(new_obj);
}

static int json_object_compare(JSON_Value *val1, JSON_Value *val2)
{ // FIXME
	JSON_Object *obj1, *obj2;
	assert(JSON_IS_OBJECT(val1));
	assert(JSON_IS_OBJECT(val2));
	obj1 = JSON_OBJECT(val1);
	obj2 = JSON_OBJECT(val2);
	return (json_array_size(obj1->keys) - json_array_size(obj2->keys));
}

static JSON_String *json_object_to_string(JSON_Value *value, int indent)
{
	size_t i;
	JSON_String *str;
	JSON_Object *obj = JSON_OBJECT(value);
	char *indent_str;

	assert(JSON_IS_OBJECT(value));

	str = json_string_new("{\n");

	indent++;

	for (i = 0; i < json_array_size(obj->keys); i++)
	{
		JSON_String *temp = json_value_to_string(json_array_nth(obj->keys, i), indent);
		if (JSON_IS_STRING(temp))
		{
			json_string_append(str, temp);
			json_value_unref(temp);
			json_string_append_cstr(str, ": ");
			temp = json_value_to_string(json_array_nth(obj->values, i), indent);
			json_string_lstrip(temp);
			if (JSON_IS_STRING(temp))
			{
				json_string_append(str, temp);
				json_value_unref(temp);
			}
			if (i != (json_array_size(obj->keys) - 1))
				json_string_append_cstr(str, ",\n");
			else
				json_string_append_cstr(str, "\n");
		}
	}

	indent--;

	indent_str = json_make_indent_string(indent);
	json_string_append_cstr(str, indent_str);
	json_free(indent_str);
	json_string_append_cstr(str, "}");

	return str;
}

JSON_Object *json_object_new(void)
{
	JSON_Value *value = json_value_alloc_size(sizeof(JSON_Object),
		JSON_VALUE_TYPE_OBJECT, json_object_free, json_object_clone,
		json_object_compare, json_object_to_string);
	JSON_Object *obj = JSON_OBJECT(value);
	obj->keys = json_array_new();
	obj->values = json_array_new();
	return obj;
}

JSON_Value *json_object_find(JSON_Object *obj, const char *key)
{
	size_t i;
	assert(JSON_IS_OBJECT(obj));
	assert(key != NULL);
	for (i = 0; i < json_array_size(obj->keys); i++)
	{
		JSON_String *test_key = JSON_STRING(json_array_nth(obj->keys, i));
		if (JSON_IS_STRING(test_key) && json_string_cstr(test_key) != NULL)
		{
			if (strcmp(json_string_cstr(test_key), key) == 0)
				return json_array_nth(obj->values, i);
		}
	}
	return NULL;
}

void json_object_insert(JSON_Object *obj, const char *key, JSON_Value *value)
{
	size_t i;
	JSON_String *new_key;

	assert(JSON_IS_OBJECT(obj));
	assert(key != NULL);
	assert(JSON_IS_VALUE(value));

	for (i = 0; i < json_array_size(obj->keys); i++)
	{
		JSON_String *test_key = JSON_STRING(json_array_nth(obj->keys, i));
		if (JSON_IS_STRING(test_key) && json_string_cstr(test_key) != NULL)
		{
			if (strcmp(json_string_cstr(test_key), key) == 0)
			{
				json_value_unref(obj->values->array[i]);
				obj->values->array[i] = json_value_ref(value);
				return;
			}
		}
	}

	new_key = json_string_new(key);
	json_array_append(obj->keys, JSON_VALUE(new_key));
	json_value_unref(new_key);
	json_array_append(obj->values, value);

	assert(json_array_size(obj->keys) == json_array_size(obj->values));
}

bool json_object_remove(JSON_Object *obj, const char *key)
{
	size_t i;
	bool removed = false;

	assert(JSON_IS_OBJECT(obj));
	assert(key != NULL);

	for (i = 0; i < json_array_size(obj->keys); i++)
	{
		JSON_String *test_key = JSON_STRING(json_array_nth(obj->keys, i));
		if (JSON_IS_STRING(test_key) && json_string_cstr(test_key) != NULL)
		{
			if (strcmp(json_string_cstr(test_key), key) == 0)
			{
				json_array_remove_nth(obj->keys, i);
				json_array_remove_nth(obj->values, i);
				removed = true;
				break;
			}
		}
	}

	assert(json_array_size(obj->keys) == json_array_size(obj->values));

	return removed;
}
