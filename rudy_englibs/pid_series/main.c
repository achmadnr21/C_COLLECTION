#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>

#include "lib/pid_controller.h"

#define SIMULATION_SYSTEM_FEEDBACK 1.3f
#define SIMULATION_TIME 3.0f // seconds


int main() {
    pid_controller_t pid;

    float kp = 1.2f;
    float kd = 0.06f;
    float ki = 0.03f;

    float output_min = -10.0f;
    float output_max = 10.0f;
    float dt = 0.1f; // 100 ms time step

    float anti_windup_factor = 0.1f; // Anti-windup factor
    float derivative_filter_coefficient = 10.0f; // Derivative filter coefficient
    PID_CONTROLLER_init(&pid, kp, ki, kd, output_min, output_max, dt, anti_windup_factor, derivative_filter_coefficient);
    pid.info(&pid);


    // INITIAL STATE
    float measured_value = 0.0f;

    // SIMULATION LOOP - STEP RESPONSE
    // Setpoint changes from 0 to 50 at t=0
    float setpoint = 50.0f;
    int steps = (int)(SIMULATION_TIME / dt)+1;

    while(1){
        system("clear");
        printf("==========================================\n");
        pid.info(&pid);
        printf("=========> Simulation Start...\n");
        for(int i = 0; i < steps; i++){
            float output = pid.compute_pid(&pid, setpoint, measured_value);
            measured_value += output * SIMULATION_SYSTEM_FEEDBACK;
            // only print every 1 second
            if (i % 1 == 0) {
                printf("Time: %.1f s, Setpoint: %.2f, Measured: %.2f, Output: %.2f\n", i * pid.dt, setpoint, measured_value, output);
            }
        }
        printf("==> Current measured value: %.2f\n\n", measured_value);
        printf("==> Input Setpoint (or 'q' to quit): ");
        char input[20];
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break; // EOF or error
        }
        if (input[0] == 'q' || input[0] == 'Q') {
            break; // Exit the loop
        }
        float new_setpoint = atof(input);
        if (new_setpoint == 0 && input[0] != '0') {
            printf("====>Invalid input. Please enter a valid number.\n");
            continue; // Prompt again
        }
        setpoint = new_setpoint;
        
        
        printf("=========> Simulation End...\nCurrent measured value: %.2f\n", measured_value);
    }

    
    return 0;
}
