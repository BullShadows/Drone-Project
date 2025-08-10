#include "flight_control.h"
#include "main.h"
#include "mpu9250.h"
#include "pid_controller.h"
#include <math.h>
#include <stdlib.h> // for abs()
#include "command_parser.h"
#include "flight_globals.h"

// Inputs from PC
extern int throttle;
extern int pitch;
extern int roll;
extern int yaw;
//other
extern int throttle;       // set from command parser
extern float desired_pitch;
extern float desired_roll;

extern I2C_HandleTypeDef hi2c1;  // your I2C for MPU
extern TIM_HandleTypeDef htim2;  // your PWM timer

PID_Controller pid_pitch;
PID_Controller pid_roll;


void FlightControl_Init(void)
{
    // You could set defaults here if needed
    throttle = 1000;
    pitch = 0;
    roll = 0;
    yaw = 0;
    PID_Init(&pid_pitch, 1.5f, 0.0f, 0.3f);
    PID_Init(&pid_roll,  1.5f, 0.0f, 0.3f);
}

void FlightControl_UpdateMotors(void)
{
    int16_t ax, ay, az;

    // Read acceleration values from IMU
    if (MPU9250_ReadAccel(&hi2c1, &ax, &ay, &az) != HAL_OK) {
    	ax = 0;
    	    ay = 0;
    	    az = 16384; // ~1g if you're using 2g range
    }

    // Convert to tilt angles (degrees)
    float pitch = atan2f(ax, sqrtf(ay * ay + az * az)) * 180.0f / M_PI;
    float roll  = atan2f(ay, sqrtf(ax * ax + az * az)) * 180.0f / M_PI;

    // Compute PID corrections
    float pitch_out = PID_Compute(&pid_pitch, desired_pitch, pitch);
    float roll_out  = PID_Compute(&pid_roll, desired_roll, roll);

    // Calculate motor values using mixing
    int m1 = throttle - pitch_out - roll_out; // Front Left
    int m2 = throttle - pitch_out + roll_out; // Front Right
    int m3 = throttle + pitch_out + roll_out; // Rear Right
    int m4 = throttle + pitch_out - roll_out; // Rear Left

    // Clamp motor values to valid PWM range
    m1 = CLAMP(m1, 1000, 2000);
    m2 = CLAMP(m2, 1000, 2000);
    m3 = CLAMP(m3, 1000, 2000);
    m4 = CLAMP(m4, 1000, 2000);

    // Send PWM to motors
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, m1); // M1
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, m2); // M2
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, m3); // M3
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, m4); // M4
}
