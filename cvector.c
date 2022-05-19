#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "cvector.h"

// Create a Vec with a default size.
Vec *Vec_create_with_size(size_t obj_size, size_t initial_size) {
	Vec *ret = malloc(sizeof(Vec) + obj_size);
	*ret = (Vec) {
		.data = malloc(initial_size * obj_size),
		.cap = initial_size,
		.size = 0,
		.obj_size = obj_size,
		.initial_size = initial_size
	};
	
	return ret;
}

// Takes the object size as a parameter
Vec *Vec_create(size_t obj_size) {
	return Vec_create_with_size(obj_size, 8);
}

// Takes the object and a custom free function, or NULL if one is not required.
void Vec_destroy(Vec *this, Vec_free_t free_func) {
	if (free_func != NULL) {
		for (void *obj = this; obj < this->data + this->obj_size * this->size; obj += this->obj_size) {
			free_func(obj);
		}
	}
	
	free(this->data);
}

// TODO: Use another algorithm to do this logarithmically.
static int Vec_realloc(Vec *this) {
	void *new_array = this->data;

	if (this->size == this->cap) {
		new_array = reallocarray(this->data, this->cap += 8, this->obj_size);
	} else if (this->size > this->initial_size && this->size < this->cap - 16) {
		new_array = reallocarray(this->data, this->cap -= 16, this->obj_size);
	}

	if (new_array == NULL) {
		this->error = "Failed to reallocate vector.";
		return 0;
	} else {
		this->data = new_array;
		return 1;
	}
}

void *Vec_get(Vec *this, size_t index) {
	if (index >= this->size) {
		this->error = "Attempted to access invalid index of a vector.";
		return NULL;
	} else {
		return this->data + this->obj_size * index;
	}
}

int Vec_set(Vec *this, size_t index, void *new_obj) {
	if (index >= this->size) {
		this->error = "Attempted to access invalid index %zu of a vector of size %zu.";
		return 0;
	} else {
		memcpy(this->data + this->obj_size * index, new_obj, this->obj_size);
		return 1;
	}
	
}

int Vec_insert(Vec *this, void *new_obj, size_t index) {
	if (!Vec_realloc(this)) return 0; // Realloc if necessary
	
	size_t copy_size = (this->size++ - index) * this->obj_size;
	memmove(this->data + this->obj_size * (index + 1), this->data + this->obj_size * index, copy_size);
	memcpy(this->data + this->obj_size * index, new_obj, this->obj_size);

	return 1;
}

int Vec_remove(Vec *this, size_t index) {
	if (index >= this->size) {
		this->error = "Attempted to remove an item from an invalid index.";
		return 0;
	}
	
	if (!Vec_realloc(this)) return 0; // Realloc if necessary
	size_t copy_size = (this->size-- - index) * this->obj_size;
	memmove(this->data + this->obj_size * index, this->data + this->obj_size * (index + 1), copy_size);
	
	return 1;
}

int Vec_push(Vec *this, void *new_obj) {
	return Vec_insert(this, new_obj, this->size);
}

void *Vec_peek(Vec *this) {
	return Vec_get(this, this->size - 1);
}

void *Vec_pop(Vec *this) {
	if (this->size == 0) {
		this->error = "Attempted to remove an item from an empty vector.";
		return NULL;
	}
	
	Vec_realloc(this);
	return memcpy(this->popbuf, this->data + this->obj_size * --this->size, this->obj_size);
}

const char *Vec_error(Vec *this) {
	return this->error;
}
