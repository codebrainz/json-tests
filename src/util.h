#ifndef JSON_UTIL_H_
#define JSON_UTIL_H_

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

void *json_calloc(size_t n, size_t sz);
#define json_malloc(sz) json_calloc(1, sz)
void *json_realloc(void *ptr, size_t sz);
void json_free(void *ptr);
#define json_new(T) json_calloc(1, sizeof(T))

char *json_strdup(const char *s);
char *json_strndup(const char *s, size_t n);

uint32_t json_strhash(const char *s);
bool json_strequal(const char *s1, const char *s2);

char *json_make_indent_string(int level);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // JSON_UTIL_H_
