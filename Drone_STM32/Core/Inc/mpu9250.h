#ifndef MPU9250_H
#define MPU9250_H

#include "stm32f4xx_hal.h"

#define MPU9250_I2C_ADDR         (0x68 << 1) // AD0 = GND
#define MPU9250_ACCEL_SCALE_2G   16384.0
#define MPU9250_WHO_AM_I_RESPONSE 0x71
#define MPU9250_GYRO_SCALE_250DPS 131.0

HAL_StatusTypeDef MPU9250_Init(I2C_HandleTypeDef *hi2c);


HAL_StatusTypeDef MPU9250_Init(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef MPU9250_ReadAccel(I2C_HandleTypeDef *hi2c, int16_t *ax, int16_t *ay, int16_t *az);
void MPU9250_PrintAccel(UART_HandleTypeDef *huart, int16_t ax, int16_t ay, int16_t az);
HAL_StatusTypeDef MPU9250_ReadGyro(I2C_HandleTypeDef *hi2c, int16_t *gx, int16_t *gy, int16_t *gz);
void MPU9250_PrintGyro(UART_HandleTypeDef *huart, int16_t gx, int16_t gy, int16_t gz);


#define AK8963_I2C_ADDR       0x0C << 1
#define AK8963_WHO_AM_I       0x00
#define AK8963_WHO_AM_I_VAL   0x48
#define AK8963_CNTL1          0x0A
#define AK8963_MODE_CONT_16BIT 0x16

HAL_StatusTypeDef MPU9250_InitMagnetometer(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef MPU9250_ReadMagnetometer(I2C_HandleTypeDef *hi2c, int16_t *mx, int16_t *my, int16_t *mz);
void MPU9250_PrintMagnetometer(UART_HandleTypeDef *huart, int16_t mx, int16_t my, int16_t mz);

void MPU9250_ComputeTilt(int16_t ax, int16_t ay, int16_t az, float* pitch, float* roll);


#endif
