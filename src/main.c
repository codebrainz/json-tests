#include "json.h"
#include "lexer.h"
#include <stdio.h>
#include <stdio.h>

int main()
{

	JSON_Lexer *lex = json_lexer_new_from_cstr("{ \"key1\": null }");
	JSON_Token tok;

	while ( (tok = json_lexer_get_token(lex)) != JSON_TOKEN_EOF )
	{
		if (tok > 30 && tok < 127)
			fprintf(stderr, "Token: %c\n", (char) tok);
		else
			fprintf(stderr, "Token: %d\n", (int) tok);
	}

	json_value_unref(lex);

#if 0
	JSON_String *str = json_string_new("Hello World, how are you doing?");
	JSON_Number *num = json_number_new(123.456);
	JSON_Array *arr = json_array_new();
	JSON_Boolean *boolean = json_boolean_new(true);
	JSON_Object *obj = json_object_new();
	JSON_Null *null_value = json_null_new();

	if (!boolean)
	{
		fprintf(stderr, "Failed to allocate boolean\n");
		return 1;
	}

	json_array_append(arr, str);
	json_array_append(arr, num);
	json_array_append(arr, boolean);

	json_value_unref_many(str, num, boolean, NULL);

	json_object_set(obj, "test-key1", arr);
	json_value_unref(arr);

	str = json_string_new("Fake Value");
	json_object_set(obj, "test-key2", str);
	json_value_unref(str);

	json_object_set(obj, "test-null-obj", null_value);
	json_value_unref(null_value);

	str = json_value_to_string(obj, 0);
	json_value_unref(obj);

	printf("ToString:\n%s\n", json_string_cstr(str));
	json_value_unref(str);
#endif
	return 0;
}
