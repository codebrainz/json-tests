#include "object.h"
#include "util.h"
#include "string.h"

#define JSON_OBJECT_INITIAL_NUM_BUCKETS 3
#define JSON_OBJECT_GROW_FACTOR (1.0+(1.0/0.3))
#define JSON_OBJECT_SHRINK_FACTOR (1.0/0.3)
#define JSON_OBJECT_REHASH_MAX_LOAD_FACTOR 0.8
#define JSON_OBJECT_REHASH_MIN_LOAD_FACTOR 0.2

#define json_bucket_link_alloc() json_new(struct JSON_BucketLink)

static inline void json_bucket_link_free(struct JSON_BucketLink *link)
{
	assert(link != NULL);
	json_free(link->key);
	json_value_unref(link->value);
	json_free(link);
}

static void json_object_free(JSON_Value *value)
{
	size_t i;
	JSON_Object *obj = JSON_OBJECT(value);

	assert(JSON_IS_OBJECT(value));

	for (i = 0; i < obj->num_buckets; i++)
	{
		struct JSON_BucketLink *link = obj->buckets[i];
		while (link != NULL)
		{
			struct JSON_BucketLink *next = link->next;
			json_bucket_link_free(link);
			link = next;
		}
	}

	json_free(obj->buckets);
}

static JSON_Value *json_object_clone(JSON_Value *value)
{
	JSON_Object *obj = JSON_OBJECT(value);
	JSON_Object *new_obj;
	size_t i;

	assert(JSON_IS_OBJECT(value));

	new_obj = json_object_new();

	for (i = 0; i < obj->num_buckets; i++)
	{
		struct JSON_BucketLink *link;
		for (link = obj->buckets[i]; link != NULL; link = link->next)
			json_object_set(new_obj, link->key, json_value_clone(link->value));
	}

	return JSON_VALUE(new_obj);
}

static bool json_object_equal(const JSON_Value *val1, const JSON_Value *val2)
{
	size_t i;
	JSON_Object *obj1 = JSON_OBJECT(val1);
	JSON_Object *obj2 = JSON_OBJECT(val2);

	assert(JSON_IS_OBJECT(val1));
	assert(JSON_IS_OBJECT(val2));

	if (obj1->num_elements != obj2->num_elements)
		return false;

	for (i = 0; i < obj1->num_buckets; i++)
	{
		struct JSON_BucketLink *link;
		for (link = obj1->buckets[i]; link != NULL; link = link->next)
		{
			JSON_Value *value = json_object_get(obj2, link->key);
			if (value == NULL)
				return false;
			else if (!json_value_equal(link->value, value))
				return false;
		}
	}

	return true;
}

static JSON_String *json_object_to_string(JSON_Value *value, int indent)
{
	JSON_Object *obj = JSON_OBJECT(value);
	JSON_String *str;
	char *indent_str;
	size_t i;

	assert(JSON_IS_OBJECT(value));

	indent_str = json_make_indent_string(indent);
	str = json_string_new_printf("%s{\n", indent_str);
	json_free(indent_str);

	indent++;
	indent_str = json_make_indent_string(indent);

	for (i = 0; i < obj->num_buckets; i++)
	{
		struct JSON_BucketLink *link;
		for (link = obj->buckets[i]; link != NULL; link = link->next)
		{
			JSON_String *value_str = json_value_to_string(link->value, indent);
			assert(JSON_IS_STRING(value_str));
			json_string_lstrip(value_str);
			json_string_prepend_printf(value_str, "%s\"%s\": ", indent_str,
				link->key);
			json_string_append(str, value_str);
			json_value_unref(value_str);
			// FIXME: this doesn't work
			if (i == (obj->num_buckets - 1) && link->next == NULL)
				json_string_append_char(str, '\n');
			else
				json_string_append_cstr(str, ",\n");
		}
	}

	json_free(indent_str);
	indent--;

	indent_str = json_make_indent_string(indent);
	json_string_append_printf(str, "%s}\n", indent_str);
	json_free(indent_str);

	return str;
}

static void json_object_init_buckets(JSON_Object *obj)
{
	obj->buckets = json_malloc(
		JSON_OBJECT_INITIAL_NUM_BUCKETS * sizeof(struct JSON_BucketLink*));
	obj->num_buckets = JSON_OBJECT_INITIAL_NUM_BUCKETS;
}

JSON_Object *json_object_new(void)
{
	JSON_Object *obj = json_value_alloc(JSON_TYPE_OBJECT);
	if (obj != NULL)
		json_object_init_buckets(obj);
	return obj;
}

JSON_Object *json_object_init(JSON_Object *obj)
{
	assert(obj != NULL);
	json_value_init(JSON_TYPE_OBJECT, JSON_VALUE(obj));
	json_object_init_buckets(obj);
	return obj;
}

static inline uint32_t json_object_get_bucket_num(JSON_Object *obj, const char *key)
{
	uint32_t bucket_num = json_strhash(key);
	if (bucket_num == 0)
		return 0;
	return bucket_num % obj->num_buckets;
}

static inline double json_object_get_load_factor(JSON_Object *obj)
{
	return ((double)obj->num_elements / (double)obj->num_buckets);
}

