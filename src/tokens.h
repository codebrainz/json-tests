#ifndef JSON_TOKENS_H_
#define JSON_TOKENS_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
	// Character-literal tokens
	JSON_TOKEN_ERROR    = -1,
	JSON_TOKEN_EOF      = '\0',
	JSON_TOKEN_LBRACE   = '{',
	JSON_TOKEN_RBRACE   = '}',
	JSON_TOKEN_LBRACKET = '[',
	JSON_TOKEN_RBRACKET = ']',
	JSON_TOKEN_COLON    = ':',
	JSON_TOKEN_COMMA    = ',',
	JSON_TOKEN_DQUOTE   = '"',
	JSON_TOKEN_SQUOTE   = '\'',

	// Non-character tokens
	JSON_TOKEN_NUMBER = 256,
	JSON_TOKEN_STRING = 257,
	JSON_TOKEN_TRUE   = 258,
	JSON_TOKEN_FALSE  = 259,
	JSON_TOKEN_NULL   = 260,
}
JSON_Token;

#ifdef __cplusplus
} // extern "C"
#endif

#endif // JSON_TOKENS_H_
