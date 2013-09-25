#include "json.h"
#include <stdio.h>

int main()
{
	JSON_Object root;
	JSON_Array arr;
	JSON_String *str;

	json_object_init(&root);

	json_object_set(&root, "a", json_string_new("str123"));
	//json_object_debug_hash(&root);
	json_object_set(&root, "b", json_number_new(123));
	//json_object_debug_hash(&root);

	json_array_init(&arr);
	json_array_append(&arr, json_boolean_true());
	json_array_append(&arr, json_boolean_true());
	json_array_append(&arr, json_boolean_false());
	json_array_append(&arr, json_null());

	json_object_set(&root, "c", &arr);
	//json_object_debug_hash(&root);
	str = json_value_to_string(&root, 0);
	json_value_unref(&root);

	json_print(json_string_cstr(str));
	json_value_unref(str);

#if 0
#   define print_size(T) printf("Size of '" #T "': %lu\n", sizeof(T))
	printf("==================================\n");
	print_size(JSON_Value);
	print_size(JSON_Null);
	print_size(JSON_Boolean);
	print_size(JSON_Number);
	print_size(JSON_String);
	print_size(JSON_Array);
	print_size(JSON_Object);
#endif

	return 0;
}
