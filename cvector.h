#ifndef CVECTOR_H
#define CVECTOR_H

#define RESIZE_CONSTANT 4

// data is just an array, so you can use it as such.
#define Vec(obj_type) struct { \
	obj_type *data; \
	size_t len; \
}

typedef struct {
	void *data;
	size_t len;
} Vec_generic;

// void Vec_free_t(void *ptr);
typedef void (*Vec_free_t)(void*);

// void *Vec_copy_t(void *dest, void *src);
typedef void *(*Vec_copy_t)(void*, void*);

// Create a Vec with enough memory for initial_cap items
Vec_generic *Vec_generic_create_with_cap(size_t obj_size, size_t initial_cap);
#define Vec_create_with_cap(obj_type, initial_cap) (Vec(obj_type) *) Vec_generic_create_with_cap(sizeof(obj_type), initial_cap)

// Takes the object size as a parameter
#define Vec_create(obj_type) Vec_create_with_cap(obj_type, RESIZE_CONSTANT)

// Create a Vec with the contents of an array. Supports deep copies.
Vec_generic *Vec_generic_create_from_array(void *src, Vec_copy_t copy_func, size_t obj_size, size_t len);
#define Vec_create_from_array(src, copy_func, len) ((Vec(typeof(src[0])) *) Vec_generic_create_from_array(src, copy_func, sizeof(src[0]), len))

// Shortcut for stack arrays, since they usually have a stored length.
#define Vec_create_from_stack_array(src, copy_func) Vec_create_from_array(src, copy_func, sizeof(src) / sizeof(src[0]))

// Takes the object and a custom free function, or NULL if one is not required.
void Vec_generic_destroy(Vec_generic *this, Vec_free_t free_func);
#define Vec_destroy(this, free_func) Vec_generic_destroy((Vec_generic *) (this), free_func)

/* Creates a copy of a vector. If no copy function is provided, the vector
 * data will be copied as if memcpy() was used to copy the data. */
Vec_generic *Vec_generic_copy(Vec_generic *this, Vec_copy_t copy_func);
#define Vec_copy(this, copy_func) ((typeof(this)) Vec_generic_copy((Vec_generic *) (this), copy_func))

// Get an item from the desired index of a vector.
#define Vec_get(this, index) ((this)->data[index])

// Copy an item to the desired index of a vector.
#define Vec_set(this, index, new_obj) (((this)->data[index]) = new_obj)

/* Truncate or extend a vector to a given length. If the vector is extended,
 * the new data in the vector is uninitialized and the values are undefined.
 * However, you may initialize them yourself using memset. */
int Vec_generic_resize(Vec_generic *this, size_t len);
#define Vec_resize(this, len) Vec_generic_resize((Vec_generic *) (this), len)

// Insert an item at the desired index of a vector.
int Vec_generic_insert(Vec_generic *this, size_t index, void *new_obj);
#define Vec_insert(this, index, new_obj) Vec_generic_insert((Vec_generic *) (this), index, &(new_obj))

// Remove an item from the desired index of a vector
void *Vec_generic_remove(Vec_generic *this, size_t index);
#define Vec_remove(this, index) (*(typeof(this->data)) Vec_remove((Vec_generic *) (this), index))

// Push an item to the end of the vector.
#define Vec_push(this, new_obj) Vec_insert(this, new_obj, this->len)

// Get the item at the top of the vector.
#define Vec_peek(this) Vec_get(this, this->len - 1)

// Pops the last item from the vector and removes it.
#define Vec_pop(this) ((this)->data[--this->len])

#endif
