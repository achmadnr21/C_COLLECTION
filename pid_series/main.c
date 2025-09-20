#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>

#include "lib/pid_controller.h"

#define SIMULATION_SYSTEM_FEEDBACK 0.3f
#define SIMULATION_TIME 50.0f // seconds


int main() {
    pid_controller_t pid;

    float kp = 1.6f;
    float kd = 0.05f;
    float ki = 0.1f;

    float output_min = -10.0f;
    float output_max = 10.0f;
    float dt = 0.1f; // 100 ms time step
    PID_init(&pid, kp, ki, kd, output_min, output_max, dt);
    pid.info(&pid);


    // INITIAL STATE
    float measured_value = 0.0f;

    // SIMULATION LOOP - STEP RESPONSE
    // Setpoint changes from 0 to 50 at t=0
    float setpoint = 50.0f;
    int steps = (int)(SIMULATION_TIME / dt)+1;
    for(int i = 0; i < steps; i++){
        float output = pid.compute_pid(&pid, setpoint, measured_value);
        measured_value += output * SIMULATION_SYSTEM_FEEDBACK;
        // only print every 1 second
        if (i % 10 == 0) {
            printf("Time: %.1f s, Setpoint: %.2f, Measured: %.2f, Output: %.2f\n", i * pid.dt, setpoint, measured_value, output);
        }
    }

    // Change setpoint to -50
    setpoint = -20.0f;
    for(int i = 0; i < steps; i++){
        float output = pid.compute_pid(&pid, setpoint, measured_value);
        measured_value += output * SIMULATION_SYSTEM_FEEDBACK;
        // only print every 1 second
        if (i % 10 == 0) {
            printf("Time: %.1f s, Setpoint: %.2f, Measured: %.2f, Output: %.2f\n", i * pid.dt, setpoint, measured_value, output);
        }
    }

    pid.reset(&pid);

    
    return 0;
}
