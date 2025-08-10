#include "pid_controller.h"

void PID_Init(PID_Controller* pid, float kp, float ki, float kd) {
    pid->Kp = kp;
    pid->Ki = ki;
    pid->Kd = kd;
    pid->integral = 0;
    pid->last_error = 0;
}

float PID_Compute(PID_Controller* pid, float setpoint, float measurement) {
    float error = setpoint - measurement;
    pid->integral += error;
    float derivative = error - pid->last_error;
    pid->last_error = error;

    return pid->Kp * error + pid->Ki * pid->integral + pid->Kd * derivative;
}
