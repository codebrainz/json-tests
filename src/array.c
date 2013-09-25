#include "array.h"
#include "util.h"
#include "string.h"

static void json_array_free(JSON_Value *arr)
{
	size_t i;
	assert(arr);
	// Unref all of the elements, possibly destroying them
	for (i = 0; i < JSON_ARRAY(arr)->size; i++)
		json_value_unref(JSON_ARRAY(arr)->array[i]);
	if (JSON_ARRAY(arr)->array)
		json_free(JSON_ARRAY(arr)->array);
}

static JSON_Value *json_array_clone(JSON_Value *arr)
{
	JSON_Array *new_arr = json_array_new();
	if (new_arr != NULL)
	{
		new_arr->array = json_malloc(JSON_ARRAY(arr)->size * sizeof(JSON_Value*));
		if (new_arr->array != NULL)
		{
			size_t i;
			new_arr->size = JSON_ARRAY(arr)->size;
			for (i = 0; i < JSON_ARRAY(arr)->size; i++)
				new_arr->array[i] = json_value_clone(JSON_ARRAY(arr)->array[i]);
		}
	}
	return JSON_VALUE(new_arr);
}

static bool json_array_equal(const JSON_Value *val1, const JSON_Value *val2)
{
	JSON_Array *arr1, *arr2;
	size_t i;

	assert(JSON_IS_ARRAY(val1));
	assert(JSON_IS_ARRAY(val2));

	arr1 = JSON_ARRAY(val1);
	arr2 = JSON_ARRAY(val2);

	if (json_array_size(arr1) != json_array_size(arr2))
		return false;

	for (i = 0; i < json_array_size(arr1); i++)
	{
		if (!json_value_equal(json_array_nth(arr1, i), json_array_nth(arr2, i)))
			return false;
	}

	return true;
}

static JSON_String *json_array_to_string(JSON_Value *value, int indent)
{
	size_t i;
	JSON_String *str;
	JSON_Array *arr;
	char *indent_str;

	assert(JSON_IS_ARRAY(value));
	arr = JSON_ARRAY(value);

	indent_str = json_make_indent_string(indent);
	str = json_string_new_printf("%s[\n", indent_str);
	json_free(indent_str);

	indent++;

	for (i = 0; i < arr->size; i++)
	{
		JSON_String *tmp = json_value_to_string(arr->array[i], indent);
		json_string_append(str, tmp);
		json_value_unref(JSON_VALUE(tmp));
		if (i != (arr->size - 1))
			json_string_append_cstr(str, ",\n");
		else
			json_string_append_char(str, '\n');
	}

	indent--;

	indent_str = json_make_indent_string(indent);
	json_string_append_printf(str, "%s]", indent_str);
	json_free(indent_str);

	return str;
}

JSON_Array *json_array_new(void)
{
	return json_value_alloc(JSON_TYPE_ARRAY);
}

JSON_Array *json_array_init(JSON_Array *arr)
{
	assert(arr != NULL);
	json_value_init(JSON_TYPE_ARRAY, JSON_VALUE(arr));
	return arr;
}

size_t json_array_size(JSON_Array *arr)
{
	assert(arr);
	return arr->size;
}

JSON_Value *json_array_nth(JSON_Array *arr, size_t n)
{
	assert(arr);
	assert(n < arr->size);
	return arr->array[n];
}

static bool json_array_resize(JSON_Array *arr, size_t new_size)
{
	JSON_Value **temp = json_realloc(arr->array, new_size * sizeof(JSON_Value*));
	if (temp != NULL)
	{
		arr->array = temp;
		arr->size = new_size;
		return true;
	}
	return false;
}

bool json_array_insert(JSON_Array *arr, JSON_Value *value, size_t pos)
{
	assert(arr);
	assert(value);

	if (pos > arr->size)
		return false;

	if (!json_array_resize(arr, arr->size + 1))
		return false;

	if (pos == (arr->size - 1))
		arr->array[pos] = json_value_ref_sink(value);
	else
	{
		size_t i;
		for (i = (arr->size - 1); i > pos; i--)
			arr->array[i] = arr->array[i-1];
		arr->array[pos] = json_value_ref_sink(value);
	}

	return false;
}

void json_array_remove_nth(JSON_Array *arr, size_t pos)
{
	size_t i;

	assert(arr);
	assert(pos < arr->size);

	json_value_unref(arr->array[pos]);
	arr->array[pos] = NULL;

	for (i = pos + 1; i < arr->size; i++)
		arr->array[i - 1] = arr->array[i];

	if ((arr->size - 1) == 0)
	{
		json_free(arr->array);
		arr->array = NULL;
		arr->size = 0;
	}
	else
		json_array_resize(arr, arr->size - 1);
}

struct JSON_ArrayClass
{
	JSON_ValueClass base__;
};

void *json_array_get_class(void)
{
	static struct JSON_ArrayClass json_array_class = { {
		sizeof(JSON_Array),
		json_array_free,
		json_array_clone,
		json_array_equal,
		json_array_to_string,
	} };
	return &json_array_class;
}
