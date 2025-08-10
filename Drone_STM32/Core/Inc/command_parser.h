#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include "main.h"

// Initializes UART reception and state
void CommandParser_Init(void);

// Call this in main loop to process received commands
void CommandParser_Update(void);

// Optional helpers
extern volatile uint8_t command_received;

#endif
