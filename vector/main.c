#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>

typedef struct _orientation{
    float pitch, roll, azimuth;
} orientation_t;

char* ORIENTATION_to_string(void* data){
    orientation_t value = *((orientation_t*)data);
    char* buffer = malloc(64 * sizeof(char));
    if (buffer == NULL) {
        return NULL;
    }
    snprintf(buffer, 64, "{ pitch: %.3f, roll: %.3f, azimuth: %.3f }", value.pitch, value.roll, value.azimuth);
    return buffer;
}

// example for INT32
char* INT32_to_string(void* data){
    int32_t value = *((int32_t*)data);
    char* buffer = malloc(12 * sizeof(char));
    if (buffer == NULL) {
        return NULL;
    }
    snprintf(buffer, 12, "%d", value);
    return buffer;
}

// TO STRING TYPEDEF
typedef char* (*to_string)(void*);

// data_type_registration
typedef enum _dtype{
    INT32,
    ORIENTATION
} data_type_t;


typedef struct _data_type_info{
    uint32_t memsize;
    to_string to_string_func;
}data_type_info_t;


data_type_info_t get_data_type_info(data_type_t dtype){
    switch(dtype){
        case INT32: return (data_type_info_t){.memsize = sizeof(int32_t), .to_string_func = INT32_to_string};
        case ORIENTATION: return (data_type_info_t){.memsize = sizeof(orientation_t), .to_string_func = ORIENTATION_to_string};
        default:
            fprintf(stderr, "[ERROR] Unknown data type!\n");
            exit(EXIT_FAILURE);
    }
}

void to_string_cleanup(void* data){
    free(data);
}

typedef struct _vector {
    void*     _datas;
    
    uint64_t  _size;
    uint64_t  _allocated_size;
    
    data_type_info_t _dtype_info;

    float _growth_factor;
    
    void (*push_back) (struct _vector* self, void* data);
    void (*pop_back)  (struct _vector* self);
    void (*print_all) (struct _vector* self);
    void (*destroy)   (struct _vector* self);
    // void (*get_at)(struct _vector* self, uint64_t index);
} vector_t;

void VECTOR_push_back(vector_t* self, void* data){
    if(self->_size * self->_growth_factor >= self->_allocated_size){
        self->_allocated_size *= self->_growth_factor + 1;
        void* tmp = realloc(self->_datas, self->_allocated_size * self->_dtype_info.memsize);
        if (!tmp) {
            fprintf(stderr, "[ERROR] realloc failed!\n");
            exit(EXIT_FAILURE);
        }
        self->_datas = tmp;
    }
    memcpy((char*)self->_datas + self->_size * self->_dtype_info.memsize, data, self->_dtype_info.memsize);
    self->_size++;
}


void VECTOR_print_all (vector_t* self){
    for(int i = 0; i < self->_size; i++){
        char* str = self->_dtype_info.to_string_func((char*)self->_datas + i * self->_dtype_info.memsize);
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

void VECTOR_init(vector_t* root, data_type_t dtype, float growth_factor){
    root->_growth_factor = growth_factor;
    root->_datas = NULL;
    root->_dtype_info = get_data_type_info(dtype);
    root->_allocated_size = 1<<2;
    root->_datas = (void*)malloc(root->_allocated_size * root->_dtype_info.memsize);
    if (!root->_datas) {
        fprintf(stderr, "[ERROR] malloc failed!\n");
        exit(EXIT_FAILURE);
    }
    root->_size = 0;
    root->push_back = VECTOR_push_back;
    root->pop_back = VECTOR_pop_back;
    root->print_all = VECTOR_print_all;
    root->destroy = VECTOR_destroy;
}

int main() {
    vector_t vec;
    VECTOR_init(&vec, ORIENTATION, 1.10f);
    for(int i = 0; i < 10000; i++){
        orientation_t data = { .pitch = i * 1.0f, .roll = i * 0.8f, .azimuth = i * 0.6f };
        vec.push_back(&vec, &data);
    }
    vec.pop_back(&vec);

    vec.print_all(&vec);
    vec.destroy(&vec);
    return 0;
}
