#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>

#include "lib/input_response_mapping.h"


int main() {
    input_response_mapping_t pitch, roll, yaw;
    INPUT_RESPONSE_MAPPING_init(&pitch, 1.0f, 1.0f, 0.05f, 0.0f, 0.5f);
    INPUT_RESPONSE_MAPPING_init(&roll, 1.0f, 1.0f, 0.05f, 0.0f, 0.5f);
    INPUT_RESPONSE_MAPPING_init(&yaw, 1.0f, 1.0f, 0.05f, 0.0f, 0.5f);

    return 0;
}
