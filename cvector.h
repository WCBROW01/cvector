#ifndef CVECTOR_H
#define CVECTOR_H

// Macro so you can provide just the datatype without having to use sizeof()
#define Vec(obj) Vec_create(sizeof(obj))

// data is just an array, so you can cast it and use it as such.
typedef struct {
	void *data;
	size_t cap, size, obj_size, initial_size;
} Vec;

typedef void (*Vec_free_t)(void*);

// Create a Vec with a default size.
Vec *Vec_create_with_size(size_t obj_size, size_t initial_size);

// Takes the object size as a parameter
Vec *Vec_create(size_t obj_size);

// Takes the object and a custom free function, or NULL if one is not required.
void Vec_destroy(Vec *this, Vec_free_t free_func);

void Vec_push(Vec *this, void *new_obj);

void Vec_insert(Vec *this, void *new_obj, size_t index);

void Vec_remove(Vec *this, size_t index);

void *Vec_get(Vec *this, size_t index);

void Vec_set(Vec *this, size_t index, void *new_obj);

#endif
