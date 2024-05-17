#ifndef _REGISTERS_H_
#define _REGISTERS_H_

#include <systemc.h>

// Define memory-mapped register addresses
#define UART_CONTROL_REG            (0) // Control register address
#define UART_RX_REG                 (1) // Value register address
#define UART_TX_REG                 (2) // Compare register address

// Define bit positions within control register
#define UART_CONTROL_EN_BIT                     (0) // Enable bit position in the control register
#define UART_CONTROL_DATA_FORMAT_BIT            (1) // 
#define UART_CONTROL_DATA_FORMAT_BIT_LENGTH     (4) // 
#define UART_CONTROL_START_TRANSFER_BIT         (6) 
#define UART_CONTROL_BAUDRATE_BIT_OFFSET        (16)
#define UART_CONTROL_BAUDRATE_BIT_LENGTH        (16)

// Define the total number of registers
#define UART_REG_COUNT         (4) // Total number of registers in the memory-mapped interface


#endif // _REGISTERS_H_
