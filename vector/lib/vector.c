#include "vector.h"

void to_string_cleanup(void* data){
    free(data);
}




void VECTOR_push_back(vector_t* self, void* data){
    if (self->_size * self->_growth_factor >= self->_allocated_size) {
        // Double the allocated size for safe growth
        self->_allocated_size *= self->_growth_factor + 1;
        void* tmp = realloc(self->_datas, self->_allocated_size * self->_dtype_memsize);
        if (!tmp) {
            fprintf(stderr, "[ERROR] realloc failed!\n");
            exit(EXIT_FAILURE);
        }
        self->_datas = tmp;
    }
    memcpy((char*)self->_datas + self->_size * self->_dtype_memsize, data, self->_dtype_memsize);
    self->_size++;
}


void VECTOR_print_all (vector_t* self){
    for(uint64_t i = 0; i < self->_size; i++){
        char* str = self->_dtype_to_string_func((char*)self->_datas + i * self->_dtype_memsize);
        if (str) {
            printf("%s\n", str);
            to_string_cleanup(str);
        }
    }
}

void VECTOR_pop_back (vector_t* self){
    if(self->_size > 0){
        self->_size--;
    }
}

void VECTOR_destroy(vector_t* self){
    free(self->_datas);
}

void* VECTOR_get_at(vector_t* self, uint64_t index){
    if(index >= self->_size){
        return NULL;
    }
    return (char*)self->_datas + index * self->_dtype_memsize;
}

void VECTOR_init(vector_t* root, to_string custom_to_string_func, uint32_t memsize, float growth_factor){
    root->_growth_factor = 1.0+growth_factor;
    root->_datas = NULL;
    root->_dtype_to_string_func = custom_to_string_func;
    root->_dtype_memsize = memsize;
    root->_allocated_size = 1<<2;
    root->_datas = (void*)malloc(root->_allocated_size * root->_dtype_memsize);

    if (!root->_datas) {
        fprintf(stderr, "[ERROR] malloc failed!\n");
        exit(EXIT_FAILURE);
    }
    root->_size = 0;
    root->push_back = VECTOR_push_back;
    root->pop_back = VECTOR_pop_back;
    root->print_all = VECTOR_print_all;
    root->destroy = VECTOR_destroy;
    root->get_at = VECTOR_get_at;
}