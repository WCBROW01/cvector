#include <stdlib.h>
#include <string.h>

#include "cvector.h"

// Create a Vec with a default size.
Vec *Vec_create_with_size(size_t obj_size, size_t initial_size) {
	Vec *ret = malloc(sizeof(Vec));
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
static void Vec_realloc(Vec *this) {
	if (this->size == this->cap) {
		this->data = reallocarray(this->data, this->cap += 8, this->obj_size);
	} else if (this->size > this->initial_size && this->size < this->cap - 16) {
		this->data = reallocarray(this->data, this->cap -= 16, this->obj_size);
	}
}

void Vec_push(Vec *this, void *new_obj) {
	Vec_realloc(this); // Realloc if necessary
	memcpy(this->data + this->size++, new_obj, this->obj_size);
}

void Vec_insert(Vec *this, void *new_obj, size_t index) {
	Vec_realloc(this); // Realloc if necessary
	size_t copy_size = (this->size++ - index) * this->obj_size;
	memmove(this->data + index + 1, this->data + index, copy_size);
	memcpy(this->data + index, new_obj, this->obj_size);
}

void Vec_remove(Vec *this, size_t index) {
	Vec_realloc(this); // Realloc if necessary
	size_t copy_size = (this->size-- - index) * this->obj_size;
	memmove(this->data + index, this->data + index + 1, copy_size);
}

void *Vec_get(Vec *this, size_t index) {
	return this->data + index;
}

void Vec_set(Vec *this, size_t index, void *new_obj) {
	memcpy(this->data + index, new_obj, this->obj_size);
}
