#include "input_response_mapping.h"

// Prototype declarations for internal functions
static void INPUT_RESPONSE_MAPPING_set_gain(input_response_mapping_t* self, float gain);
static void INPUT_RESPONSE_MAPPING_set_rate(input_response_mapping_t* self, float rate);
static void INPUT_RESPONSE_MAPPING_set_deadzone(input_response_mapping_t* self, float deadzone);
static void INPUT_RESPONSE_MAPPING_set_offset(input_response_mapping_t* self, float offset);
static void INPUT_RESPONSE_MAPPING_set_k(input_response_mapping_t* self, float k);
static void INPUT_RESPONSE_MAPPING_info(input_response_mapping_t* self);
static float INPUT_RESPONSE_MAPPING_get_response(input_response_mapping_t* self, float input);


void INPUT_RESPONSE_MAPPING_init(input_response_mapping_t* self, float gain, float rate, float deadzone, float offset, float k) {
    self->gain = gain;
    self->rate = rate;
    self->deadzone = deadzone;
    self->offset = offset;
    self->k = k;

    self->set_gain = INPUT_RESPONSE_MAPPING_set_gain;
    self->set_rate = INPUT_RESPONSE_MAPPING_set_rate;
    self->set_deadzone = INPUT_RESPONSE_MAPPING_set_deadzone;
    self->set_offset = INPUT_RESPONSE_MAPPING_set_offset;
    self->set_k = INPUT_RESPONSE_MAPPING_set_k;
    self->info = INPUT_RESPONSE_MAPPING_info;
    self->get_response = INPUT_RESPONSE_MAPPING_get_response;
}

void INPUT_RESPONSE_MAPPING_set_gain(input_response_mapping_t* self, float gain) {
    // gain can only from 10 to 100
    if (gain < 10.0f) gain = 10.0f;
    if (gain > 100.0f) gain = 100.0f;
    self->gain = gain;
}
void INPUT_RESPONSE_MAPPING_set_rate(input_response_mapping_t* self, float rate) {
    // rate can only from 0.1 to 1.0
    if (rate < 0.1f) rate = 0.1f;
    if (rate > 1.0f) rate = 1.0f;
    self->rate = rate;
}
void INPUT_RESPONSE_MAPPING_set_deadzone(input_response_mapping_t* self, float deadzone) {
    // deadzone can only from 0.0 to 0.8
    if (deadzone < 0.0f) deadzone = 0.0f;
    if (deadzone > 0.8f) deadzone = 0.8f;
    self->deadzone = deadzone;
}
void INPUT_RESPONSE_MAPPING_set_offset(input_response_mapping_t* self, float offset) {
    // offset can only from -1.0 to 1.0
    if (offset < -1.0f) offset = -1.0f;
    if (offset > 1.0f) offset = 1.0f;
    self->offset = offset;
}
void INPUT_RESPONSE_MAPPING_set_k(input_response_mapping_t* self, float k) {
    // k can only from 0.1 to 0.9
    if (k < 0.1f) k = 0.1f;
    if (k > 0.9f) k = 0.9f;
    self->k = k;
}

void INPUT_RESPONSE_MAPPING_info(input_response_mapping_t* self) {
    printf("===========================================\n");
    printf(" Input Response Mapping Configuration \n");
    printf("===========================================\n");
    printf(" Gain     : %.3f\n", self->gain);
    printf(" Rate     : %.3f\n", self->rate);
    printf(" Deadzone : %.3f\n", self->deadzone);
    printf(" Offset   : %.3f\n", self->offset);
    printf(" K        : %.3f\n", self->k);

    printf("================== HINTS ==================\n");
    printf("Recommendation:\n");
    printf(" Gain     : Start with 20.0f (10.0f - 100.0f)\n");
    printf(" Rate     : Start with 1.0f  (0.1f - 1.0f)\n");
    printf(" Deadzone : Start with 0.00f (0.0f - 0.8f)\n");
    printf(" Offset   : Start with 0.0f  (-1.0f - 1.0f)\n");
    printf(" K        : Start with 0.5f  (0.1f - 0.9f)\n");
    printf("===========================================\n");
}

static inline float rudy_input_pipeline(
    float x,
    float g,
    float k,
    float r
    ){
        float g_pow = -(k-0.5);
        float x_pow = powf(g, g_pow);
        float response = powf(x, x_pow)*(x/fabsf(x));
        return response * r;
    }

float INPUT_RESPONSE_MAPPING_get_response(input_response_mapping_t* self, float input) {
    input += self->offset;
    if (fabsf(input) < self->deadzone) {
        return 0.0f;
    }
    if (fabsf(input) > 1.0f) {
        input = (input/fabsf(input)) * 1.0f;
    }
    return rudy_input_pipeline(input, self->gain, self->k, self->rate);
}
