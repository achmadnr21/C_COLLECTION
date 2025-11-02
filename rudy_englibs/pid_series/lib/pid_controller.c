#include "pid_controller.h"

#include <math.h>
// ================================= PID CONTROLLER PROTOTYPES ==============================

static float PID_CONTROLLER_compute_pid(pid_controller_t* pid, float setpoint, float measured_value);
static void  PID_CONTROLLER_reset(pid_controller_t* pid);
static void  PID_CONTROLLER_info(pid_controller_t* pid);


// ================================= PID CONTROLLER IMPLEMENTATION ==============================

float PID_CONTROLLER_compute_pid(pid_controller_t* pid, float setpoint, float measured_value){
    float error = setpoint - measured_value;

    // 1. Proportional term
    float proportional_term = pid->kp * error;

    // 2. Integral term
    pid->integral += error * pid->dt;
    float integral_term = pid->ki * pid->integral;

    // 3. Derivative term
    float derivative = -(measured_value - pid->previous_measurement) / pid->dt;
    float N = pid->kd_N;
    pid->filtered_derivative = (derivative + (N-1)*pid->filtered_derivative) / N;
    float derivative_term = pid->kd * pid->filtered_derivative;

    // PID output before saturation
    float output_unsat = proportional_term + integral_term + derivative_term;
    float output = output_unsat;

    // Clamp output to min/max
    if (output_unsat > pid->output_max) output = pid->output_max;
    if (output_unsat < pid->output_min) output = pid->output_min;

    // Anti-windup: Adjust integral term if output is saturated
    if(pid->ki_aw!=0.0f 
        && fabsf(output - output_unsat) > 0.0001f
     ){
        float aw_correction = (output - output_unsat) * pid->ki_aw * pid->dt;
        pid->integral += aw_correction;
    }

    // update previous measurement
    pid->previous_measurement = measured_value;

    return output;
}

void PID_CONTROLLER_reset(pid_controller_t* pid){
    pid->previous_measurement = 0.0f;
    pid->integral = 0.0f;
}

void PID_CONTROLLER_info(pid_controller_t* pid){
    printf("===========================================\n");
    printf("PID Controller Info:\n");
    printf("Kp             : %.3f\n", pid->kp);
    printf("Ki             : %.3f\n", pid->ki);
    printf("Kd             : %.3f\n", pid->kd);
    printf("Output Limits  : (%.3f <= output <= %.3f)\n", pid->output_min, pid->output_max);
    printf("Time Step (dt) : %.3f seconds\n", pid->dt);
    printf("Prev Measurement: %.3f\n", pid->previous_measurement); // Ganti ini
    printf("Integral       : %.3f\n", pid->integral);
    printf("Anti-windup K  : %.3f\n", pid->ki_aw);
    printf("================== HINTS ==================\n");
    printf("• Using Derivative-on-Measurement (no kick)\n");
    printf("• Tuning: Start with Kp, then Kd, last Ki\n");
    printf("===========================================\n");
}

void PID_CONTROLLER_init(pid_controller_t* pid, float kp, float ki, float kd, float output_min, float output_max, float dt, float anti_windup_factor, float derivative_filter_coefficient){
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->ki_aw = anti_windup_factor * ki;
    pid->kd_N = derivative_filter_coefficient;
    pid->output_min = output_min;
    pid->output_max = output_max;
    pid->dt = dt;

    pid->previous_measurement = 0.0f;
    pid->integral = 0.0f;
    pid->filtered_derivative = 0.0f;

    pid->compute_pid = PID_CONTROLLER_compute_pid;
    pid->reset = PID_CONTROLLER_reset;
    pid->info = PID_CONTROLLER_info;
}

