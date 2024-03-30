#ifndef _REGISTERS_H_
#define _REGISTERS_H_

#include <systemc.h>

// Define memory-mapped register addresses
#define TIMER_CONTROL_REG           (0) // Control register address
#define TIMER_VALUE_REG             (1) // Value register address
#define TIMER_COMPARE_REG           (2) // Compare register address
#define TIMER_IRQ_STATUS_REG        (3) // IRQ status register address

// Define bit positions within control register
#define TIMER_CONTROL_EN_BIT        (0) // Enable bit position in the control register
#define TIMER_CONTROL_CMP_BIT       (1) // Compare bit position in the control register
#define TIMER_CONTROL_OV_BIT        (2) // Overflow bit position in the control register

// Define bit positions within status register
#define TIMER_IRQ_STATUS_CMP_IRQ_BIT   (0) // Compare interrupt bit position in the IRQ status register
#define TIMER_IRQ_STATUS_OV_IRQ_BIT    (1) // Overflow interrupt bit position in the IRQ status register

// Define the total number of registers
#define TIMER_REG_COUNT         (4) // Total number of registers in the memory-mapped interface

// Array to store register values
sc_bv<32> timer_reg[TIMER_REG_COUNT];

#endif // _REGISTERS_H_
