# cvector

A vector type for C. Not compatible with C++.

To use this in your own projects, include the source and header file pair as you would one of your own files!

There are a few ways to initialize a vector. The easiest way is to use the `Vec(datatype)` macro, where you can just provide your datatype. (no need for `sizeof()`)
This is just a call to the `Vec_create(size_t obj_size)` function, so you can use that instead if you would like.
You can also provide a preallocated initial size for the vector using the `Vec_create_with_size(size_t obj_size)` function.
