#ifndef JSON_LEXER_H_
#define JSON_LEXER_H_

#include "value.h"
#include "string.h"
#include "tokens.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define JSON_LEXER_ERROR ((uint32_t)-1)
#define JSON_LEXER_EOF   ((JSON_LEXER_ERROR) - 1)

typedef struct
{
	JSON_Value base__;
	JSON_String *str;
	JSON_String *value;
	size_t offset;
	size_t line;
	size_t column;
	uint32_t lastchar;
}
JSON_Lexer;

#define JSON_LEXER(v)    ((JSON_Lexer*)(v))
#define JSON_IS_LEXER(v) JSON_LIKELY(((v) != NULL) && (JSON_VALUE(v)->type == JSON_VALUE_TYPE_LEXER))

JSON_Lexer *json_lexer_new(JSON_String *input_str);
JSON_Lexer *json_lexer_new_from_cstr(const char *input_str);
JSON_Lexer *json_lexer_new_from_cstr_length(const char *input_str, size_t len);
JSON_Lexer *json_lexer_new_from_stream(FILE *fp);
JSON_Lexer *json_lexer_new_from_file(const char *filename);

#define json_lexer_offset(lex)   JSON_LEXER(lex)->offset
#define json_lexer_line(lex)     JSON_LEXER(lex)->line
#define json_lexer_column(lex)   JSON_LEXER(lex)->column
#define json_lexer_eof(lex)      (json_lexer_lastchar(lex) == JSON_LEXER_EOF)

JSON_Token json_lexer_get_token(JSON_Lexer *lex);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // JSON_LEXER_H_
