#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "lib/pid_controller.h"

#define SIMULATION_SYSTEM_FEEDBACK 1.3f
#define SIMULATION_TIME 20.0f // seconds

void print_simulation_header(float setpoint, float initial_measurement) {
    printf("🚀 PID SIMULATION - STEP RESPONSE ANALYSIS\n");
    printf("==========================================\n");
    printf("Setpoint Change: %.1f → %.1f\n", initial_measurement, setpoint);
    printf("Simulation Time: %.1f seconds | DT: %.1f ms\n", 
           SIMULATION_TIME, SIMULATION_TIME*1000);
    printf("System Feedback Gain: %.1f\n", SIMULATION_SYSTEM_FEEDBACK);
    printf("==========================================\n\n");
}

void print_performance_metrics(float* measurements, int steps, float setpoint, float dt) {
    float steady_state_error = setpoint - measurements[steps-1];
    float overshoot = 0.0f;
    float rise_time = 0.0f;
    float settling_time = 0.0f;
    
    for(int i = 0; i < steps; i++) {
        float current = measurements[i];
        if(current > setpoint && (current - setpoint) > overshoot) {
            overshoot = current - setpoint;
        }
        if(rise_time == 0.0f && current >= setpoint * 0.9f) {
            rise_time = i * dt;
        }
        if(fabsf(current - setpoint) <= setpoint * 0.02f) { // 2% settling band
            settling_time = i * dt;
            break;
        }
    }
    
    printf("\n📊 PERFORMANCE METRICS:\n");
    printf("──────────────────────────\n");
    printf("Steady-State Error: %7.3f\n", steady_state_error);
    printf("Overshoot:          %7.3f\n", overshoot);
    printf("Rise Time (90%%):    %7.3f s\n", rise_time);
    printf("Settling Time (2%%): %7.3f s\n", settling_time);
    printf("──────────────────────────\n");
}

int main() {
    pid_controller_t pid;

    // PID Parameters - bisa dijadikan user input juga
    float kp = 1.2f;
    float kd = 0.06f;
    float ki = 0.4f;

    float output_min = -10.0f;
    float output_max = 10.0f;
    float dt = 0.1f; // 100 ms time step

    float anti_windup_factor = 0.01f;
    float derivative_filter_coefficient = 10.0f;
    
    PID_CONTROLLER_init(&pid, kp, ki, kd, output_min, output_max, dt, 
                       anti_windup_factor, derivative_filter_coefficient);

    // INITIAL STATE
    float measured_value = 1000.0f;
    float initial_measurement = measured_value;

    // SIMULATION LOOP
    float setpoint = 700.0f;
    int steps = (int)(SIMULATION_TIME / dt) + 1;
    
    // Array untuk menyimpan data performance analysis
    float* measurement_history = malloc(steps * sizeof(float));
    float* output_history = malloc(steps * sizeof(float));

    while(1) {
        system("clear");
        
        // Print header info
        print_simulation_header(setpoint, initial_measurement);
        pid.info(&pid);
        
        printf("\n🎯 SIMULATION RESULTS:\n");
        printf("Time(s) | Setpoint | Measured | Output | Error\n");
        printf("────────┼──────────┼──────────┼────────┼────────\n");

        // Reset PID dan measurement untuk setiap simulasi baru
        pid.reset(&pid);
        measured_value = initial_measurement;

        // RUN SIMULATION
        for(int i = 0; i < steps; i++) {
            float output = pid.compute_pid(&pid, setpoint, measured_value);
            measured_value += output * SIMULATION_SYSTEM_FEEDBACK;
            
            // Store for analysis
            measurement_history[i] = measured_value;
            output_history[i] = output;
            
            // Print every 0.2 seconds for better readability
            if (i % 1 == 0) {
                float error = setpoint - measured_value;
                printf("%6.1f | %8.5f | %8.5f | %6.5f | %6.5f\n", 
                       i * pid.dt, setpoint, measured_value, output, error);
            }
        }

        // Print final state dengan highlight
        float final_error = setpoint - measured_value;
        printf("\n💡 FINAL STATE:\n");
        printf("Measured: %.2f | Error: %.2f | Status: ", measured_value, final_error);
        
        if(fabsf(final_error) < 0.5f) {
            printf("✅ EXCELLENT\n");
        } else if(fabsf(final_error) < 2.0f) {
            printf("⚠️  ACCEPTABLE\n");
        } else {
            printf("❌ NEEDS TUNING\n");
        }

        // Performance analysis
        print_performance_metrics(measurement_history, steps, setpoint, dt);

        // User interaction
        printf("\n🎮 CONTROLS:\n");
        printf("• Enter new setpoint to continue\n");
        printf("• 't' to tune PID parameters\n");  
        printf("• 'r' to reset to default\n");
        printf("• 's' to set initial measurement and setpoint\n");
        printf("• 'q' to quit\n");
        printf("\n==> Choose option: ");
        
        char input[50];
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }
        
        // Remove newline character
        input[strcspn(input, "\n")] = 0;
        
        if (input[0] == 'q' || input[0] == 'Q') {
            break;
        }
        else if (input[0] == 't' || input[0] == 'T') {
            printf("Enter new Kp, Ki, Kd (comma separated): ");
            if (fgets(input, sizeof(input), stdin)) {
                if (sscanf(input, "%f,%f,%f", &kp, &ki, &kd) == 3) {
                    PID_CONTROLLER_init(&pid, kp, ki, kd, output_min, output_max, dt,
                                      anti_windup_factor, derivative_filter_coefficient);
                    printf("✅ Parameters updated: Kp=%.3f, Ki=%.3f, Kd=%.3f\n", kp, ki, kd);
                } else {
                    printf("❌ Invalid format! Using previous values.\n");
                }
            }
            printf("Press Enter to continue...");
            fgets(input, sizeof(input), stdin);
        }
        else if (input[0] == 'r' || input[0] == 'R') {
            // Reset to default parameters
            kp = 1.2f; kd = 0.06f; ki = 0.03f;
            PID_CONTROLLER_init(&pid, kp, ki, kd, output_min, output_max, dt,
                              anti_windup_factor, derivative_filter_coefficient);
            printf("✅ Reset to default parameters\n");
            printf("Press Enter to continue...");
            fgets(input, sizeof(input), stdin);
        }
        else if (input[0] == 's' || input[0] == 'S') {
            // format: <initial_measurement>,<setpoint>
            printf("Enter initial measurement and setpoint (comma separated): ");
            if (fgets(input, sizeof(input), stdin)) {
                float new_initial, new_setpoint;
                if (sscanf(input, "%f,%f", &new_initial, &new_setpoint) == 2) {
                    initial_measurement = new_initial;
                    setpoint = new_setpoint;
                    printf("✅ Updated initial measurement to %.2f and setpoint to %.2f\n", initial_measurement, setpoint);
                } else {
                    printf("❌ Invalid format! Using previous values.\n");
                }
            }
            printf("Press Enter to continue...");
            fgets(input, sizeof(input), stdin);
        }
        else {
            // Assume it's a new setpoint
            float new_setpoint = atof(input);
            if (new_setpoint == 0.0f && input[0] != '0' && input[0] != '-' && input[0] != '+') {
                printf("❌ Invalid input! Using previous setpoint: %.2f\n", setpoint);
            } else {
                setpoint = new_setpoint;
            }
        }
    }

    // Cleanup
    free(measurement_history);
    free(output_history);
    
    printf("\n👋 Simulation ended. Thank you!\n");
    return 0;
}