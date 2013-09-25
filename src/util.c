#include "util.h"
#include "value.h"
#include <stdio.h>

#define JSON_ABORT_OOM(ptr)          \
do {                                 \
  if (JSON_UNLIKELY(!ptr)) {         \
    fputs("Out of memory.", stderr); \
    abort();                         \
  }                                  \
} while (false)

static struct JSON_AllocData
{
	JSON_AllocMallocFunc malloc_fn;
	JSON_AllocReallocFunc realloc_fn;
	JSON_AllocFreeFunc free_fn;
}
json_alloc_data = { malloc, realloc, free };

bool json_set_allocator(JSON_AllocMallocFunc malloc_fn,
	JSON_AllocReallocFunc realloc_fn, JSON_AllocFreeFunc free_fn)
{
	// If all functions are NULL, use defaults
	if (malloc_fn == NULL && realloc_fn == NULL && free_fn == NULL)
	{
		json_alloc_data.malloc_fn = malloc;
		json_alloc_data.realloc_fn = realloc;
		json_alloc_data.free_fn = free;
		return true;
	}
	// If all functions are not NULL, use them
	else if (malloc_fn != NULL && realloc_fn != NULL && free_fn != NULL)
	{
		json_alloc_data.malloc_fn = malloc_fn;
		json_alloc_data.realloc_fn = realloc_fn;
		json_alloc_data.free_fn = free_fn;
		return true;
	}
	// If there's a mix-and-match, just bail-out/return, we're screwed
	else
	{
#ifndef NDEBUG
		fprintf(stderr,
		        "The function '%s' requires either all parameters "
		        "to be NULL (ie. use defaults) or ALL arguments to "
		        "NOT be NULL. Mixing and matching default and "
		        "user-provided allocators is asking for disaster. "
		        "Sorry.", __func__);
		abort();
#endif
		return false;
	}
}

void *json_malloc(size_t sz)
{
	void *mem;
	assert(json_alloc_data.malloc_fn);
	mem = json_alloc_data.malloc_fn(sz);
	JSON_ABORT_OOM(mem);
	memset(mem, 0, sz); // make sure memory is zeroed-out
	return mem;
}

void *json_realloc(void *mem, size_t sz)
{
	void *new_mem;
	assert(json_alloc_data.realloc_fn);
	new_mem = json_alloc_data.realloc_fn(mem, sz);
	JSON_ABORT_OOM(new_mem);
	return new_mem;
}

void json_free(void *mem)
{
	assert(mem);
	assert(json_alloc_data.free_fn);
	json_alloc_data.free_fn(mem);
}

char *json_strdup(const char *other)
{
	size_t len;
	char *buf;
	if (!other)
		return NULL;
	len = strlen(other);
	buf = json_malloc(len + 1);
	if (!buf)
		return NULL;
	memcpy(buf, other, len + 1);
	return buf;
}

char *json_strndup(const char *other, size_t n)
{
	char *buf;
	if (!other)
		return NULL;
	buf = json_malloc(n + 1);
	if (!buf)
		return NULL;
	memcpy(buf, other, n);
	buf[n] = '\0';
	return buf;
}

// Source: http://www.cse.yorku.ca/~oz/hash.html
uint32_t json_strhash(const char *s)
{
	uint32_t hash = 0;
	int c;
	assert(s);
	while ( (c = *s++) )
		hash = c + (hash << 6) + (hash << 16) - hash;
	return hash;
}

bool json_strequal(const char *s1, const char *s2)
{
	if (!s1 && !s2)
		return true;
	else if ( (s1 && !s2) || (!s1 && s2) )
		return false;
	return (strcmp(s1, s2) == 0);
}

char *json_make_indent_string(int level)
{
	size_t n_chars = level * JSON_INDENT_WIDTH;
	char *out = json_malloc(n_chars + 1);
	if (n_chars > 0)
		memset(out, ' ', n_chars);
	out[n_chars] = '\0';
	return out;
}

// Adapated from vsnprintf()
char *json_strvprintf(const char *fmt, va_list ap_in)
{
	int n, size = 64 /*guess*/;
	char *p, *np;
	va_list ap;

	assert(fmt != NULL);

	if ( !(p = json_malloc(size)) )
		return NULL;

	while (true)
	{
		va_copy(ap, ap_in);
		n = vsnprintf(p, size, fmt, ap);
		va_end(ap);

		if (n > -1 && n < size)
			return p;

		if (n > -1)
			size = n + 1;
		else
			size *= 2;

		if ( (np = json_realloc(p, size)) == NULL )
		{
			json_free(p);
			return NULL;
		}

		p = np;
	}
}

char *json_strprintf(const char *fmt, ...)
{
	char *s;
	va_list ap;
	va_start(ap, fmt);
	s = json_strvprintf(fmt, ap);
	va_end(ap);
	return s;
}

void json_print(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stdout, fmt, ap);
	va_end(ap);
	fputc('\n', stdout);
	fflush(stdout);
}

void json_printerr(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fputc('\n', stderr);
	fflush(stderr);
}
