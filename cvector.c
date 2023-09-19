#include <stdlib.h>
#include <string.h>

#include "cvector.h"

struct vec_internal {
	Vec_generic ext;
	size_t cap, obj_size;
	char popbuf[]; // intermediate buffer for popped items.
};

// Create a Vec_generic with enough memory for initial_cap items
Vec_generic *Vec_generic_create_with_cap(size_t obj_size, size_t initial_cap) {
	struct vec_internal *ret = malloc(sizeof(struct vec_internal) + obj_size);
	*ret = (struct vec_internal) {
		.ext = {
			.data = NULL,
			.len = 0
		},
		.cap = initial_cap,
		.obj_size = obj_size
	};
	
	return (Vec_generic*) ret;
}

// Create a Vec_generic with the contents of an array. Supports deep copies.
Vec_generic *Vec_generic_create_from_array(void *src, Vec_copy_t copy_func, size_t obj_size, size_t len) {
	Vec_generic *ret = Vec_generic_create_with_cap(obj_size, RESIZE_CONSTANT);
	if (Vec_generic_resize(ret, len)) {
		if (copy_func != NULL) {
			for (size_t i = 0; i < len; ++i)
				copy_func(ret->data + i * obj_size, ret->data + i * obj_size);
		} else {
			memcpy(ret->data, src, len * obj_size);
		}
		
		return ret;
	} else {
		Vec_generic_destroy(ret, NULL);
		return NULL;
	}
}

// Takes the object and a custom free function, or NULL if one is not required.
void Vec_generic_destroy(Vec_generic *this, Vec_free_t free_func) {
	struct vec_internal *internal = (struct vec_internal*) this;
	
	if (free_func != NULL) {
		for (
			void *obj = this->data;
			obj < this->data + internal->obj_size * this->len;
			obj += internal->obj_size
		) free_func(obj);
	}
	
	free(this->data);
	free(this);
}

Vec_generic *Vec_generic_copy(Vec_generic *this, Vec_copy_t copy_func) {
	struct vec_internal *internal = (struct vec_internal*) this;

	struct vec_internal *ret = malloc(sizeof(struct vec_internal) + internal->obj_size);
	memcpy(ret, this, sizeof(struct vec_internal) + internal->obj_size);
	ret->ext.data = malloc(ret->cap * ret->obj_size);
	
	if (copy_func != NULL) {
		for (size_t i = 0; i < this->len; ++i)
			copy_func(ret->ext.data + i * ret->obj_size, this->data + i * ret->obj_size);
	} else {
		memcpy(ret->ext.data, this->data, ret->ext.len * ret->obj_size);
	}
	
	return (Vec_generic*) ret;
}

static int Vec_generic_realloc(Vec_generic *this) {
	struct vec_internal *internal = (struct vec_internal*) this;
	
	void *new_array = this->data;

	if (!this->data) {
		new_array = malloc(internal->cap * internal->obj_size);
	} else if (this->len == internal->cap) {
		new_array = realloc(this->data, (internal->cap += RESIZE_CONSTANT) * internal->obj_size);
	} else if (this->len > RESIZE_CONSTANT * 2 && this->len < internal->cap - RESIZE_CONSTANT * 2) {
		new_array = realloc(this->data, (internal->cap = this->len) * internal->obj_size);
	}

	if (!new_array) {
		return 0;
	} else {
		this->data = new_array;
		return 1;
	}
}

int Vec_generic_resize(Vec_generic *this, size_t len) {
	size_t old_len = this->len;
	this->len = len;
	int status = Vec_generic_realloc(this);
	if (!status) this->len = old_len;
	return status;
}

int Vec_generic_insert(Vec_generic *this, size_t index, void *new_obj) {
	struct vec_internal *internal = (struct vec_internal*) this;
	
	if (!Vec_generic_realloc(this)) {
		return 0;
	} else {
		size_t copy_size = (this->len++ - index) * internal->obj_size;
		memmove(this->data + internal->obj_size * (index + 1), this->data + internal->obj_size * index, copy_size);
		memcpy(this->data + internal->obj_size * index, new_obj, internal->obj_size);
		return 1;
	}
	
}

void *Vec_generic_remove(Vec_generic *this, size_t index) {
	struct vec_internal *internal = (struct vec_internal*) this;

	if (index >= this->len || !Vec_generic_realloc(this)) {
		return NULL;
	} else {
		memcpy(internal->popbuf, this->data + internal->obj_size * index, internal->obj_size);
		size_t copy_size = (this->len-- - index) * internal->obj_size;
		memmove(this->data + internal->obj_size * index, this->data + internal->obj_size * (index + 1), copy_size);
		return internal->popbuf;
	}
}
