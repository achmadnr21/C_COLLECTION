#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
// file IO
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "lib/input_response_mapping.h"


int main() {
    input_response_mapping_t roll;
    INPUT_RESPONSE_MAPPING_init(&roll, 20.0f, 0.0f, 0.0f, 0.0f, 0.5f);
    roll.set_gain(&roll, 20.0f);
    roll.set_rate(&roll, 0.9f);
    roll.set_deadzone(&roll, 0.6f);
    roll.set_offset(&roll, 0.0f);
    roll.set_k(&roll, 0.5f);
    roll.info(&roll);
    
    int fd = open("response.csv", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }
    // simulate input from -1.0 to 1.0
    dprintf(fd, "input,response\n");
    for (float input = -1.0f; input <= 1.0f; input += 0.01f) {
        float response = roll.get_response(&roll, input);
        dprintf(fd, "%5.2f,%5.3f\n", input, response);
    }

    close(fd);

    system("python visualize.py");

    
    return 0;
}
