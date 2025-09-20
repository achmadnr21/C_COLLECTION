#ifndef VECTOR_H
#define VECTOR_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>

// TO STRING TYPEDEF
typedef char* (*to_string)(void*);

// MAIN VECTOR STRUCTURE
typedef struct _vector {
    void*     _datas;
    
    uint64_t  _size;
    uint64_t  _allocated_size;


    uint32_t _dtype_memsize;
    to_string _dtype_to_string_func;

    float _growth_factor;
    
    void (*push_back) (struct _vector* self, void* data);
    void (*pop_back)  (struct _vector* self);
    void (*print_all) (struct _vector* self);
    void (*destroy)   (struct _vector* self);
    void* (*get_at)(struct _vector* self, uint64_t index);
} vector_t;

void VECTOR_init(vector_t* root, to_string custom_to_string_func, uint32_t memsize, float growth_factor);

#endif