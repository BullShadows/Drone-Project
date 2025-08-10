#include "command_parser.h"
#include "flight_control.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>              // for atoi()
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
#include "main.h"
#include "flight_globals.h"
#define RX_BUFFER_SIZE 32

char rx_buffer[RX_BUFFER_SIZE];
uint8_t rx_index = 0;
volatile uint8_t command_received = 0;
float desired_pitch = 0.0f;
float desired_roll = 0.0f;
int throttle = 1000;
int pitch = 0;
int roll = 0;
int yaw = 0;

void CommandParser_Init(void)
{
    rx_index = 0;
    memset(rx_buffer, 0, sizeof(rx_buffer));
    HAL_UART_Receive_IT(&huart1, (uint8_t *)&rx_buffer[rx_index], 1);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        if (rx_buffer[rx_index] == '\n')
        {
            rx_buffer[rx_index] = '\0';
            command_received = 1;
            rx_index = 0;
        }
        else
        {
            if (rx_index < RX_BUFFER_SIZE - 1)
            {
                rx_index++;
            }
            else
            {
                rx_index = 0;
                memset(rx_buffer, 0, sizeof(rx_buffer));
            }
        }

        HAL_UART_Receive_IT(&huart1, (uint8_t *)&rx_buffer[rx_index], 1);
    }
}

void CommandParser_Update(void)
{
    if (command_received)
    {
        command_received = 0;

        char command = rx_buffer[0];
        char num_str[8] = {0};
        strncpy(num_str, &rx_buffer[1], sizeof(num_str) - 1);
        int value = atoi(num_str);

        // Debug print
        char debug[64];
        snprintf(debug, sizeof(debug), "Parsed: %c = %d\r\n", command, value);
        HAL_UART_Transmit(&huart2, (uint8_t*)debug, strlen(debug), 100);

        if (command == 'T') {
            throttle = value;
        }
        else if (command == 'P') {
            pitch = value;
            desired_pitch = (float)value; // Can be negative

        }
        else if (command == 'R') {
            roll = value;
            desired_roll = (float)value;

        }
        else if (command == 'Y') {
            yaw = value;
        }

        memset(rx_buffer, 0, sizeof(rx_buffer));
    }
}
