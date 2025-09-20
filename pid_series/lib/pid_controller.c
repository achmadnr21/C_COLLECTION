#include "pid_controller.h"


// ================================= PID CONTROLLER PROTOTYPES ==============================

static float PID_CONTROLLER_compute_pi(pid_controller_t* pid, float setpoint, float measured_value);
static float PID_CONTROLLER_compute_pid(pid_controller_t* pid, float setpoint, float measured_value);
static float PID_CONTROLLER_compute_pd(pid_controller_t* pid, float setpoint, float measured_value);
static void  PID_CONTROLLER_reset(pid_controller_t* pid);
static void  PID_CONTROLLER_info(pid_controller_t* pid);


// ================================= PID CONTROLLER IMPLEMENTATION ==============================

float PID_CONTROLLER_compute_pi(pid_controller_t* pid, float setpoint, float measured_value){
    float error = setpoint - measured_value;
    pid->integral += error * pid->dt;
    float output = pid->kp * error + pid->ki * pid->integral;

    // Clamp output to min/max
    if (output > pid->output_max) output = pid->output_max;
    if (output < pid->output_min) output = pid->output_min;

    return output;
}

float PID_CONTROLLER_compute_pd(pid_controller_t* pid, float setpoint, float measured_value){
    float error = setpoint - measured_value;
    float derivative = (error - pid->previous_error) / pid->dt;
    pid->previous_error = error;
    float output = pid->kp * error + pid->kd * derivative;

    // Clamp output to min/max
    if (output > pid->output_max) output = pid->output_max;
    if (output < pid->output_min) output = pid->output_min;

    return output;
}

float PID_CONTROLLER_compute_pid(pid_controller_t* pid, float setpoint, float measured_value){
    float error = setpoint - measured_value;
    pid->integral += error * pid->dt;
    float derivative = (error - pid->previous_error) / pid->dt;
    pid->previous_error = error;
    float output = pid->kp * error + pid->ki * pid->integral + pid->kd * derivative;

    // Clamp output to min/max
    if (output > pid->output_max) output = pid->output_max;
    if (output < pid->output_min) output = pid->output_min;

    return output;
}

void PID_CONTROLLER_reset(pid_controller_t* pid){
    pid->previous_error = 0.0f;
    pid->integral = 0.0f;
}

void PID_CONTROLLER_info(pid_controller_t* pid){
    printf("===========================================\n");
    printf("PID Controller Info:\n");
    printf("Kp             : %.3f\n", pid->kp);
    printf("Ki             : %.3f\n", pid->ki);
    printf("Kd             : %.3f\n", pid->kd);
    printf("Output Limits. : (%.3f <= output <= %.3f)\n", pid->output_min, pid->output_max);
    printf("Time Step (dt) : %.3f seconds\n", pid->dt);
    printf("Previous Error : %.3f\n", pid->previous_error);
    printf("Integral.      : %.3f\n", pid->integral);
    /*
    I wanna print into the console an information like, the best step to tune the PID controller is 
    starting from the Kp, then Kd, and the last is Ki.  But in pretty format.
    */
    printf("================== HINTS ==================\n");
    printf("Tuning Recommendation:\n-> Start with Kp,\n-> then Kd,\n-> and finally Ki.\n");
    printf("===========================================\n");

}

void PID_CONTROLLER_init(pid_controller_t* pid, float kp, float ki, float kd, float output_min, float output_max, float dt){
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->output_min = output_min;
    pid->output_max = output_max;
    pid->dt = dt;

    pid->previous_error = 0.0f;
    pid->integral = 0.0f;

    pid->compute_pi = PID_CONTROLLER_compute_pi;
    pid->compute_pd = PID_CONTROLLER_compute_pd;
    pid->compute_pid = PID_CONTROLLER_compute_pid;
    pid->reset = PID_CONTROLLER_reset;
    pid->info = PID_CONTROLLER_info;
}