bool json_object_rehash(JSON_Object *obj)
{
	uint32_t new_num_buckets;
	double load_factor;
	size_t i, temp_num_buckets, temp_num_elements;
	struct JSON_BucketLink **temp_buckets;
	JSON_Object temp_object;

	assert(JSON_IS_OBJECT(obj));

	load_factor = json_object_get_load_factor(obj);
	if ( (load_factor < JSON_OBJECT_REHASH_MAX_LOAD_FACTOR) &&
	     (load_factor > JSON_OBJECT_REHASH_MIN_LOAD_FACTOR) )
	{
		return false;
	}

	if (load_factor > JSON_OBJECT_REHASH_MAX_LOAD_FACTOR)
	{
		new_num_buckets =
			(uint32_t)((double)obj->num_buckets * JSON_OBJECT_GROW_FACTOR);
		assert(new_num_buckets > obj->num_buckets);
	}
	else
	{
		new_num_buckets =
			(uint32_t)((double)obj->num_buckets * JSON_OBJECT_SHRINK_FACTOR);
		assert(new_num_buckets < obj->num_buckets);
	}

	json_value_init(JSON_TYPE_OBJECT, JSON_VALUE(&temp_object));

	temp_object.num_buckets = new_num_buckets;
	temp_object.buckets = json_malloc(
		temp_object.num_buckets * sizeof(struct JSON_BucketLink*));

	if (temp_object.buckets == NULL)
	{
		json_value_unref(&temp_object);
		return false;
	}

	for (i = 0; i < obj->num_buckets; i++)
	{
		struct JSON_BucketLink *link;
		for (link = obj->buckets[i]; link != NULL; link = link->next)
			json_object_set(&temp_object, link->key, json_value_ref(link->value));
	}

	temp_num_buckets = obj->num_buckets;
	temp_num_elements = obj->num_elements;
	temp_buckets = obj->buckets;

	obj->num_buckets = temp_object.num_buckets;
	obj->num_elements = temp_object.num_elements;
	obj->buckets = temp_object.buckets;

	temp_object.num_buckets = temp_num_buckets;
	temp_object.num_elements = temp_num_elements;
	temp_object.buckets = temp_buckets;
	json_value_unref(&temp_object);

	return true;
}

JSON_Value *json_object_get(JSON_Object *obj, const char *key)
{
	uint32_t bucket_num;
	struct JSON_BucketLink *link;

	assert(JSON_IS_OBJECT(obj));
	assert(key != NULL);

	bucket_num = json_object_get_bucket_num(obj, key);
	assert(bucket_num < obj->num_buckets);

	for (link = obj->buckets[bucket_num]; link != NULL; link = link->next)
	{
		if (json_strequal(key, link->key))
			return link->value;
	}

	return NULL;
}

// return true if element is added, false if replaced
bool json_object_set_value(JSON_Object *obj, const char *key, JSON_Value *value)
{
	uint32_t bucket_num;
	struct JSON_BucketLink *link;

	assert(JSON_IS_OBJECT(obj));
	assert(key != NULL);
	assert(value != NULL);

	bucket_num = json_object_get_bucket_num(obj, key);
	assert(bucket_num < obj->num_buckets);

	// Look for existing
	for (link = obj->buckets[bucket_num]; link != NULL; link = link->next)
	{
		if (json_strequal(key, link->key))
		{
			json_value_unref(link->value);
			link->value = json_value_ref_sink(value);
			return false;
		}
	}

	// Else add new element
	link = json_bucket_link_alloc();
	link->key = json_strdup(key);
	link->value = json_value_ref_sink(value);
	link->next = obj->buckets[bucket_num];
	obj->buckets[bucket_num] = link;
	obj->num_elements++;

	return true;
}

bool json_object_del(JSON_Object *obj, const char *key)
{
	uint32_t bucket_num;
	struct JSON_BucketLink *link, *prev = NULL;

	assert(JSON_IS_OBJECT(obj));
	assert(key != NULL);

	bucket_num = json_object_get_bucket_num(obj, key);
	assert(bucket_num < obj->num_buckets);

	for (link = obj->buckets[bucket_num]; link != NULL; link = link->next)
	{
		if (json_strequal(key, link->key))
		{
			if (prev != NULL)
				prev->next = link->next;
			else // first/head element
				obj->buckets[bucket_num] = link->next;
			json_bucket_link_free(link);
			obj->num_elements--;
			return true;
		}
		prev = link;
	}

	return false;
}

void json_object_debug_hash(JSON_Object *obj)
{
	double load_factor;
	assert(JSON_IS_OBJECT(obj));
	load_factor = json_object_get_load_factor(obj) * 100.0;
	json_print("JSON_Object Debug %p", (void*) obj);
	json_print("-------------------------------------");
	json_print("  Num Buckets: %u", obj->num_buckets);
	json_print("  Num Elements: %u", obj->num_elements);
	json_print("  Load factor: %f %%", load_factor);
}


struct JSON_ObjectClass
{
	JSON_ValueClass base__;
};

void *json_object_get_class(void)
{
	static struct JSON_ObjectClass json_object_class = { {
		sizeof(JSON_Object),
		json_object_free,
		json_object_clone,
		json_object_equal,
		json_object_to_string,
	} };
	return &json_object_class;
}
