#ifndef _REGISTERS_H_
#define _REGISTERS_H_

#include <systemc.h>

#define TIMER_CONTROL_REG       (0)

#define TIMER_CONTROL_EN_BIT    (0)
#define TIMER_CONTROL_CMP_BIT   (1)
#define TIMER_CONTROL_OV_BIT    (2)

#define TIMER_VALUE_REG         (1)

#define TIMER_COMPARE_REG       (2)

#define TIMER_IRQ_STATUS_REG    (3)

#define TIMER_CONTROL_CMP_IRQ_BIT   (0)
#define TIMER_CONTROL_OV_IRQ_BIT   (1)

#define TIMER_REG_COUNT         (4)

sc_bv<32> timer_reg[TIMER_REG_COUNT];

#endif // _REGISTERS_H_