#ifndef PAWN_ARRAY_H
#define PAWN_ARRAY_H

#include "common.h"

template <typename T>
struct Array {
    Allocator *allocator;
    T *data;
    s64 capacity;
    s64 size;
};

template <typename T>
Array<T> new_array(Allocator *allocator, s64 capacity) {
    Array<T> array;
    array.allocator = allocator;
    array.data = ALLOC(allocator, capacity, T);
    array.capacity = capacity;
    array.size = 0;
    return array;
}

template <typename T>
s64 resize(Array<T> *array, s64 new_capacity) {
    array->data = REALLOC(array->allocator, array->data, array->capacity, new_capacity, T);
    array->capacity = new_capacity;
    return array->capacity;
}

template <typename T>
bool add(Array<T> *array, T item) {
    if (array->size + 1 > array->capacity)  return false;

    array->data[array->size] = item;
    array->size++;
    return true;
}

template <typename T>
T pop(Array<T> *array) {
    T item = { };
    if (array->size < 1)  return item;

    item = array->data[size];
    array->size--;
    return item;
}

template <typename T>
void clear(Array<T> *array) {
    array->size = 0;
}

template <typename T>
bool free(Array<T> *array) {
    if (!array->data)         return false;
    if (array->capacity < 1)  return false;

    FREE(array->allocator, array->data);
    array->size = 0;
    array->capacity = 0;
    return true;
}

template <typename T>
bool contains(Array<T> *array, T item) {
    for (int index = 0; index < array->size; index++) {
        if (array->data[index] == item)  return true;
    }
    return false;
}

template <typename T>
T* find(Array<T> *array, T item) {
    for (int index = 0; index < array->size; index++) {
        if (array->data[index] == item)  return *array->data[index];
    }
    return NULL;
}

void array_test();

#endif /* PAWN_ARRAY_H */
