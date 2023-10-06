#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAY_BE_UNUSED __attribute__((unused))

struct vector;

static bool vector_reserve(struct vector* vector_ptr, size_t new_capacity);
struct vector* vector_new(size_t initial_size, size_t item_size);
struct vector* vector_delete(struct vector* vector_ptr);

void* vector_get_item_ptr(struct vector* vector_ptr, size_t index);
void const* vector_get_const_item_ptr(struct vector const* vector_ptr, size_t index);

void vector_set(struct vector* vector_ptr, size_t index, void const* item_ptr);
void vector_get(struct vector const* vector_ptr, size_t index, void* item_ptr);

bool vector_check_empty(struct vector const* vector_ptr);
size_t vector_get_size(struct vector const* vector_ptr);

bool vector_pop(struct vector* vector_ptr);
size_t vector_push(struct vector* vector_ptr, const void* item_ptr);

void vector_print(struct vector const* vector_ptr, void (*print_function)(void const* item_ptr));
