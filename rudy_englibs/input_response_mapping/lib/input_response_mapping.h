#ifndef INPUT_RESPONSE_MAPPING_H
#define INPUT_RESPONSE_MAPPING_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

typedef struct _input_response_mapping {
    float gain;
    float rate;
    float deadzone;
    float offset;
    float k;
    void (*set_gain)(struct _input_response_mapping* self, float gain);
    void (*set_rate)(struct _input_response_mapping* self, float rate);
    void (*set_deadzone)(struct _input_response_mapping* self, float deadzone);
    void (*set_offset)(struct _input_response_mapping* self, float offset);
    void (*set_k)(struct _input_response_mapping* self, float k);
    void (*info)(struct _input_response_mapping* self);
    float (*get_response)(struct _input_response_mapping* self, float input);

} input_response_mapping_t;

void INPUT_RESPONSE_MAPPING_init(input_response_mapping_t* self, float gain, float rate, float deadzone, float offset, float k);

#endif