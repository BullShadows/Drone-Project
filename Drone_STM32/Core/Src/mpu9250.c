#include "mpu9250.h"
#include "stdio.h"
#include "stdlib.h"
#include <String.h>
#include <math.h> // for atan2f()

HAL_StatusTypeDef MPU9250_Init(I2C_HandleTypeDef *hi2c)
{
    uint8_t data = 0;
    return HAL_I2C_Mem_Write(hi2c, MPU9250_I2C_ADDR, 0x6B, 1, &data, 1, 1000);
}


HAL_StatusTypeDef MPU9250_InitMagnetometer(I2C_HandleTypeDef *hi2c)
{
    uint8_t data;

    // Enable I2C bypass mode on MPU9250
    data = 0x02; // BYPASS_EN = 1
    if (HAL_I2C_Mem_Write(hi2c, MPU9250_I2C_ADDR, 0x37, 1, &data, 1, 1000) != HAL_OK)
        return HAL_ERROR;

    // Check if AK8963 responds
    uint8_t whoami = 0;
    if (HAL_I2C_Mem_Read(hi2c, AK8963_I2C_ADDR, AK8963_WHO_AM_I, 1, &whoami, 1, 1000) != HAL_OK || whoami != AK8963_WHO_AM_I_VAL)
        return HAL_ERROR;

    // Power down first
    data = 0x00;
    HAL_I2C_Mem_Write(hi2c, AK8963_I2C_ADDR, AK8963_CNTL1, 1, &data, 1, 1000);
    HAL_Delay(10);

    // Set to continuous measurement mode 2 (16-bit output)
    data = AK8963_MODE_CONT_16BIT;
    return HAL_I2C_Mem_Write(hi2c, AK8963_I2C_ADDR, AK8963_CNTL1, 1, &data, 1, 1000);
}


HAL_StatusTypeDef MPU9250_ReadAccel(I2C_HandleTypeDef *hi2c, int16_t *ax, int16_t *ay, int16_t *az)
{
    uint8_t data[6];

    if (HAL_I2C_Mem_Read(hi2c, MPU9250_I2C_ADDR, 0x3B, 1, data, 6, 1000) != HAL_OK)
        return HAL_ERROR;

    *ax = (int16_t)(data[0] << 8 | data[1]);
    *ay = (int16_t)(data[2] << 8 | data[3]);
    *az = (int16_t)(data[4] << 8 | data[5]);

    return HAL_OK;
}

void MPU9250_PrintAccel(UART_HandleTypeDef *huart, int16_t ax, int16_t ay, int16_t az)
{
    float ax_g = ax / MPU9250_ACCEL_SCALE_2G;
    float ay_g = ay / MPU9250_ACCEL_SCALE_2G;
    float az_g = az / MPU9250_ACCEL_SCALE_2G;

    int ax_int = (int)(ax_g * 100);
    int ay_int = (int)(ay_g * 100);
    int az_int = (int)(az_g * 100);

    char msg[100];
    sprintf(msg,
        "Accel X: %d.%02d | Y: %d.%02d | Z: %d.%02d\r\n",
        ax_int / 100, abs(ax_int % 100),
        ay_int / 100, abs(ay_int % 100),
        az_int / 100, abs(az_int % 100));

    HAL_UART_Transmit(huart, (uint8_t*)msg, strlen(msg), 1000);
}
HAL_StatusTypeDef MPU9250_ReadGyro(I2C_HandleTypeDef *hi2c, int16_t *gx, int16_t *gy, int16_t *gz)
{
    uint8_t data[6];

    if (HAL_I2C_Mem_Read(hi2c, MPU9250_I2C_ADDR, 0x43, 1, data, 6, 1000) != HAL_OK)
        return HAL_ERROR;

    *gx = (int16_t)(data[0] << 8 | data[1]);
    *gy = (int16_t)(data[2] << 8 | data[3]);
    *gz = (int16_t)(data[4] << 8 | data[5]);

    return HAL_OK;
}
void MPU9250_PrintGyro(UART_HandleTypeDef *huart, int16_t gx, int16_t gy, int16_t gz)
{
    float gx_dps = gx / MPU9250_GYRO_SCALE_250DPS;
    float gy_dps = gy / MPU9250_GYRO_SCALE_250DPS;
    float gz_dps = gz / MPU9250_GYRO_SCALE_250DPS;

    int gx_int = (int)(gx_dps * 100);
    int gy_int = (int)(gy_dps * 100);
    int gz_int = (int)(gz_dps * 100);

    char msg[100];
    sprintf(msg,
        "Gyro X: %d.%02d | Y: %d.%02d | Z: %d.%02d\r\n",
        gx_int / 100, abs(gx_int % 100),
        gy_int / 100, abs(gy_int % 100),
        gz_int / 100, abs(gz_int % 100));

    HAL_UART_Transmit(huart, (uint8_t*)msg, strlen(msg), 1000);
}
HAL_StatusTypeDef MPU9250_ReadMagnetometer(I2C_HandleTypeDef *hi2c, int16_t *mx, int16_t *my, int16_t *mz)
{
    uint8_t rawData[6];

    if (HAL_I2C_Mem_Read(hi2c, AK8963_I2C_ADDR, 0x03, 1, rawData, 6, 1000) != HAL_OK)
        return HAL_ERROR;

    *mx = (int16_t)(rawData[1] << 8 | rawData[0]);
    *my = (int16_t)(rawData[3] << 8 | rawData[2]);
    *mz = (int16_t)(rawData[5] << 8 | rawData[4]);

    return HAL_OK;
}
void MPU9250_PrintMagnetometer(UART_HandleTypeDef *huart, int16_t mx, int16_t my, int16_t mz)
{
    char msg[100];
    sprintf(msg, "Mag X: %d | Y: %d | Z: %d\r\n", mx, my, mz);
    HAL_UART_Transmit(huart, (uint8_t*)msg, strlen(msg), 1000);
}

void MPU9250_ComputeTilt(int16_t ax, int16_t ay, int16_t az, float* pitch, float* roll)
{
    *pitch = atan2f((float)ax, sqrtf(ay*ay + az*az)) * 180.0f / M_PI;
    *roll  = atan2f((float)ay, sqrtf(ax*ax + az*az)) * 180.0f / M_PI;
}
