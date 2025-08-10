#ifndef FLIGHT_CONTROL_H
#define FLIGHT_CONTROL_H

#include "main.h"

// Control input values
extern int throttle;
extern int pitch;
extern int roll;
extern int yaw;

// Setup function (optional)
void FlightControl_Init(void);

// Updates motor PWM signals based on control values
void FlightControl_UpdateMotors(void);

// Clamping macro
#define CLAMP(val, min, max) ((val) < (min) ? (min) : ((val) > (max) ? (max) : (val)))

#endif
