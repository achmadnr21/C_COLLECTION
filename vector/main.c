#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>

#include "lib/vector.h"

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

int main() {
    vector_t vec;
    VECTOR_init(&vec, ORIENTATION_to_string, sizeof(orientation_t), 0.5f);

    for(uint64_t i = 0; i < 10000; i++){
        orientation_t data = { .pitch = i * 1.0f, .roll = i * 0.8f, .azimuth = i * 0.6f };
        vec.push_back(&vec, &data);
    }
    vec.pop_back(&vec);

    vec.print_all(&vec);

    int get_idx = 5000;
    orientation_t item = *((orientation_t*) vec.get_at(&vec, get_idx));

    printf("Item at index %d: { pitch: %.3f, roll: %.3f, azimuth: %.3f }\n", get_idx, item.pitch, item.roll, item.azimuth);
    vec.destroy(&vec);
    return 0;
}
