#ifndef JSON_UTIL_H_
#define JSON_UTIL_H_

#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* (*JSON_AllocMallocFunc)(size_t);
typedef void* (*JSON_AllocReallocFunc)(void*, size_t);
typedef void (*JSON_AllocFreeFunc)(void*);

bool json_set_allocator(JSON_AllocMallocFunc malloc_fn,
	JSON_AllocReallocFunc realloc_fn, JSON_AllocFreeFunc free_fn);

void *json_malloc(size_t sz);
void *json_realloc(void *ptr, size_t sz);
void json_free(void *ptr);
#define json_new(T) json_malloc(sizeof(T))

char *json_strdup(const char *s);
char *json_strndup(const char *s, size_t n);
uint32_t json_strhash(const char *s);
bool json_strequal(const char *s1, const char *s2);
char *json_strprintf(const char *fmt, ...);
char *json_strvprintf(const char *fmt, va_list ap);

void json_print(const char *fmt, ...);
void json_printerr(const char *fmt, ...);

#define JSON_INDENT_WIDTH 2
char *json_make_indent_string(int level);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // JSON_UTIL_H_
