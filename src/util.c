#include "util.h"
#include "value.h"
#include <stdio.h>

#ifdef JSON_ALLOC_THREAD_SAFE
#include <pthread.h>
static pthread_mutex_t json_alloc_mutex;
# define JSON_ALLOC_MUTEX_INIT()   pthread_mutex_init(&json_alloc_mutex, NULL)
# define JSON_ALLOC_MUTEX_DEINIT() pthead_mutex_deinit(&json_alloc_mutex)
# define JSON_ALLOC_MUTEX_LOCK()   pthread_mutex_lock(&json_alloc_mutex)
# define JSON_ALLOC_MUTEX_UNLOCK() pthread_mutex_unlock(&json_alloc_mutex)
#else
# define JSON_ALLOC_MUTEX_INIT()   do {} while (false)
# define JSON_ALLOC_MUTEX_DEINIT() do {} while (false)
# define JSON_ALLOC_MUTEX_LOCK()   do {} while (false)
# define JSON_ALLOC_MUTEX_UNLOCK() do {} while (false)
#endif

#ifndef JSON_MALLOC
# define JSON_MALLOC malloc
#endif
#ifndef JSON_CALLOC
# define JSON_CALLOC calloc
#endif
#ifndef JSON_REALLOC
# define JSON_REALLOC realloc
#endif
#ifndef JSON_FREE
# define JSON_FREE free
#endif

#ifndef NDEBUG
# define JSON_ALLOC_STATS 1
#endif

#ifdef JSON_ALLOC_STATS
static size_t json_bytes_allocated = 0;
static size_t json_bytes_freed = 0;
#endif

static bool json_alloc_initialized = false;

static void json_alloc_deinit(void)
{
	if (JSON_LIKELY(json_alloc_initialized))
	{
		JSON_ALLOC_MUTEX_DEINIT();

#ifdef JSON_ALLOC_STATS
		fputs("==================================\n", stderr);
		fprintf(stderr, "JSON Allocator Summary:\n"
		                "  Bytes allocated: %lu\n"
		                "  Bytes freed:     %lu\n",
		                json_bytes_allocated,
		                json_bytes_freed);
		if (json_bytes_allocated > json_bytes_freed)
		{
			fprintf(stderr, "Warning: not all memory freed, %lu was leaked\n",
				json_bytes_allocated - json_bytes_freed);
		}
		json_bytes_allocated = 0;
		json_bytes_freed = 0;
#endif
		json_alloc_initialized = false;
	}
}

static void json_alloc_init(void)
{
	if (JSON_UNLIKELY(!json_alloc_initialized))
	{
		JSON_ALLOC_MUTEX_INIT();
		atexit(json_alloc_deinit);
#ifdef JSON_ALLOC_STATS
		json_bytes_allocated = 0;
		json_bytes_freed = 0;
#endif
		json_alloc_initialized = true;
	}
}

void *json_calloc(size_t n, size_t sz)
{
	uint32_t alloc_sz;
	void *ptr;

	json_alloc_init();

	JSON_ALLOC_MUTEX_LOCK();

	alloc_sz = (n * sz);
#ifdef JSON_ALLOC_STATS
	alloc_sz += sizeof(uint32_t);
#endif
	ptr = JSON_MALLOC(alloc_sz);

	if (ptr != NULL)
	{
#ifdef JSON_ALLOC_STATS
		uint32_t *hdr_ptr = ((uint32_t*)ptr);
		hdr_ptr[0] = alloc_sz;
		ptr = hdr_ptr + 1;
		memset(ptr, 0, alloc_sz - sizeof(uint32_t));
		json_bytes_allocated += alloc_sz;
#else
		memset(ptr, 0, alloc_sz);
#endif
	}
#ifndef NDEBUG
	else
	{
		fputs("Out of memory, aborting.", stderr);
		abort();
	}
#endif

	JSON_ALLOC_MUTEX_UNLOCK();

	return ptr;
}

void *json_realloc(void *ptr, size_t sz)
{
	void *new_ptr = NULL;

	json_alloc_init();

	assert(sz > 0);

	if (ptr != NULL)
	{
		uint32_t new_sz = sz;

#ifdef JSON_ALLOC_STATS
		uint32_t *hdr_ptr = ((uint32_t*)ptr) - 1;
		uint32_t old_sz = hdr_ptr[0];
		new_sz += sizeof(uint32_t);
#endif

		JSON_ALLOC_MUTEX_LOCK();

#ifndef JSON_ALLOC_STATS
		new_ptr = JSON_REALLOC(ptr, new_sz);
#else
		new_ptr = JSON_REALLOC(hdr_ptr, new_sz);
		if (new_ptr != NULL)
		{
			hdr_ptr = ((uint32_t*)new_ptr);
			hdr_ptr[0] = new_sz;
			new_ptr = hdr_ptr + 1;

			if (new_sz > old_sz)
			{
				size_t new_sz_ = new_sz - sizeof(uint32_t);
				size_t old_sz_ = old_sz - sizeof(uint32_t);
				memset(new_ptr + old_sz_, 0, new_sz_ - old_sz_);
				json_bytes_allocated += (new_sz - old_sz);
			}
			else
				json_bytes_freed += (old_sz - new_sz);
		}
#endif

		JSON_ALLOC_MUTEX_UNLOCK();
	}
	else
		new_ptr = json_malloc(sz);

#ifndef NDEBUG
	if (new_ptr == NULL)
	{
		fputs("Out of memory, aborting", stderr);
		abort();
	}
#endif

	return new_ptr;
}

void json_free(void *ptr)
{
#ifdef JSON_ALLOC_STATS
	uint32_t *hdr_ptr;
#endif

	json_alloc_init();

	assert(ptr);

#ifdef JSON_ALLOC_STATS
	hdr_ptr = ((uint32_t*)ptr) - 1;
	assert(hdr_ptr[0] > 0);
#endif

	JSON_ALLOC_MUTEX_LOCK();

#ifdef JSON_ALLOC_STATS
	json_bytes_freed += hdr_ptr[0];
	memset(hdr_ptr, 0, hdr_ptr[0]);
	JSON_FREE(hdr_ptr);
#else
	JSON_FREE(ptr);
#endif

	JSON_ALLOC_MUTEX_UNLOCK();
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
	else if (s1 && !s2)
		return 1;
	else if (!s1 && s2)
		return -1;
	else
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
