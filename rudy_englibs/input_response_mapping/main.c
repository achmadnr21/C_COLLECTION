#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>

#include "lib/input_response_mapping.h"


int main() {
    input_response_mapping_t roll;
    INPUT_RESPONSE_MAPPING_init(&roll, 1.0f, 1.0f, 0.05f, 0.0f, 0.5f);
    roll.info(&roll);

    // simulate input from -1.0 to 1.0
    for (float input = -1.0f; input <= 1.0f; input += 0.01f) {
        float response = roll.get_response(&roll, input);
        printf("j_input: %5.2f\t|sig_resp: %5.2f\n", input, response);
    }

    return 0;
}
