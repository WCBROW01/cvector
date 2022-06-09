#include <stdlib.h>
#include <string.h>

#include "cvector.h"

struct vec_internal {
	Vec ext;
	size_t cap, obj_size, initial_len;
	const char *error;
	char popbuf[]; // intermediate buffer for popped items.
};

// Create a Vec with a default size.
Vec *Vec_create_with_size(size_t obj_size, size_t initial_len) {
	struct vec_internal *ret = malloc(sizeof(struct vec_internal) + obj_size);
	*ret = (struct vec_internal) {
		.ext = {
			.data = malloc(initial_len * obj_size),
			.len = 0
		},
		.cap = initial_len,
		.obj_size = obj_size,
		.initial_len = initial_len
	};
	
	return (Vec*) ret;
}

// Takes the object size as a parameter
Vec *Vec_create(size_t obj_size) {
	return Vec_create_with_size(obj_size, 4);
}

// Takes the object and a custom free function, or NULL if one is not required.
void Vec_destroy(Vec *this, Vec_free_t free_func) {
	struct vec_internal *internal = (struct vec_internal*) this;
	
	if (free_func != NULL) {
		for (
			void *obj = this->data;
			obj < this->data + internal->obj_size * this->len;
			obj += internal->obj_size
		) free_func(obj);

		if (internal->popbuf != NULL) free_func(internal->popbuf);
	}
	
	free(this->data);
	free(this);
}

static int log2i(size_t x) {
	int i;
	for (i = 0; x > 0; x >>= 1, ++i);
	return i - 1;
}

Vec *Vec_copy(Vec *this, Vec_copy_t copy_func) {
	struct vec_internal *internal = (struct vec_internal*) this;

	struct vec_internal *ret = malloc(sizeof(struct vec_internal) + internal->obj_size);
	memcpy(ret, this, sizeof(struct vec_internal) + internal->obj_size);
	ret->ext.data = malloc(ret->cap * ret->obj_size);
	
	if (copy_func != NULL) {
		for (size_t i = 0; i < this->len; ++i)
			copy_func(ret->ext.data + i * ret->obj_size, this->data + i * ret->obj_size);
	} else {
		memcpy(ret->ext.data, this->data, ret->cap * ret->obj_size);
	}
	
	return (Vec*) ret;
}

static int Vec_realloc(Vec *this) {
	struct vec_internal *internal = (struct vec_internal*) this;
	
	void *new_array = this->data;

	if (this->len == internal->cap) {
		internal->cap += this->len == 1 ? 1 : log2i(this->len);
		new_array = reallocarray(this->data, internal->cap, internal->obj_size);
	} else if (
		this->len > internal->initial_len &&
		this->len < internal->cap - log2i(internal->cap) * 2
	) {
		new_array = reallocarray(this->data, internal->cap = this->len, internal->obj_size);
	}

	if (new_array == NULL) {
		internal->error = "Failed to reallocate vector.";
		return 0;
	} else {
		this->data = new_array;
		return 1;
	}
}

void *Vec_get(Vec *this, size_t index) {
	struct vec_internal *internal = (struct vec_internal*) this;

	if (index >= this->len) {
		internal->error = "Attempted to access invalid index of a vector.";
		return NULL;
	} else {
		return this->data + internal->obj_size * index;
	}
}

int Vec_set(Vec *this, size_t index, void *new_obj) {
	struct vec_internal *internal = (struct vec_internal*) this;
	
	if (index >= this->len) {
		internal->error = "Attempted to access invalid index of a vector.";
		return 0;
	} else {
		memcpy(this->data + internal->obj_size * index, new_obj, internal->obj_size);
		return 1;
	}
	
}

int Vec_insert(Vec *this, void *new_obj, size_t index) {
	struct vec_internal *internal = (struct vec_internal*) this;
	
	if (!Vec_realloc(this)) {
		return 0;
	} else {	
		size_t copy_size = (this->len++ - index) * internal->obj_size;
		memmove(this->data + internal->obj_size * (index + 1), this->data + internal->obj_size * index, copy_size);
		memcpy(this->data + internal->obj_size * index, new_obj, internal->obj_size);
		return 1;
	}
	
}

void *Vec_remove(Vec *this, size_t index) {
	struct vec_internal *internal = (struct vec_internal*) this;

	if (index >= this->len) {
		internal->error = "Attempted to remove an item from an invalid index.";
		return NULL;
	} else if (!Vec_realloc(this)) {
		return NULL;
	} else {
		memcpy(internal->popbuf, this->data + internal->obj_size * index, internal->obj_size);
		size_t copy_size = (this->len-- - index) * internal->obj_size;
		memmove(this->data + internal->obj_size * index, this->data + internal->obj_size * (index + 1), copy_size);
		return internal->popbuf;
	}
}

int Vec_push(Vec *this, void *new_obj) {
	return Vec_insert(this, new_obj, this->len);
}

void *Vec_peek(Vec *this) {
	return Vec_get(this, this->len - 1);
}

void *Vec_pop(Vec *this) {
	struct vec_internal *internal = (struct vec_internal*) this;

	if (this->len == 0) {
		internal->error = "Attempted to remove an item from an empty vector.";
		return NULL;
	} else if (!Vec_realloc(this)) {
		return NULL;
	} else {	
		return memcpy(internal->popbuf, this->data + internal->obj_size * --this->len, internal->obj_size);
	}
}

const char *Vec_error(Vec *this) {
	struct vec_internal *internal = (struct vec_internal*) this;

	return internal->error;
}
