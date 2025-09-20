#ifndef PID_H
#define PID_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>

typedef struct _pid_controller {
    float kp; // Proportional gain
    float ki; // Integral gain
    float kd; // Derivative gain

    float previous_error; // Previous error value
    float integral;       // Integral of the error

    float output_min; // Minimum output limit
    float output_max; // Maximum output limit

    float dt; // Time step

    float (*compute_pi)(struct _pid_controller* pid, float setpoint, float measured_value);
    float (*compute_pd)(struct _pid_controller* pid, float setpoint, float measured_value);
    float (*compute_pid)(struct _pid_controller* pid, float setpoint, float measured_value);
    void  (*reset)(struct _pid_controller* pid);
    void (*info)(struct _pid_controller* pid);
} pid_controller_t;

void PID_init(pid_controller_t* pid, float kp, float ki, float kd, float output_min, float output_max, float dt);

#endif