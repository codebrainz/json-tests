#include "lexer.h"
#include "util.h"
#include <ctype.h>

static void json_lexer_free(JSON_Value *value)
{
	JSON_Lexer *lex = JSON_LEXER(value);
	assert(JSON_IS_LEXER(lex));
	json_value_unref(lex->str);
	json_value_unref(lex->value);
	lex->offset = 0;
	lex->lastchar = 0;
}

static JSON_Lexer *json_lexer_new_internal(void)
{
	JSON_Lexer *lex = json_value_alloc(JSON_TYPE_LEXER);
	lex->str = json_string_new("");
	lex->value = json_string_new("");
	lex->lastchar = JSON_LEXER_ERROR;
	return lex;
}

JSON_Lexer *json_lexer_new(JSON_String *input_str)
{
	assert(JSON_IS_STRING(input_str));
	return json_lexer_new_from_cstr_length(input_str->str, input_str->len);
}

JSON_Lexer *json_lexer_new_from_cstr(const char *input_str)
{
	size_t len = 0;
	if (input_str != NULL)
		len = strlen(input_str);
	return json_lexer_new_from_cstr_length(input_str, len);
}

JSON_Lexer *json_lexer_new_from_cstr_length(const char *input_str, size_t len)
{
	JSON_Lexer *lex;
	assert(input_str != NULL);
	assert(len > 0);
	lex = json_lexer_new_internal();
	json_string_assign_length(lex->str, input_str, len);
	return lex;
}

JSON_Lexer *json_lexer_new_from_stream(FILE *fp)
{// FIXME: error checking
	size_t len;
	char *temp;
	JSON_Lexer *lex;

	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	lex = json_lexer_new_internal();
	temp = json_realloc(lex->str->str, len + 1);
	if (temp != NULL)
	{
		size_t read_len;
		lex->str->len = len;
		lex->str->str = temp;
		read_len = fread(lex->str->str, sizeof(char), len, fp);
		assert(read_len == len);
		(void)read_len;
		lex->str->str[lex->str->len] = '\0';
	}

	return lex;
}

JSON_Lexer *json_lexer_new_from_file(const char *filename)
{
	FILE *fp;
	JSON_Lexer *lex;

	assert(filename != NULL);

	fp = fopen(filename, "rb");
	if (!fp)
		return NULL;

	lex = json_lexer_new_from_stream(fp);
	fclose(fp);

	return lex;
}

static uint32_t json_lexer_peekchar(JSON_Lexer *lex)
{
	assert(JSON_IS_LEXER(lex));
	if ((lex->offset + 1) >= lex->str->len)
		return JSON_LEXER_EOF;
	return lex->str->str[lex->offset + 1];
}

static uint32_t json_lexer_getchar(JSON_Lexer *lex)
{
	assert(JSON_IS_LEXER(lex));

	if (lex->offset >= lex->str->len)
		lex->lastchar = JSON_LEXER_EOF;
	else
		lex->lastchar = lex->str->str[lex->offset++];

	if (lex->lastchar == '\r' && json_lexer_peekchar(lex) != '\n')
	{
		lex->line++;
		lex->column = 0;
	}
	else if (lex->lastchar == '\n')
	{
		lex->line++;
		lex->column = 0;
	}

	return lex->lastchar;
}

JSON_Token json_lexer_get_token(JSON_Lexer *lex)
{
	static uint32_t last = ' '; // FIXME: use lex->lastchar
	uint32_t temp;

	assert(JSON_IS_LEXER(lex));

	// Skip white space
	while (isspace(last))
		last = json_lexer_getchar(lex);

	// Identifiers (null, true, false)
	if (isalpha(last))
	{
		JSON_String *ident;
		JSON_Token tok = JSON_TOKEN_ERROR;
		ident = json_string_new_printf("%c", (char) last);
		while (isalnum( (last = json_lexer_getchar(lex)) ) || last == '_')
			json_string_append_char(ident, last);
		if (json_strequal(json_string_cstr(ident), "null"))
			tok = JSON_TOKEN_NULL;
		else if (json_strequal(json_string_cstr(ident), "true"))
			tok = JSON_TOKEN_TRUE;
		else if (json_strequal(json_string_cstr(ident), "false"))
			tok = JSON_TOKEN_FALSE;
		json_value_unref(ident);
		return tok;
	}

	// Numbers
	// FIXME: write this properly
	if (isdigit(last) || last == '.')
	{
		JSON_String *ident = json_string_new("");
		do
		{
			json_string_append_char(ident, last);
			last = json_lexer_getchar(lex);
		}
		while (isdigit(last) || last == '.');
		json_string_assign_length(lex->value, ident->str, ident->len);
		json_value_unref(ident);
		return JSON_TOKEN_NUMBER;
	}

#if 0
	// String literals
	if (last == '"')
	{
		JSON_String *str = json_string_new("");
		do
		{
			json_string_append_char(str, last);
			last = json_lexer_getchar(lex);
		} while (last != '"' && last != JSON_LEXER_EOF);
		json_string_assign_length(lex->value, str->str, str->len);
		json_value_unref(str);
		return JSON_TOKEN_STRING;
	}
#endif

	// Check for EOF
	if (last == JSON_LEXER_EOF)
		return JSON_TOKEN_EOF;

	// Return character token
	temp = last;
	last = json_lexer_getchar(lex);
	return temp;
}

struct JSON_LexerClass
{
	JSON_ValueClass base__;
};

void *json_lexer_get_class(void)
{
	static struct JSON_LexerClass json_lexer_class = { {
		sizeof(JSON_Lexer),
		json_lexer_free,
		NULL,
		NULL,
		NULL,
	} };
	return &json_lexer_class;
}
