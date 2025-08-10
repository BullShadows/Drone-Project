#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

typedef struct {
    float Kp;
    float Ki;
    float Kd;
    float integral;
    float last_error;
} PID_Controller;

void PID_Init(PID_Controller* pid, float kp, float ki, float kd);
float PID_Compute(PID_Controller* pid, float setpoint, float measurement);

#endif
