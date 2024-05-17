#ifndef _TIMER_COUNTER_H_
#define _TIMER_COUNTER_H_

#include "systemc.h"
#include "registers.h"

SC_MODULE(timer_counter)
{
    // Input ports
    sc_in<sc_time> clock;      // Clock input of the design
    sc_in<bool> reset;         // Active high synchronous reset for all registers
    sc_in<sc_uint<8>> address; // Address for read/write operations
    sc_in<sc_uint<8>> data;    // Data to be written to the register
    sc_in<bool> write_enable;  // Indicates when data should be written to the specified address

    // Output ports
    sc_out<bool> irq0; // IRQ 0 output Compare pulse interrupt
    sc_out<bool> irq1; // IRQ 1 output Overflow pulse Interrupt

    sc_uint<8> count = 0; // Internal counter

    // Method to update the timer and associated registers
    void update_timer()
    {
        while (1)
        {
            irq0.write(0);
            irq1.write(0);

            // Resetting all registers when reset signal is asserted
            if (reset.read() == 0)
            {
                timer_reg[TIMER_CONTROL_REG] = 0;
                timer_reg[TIMER_VALUE_REG] = 0;
                timer_reg[TIMER_COMPARE_REG] = 0;
                timer_reg[TIMER_IRQ_STATUS_REG] = 0;
                count = 0;
            }
            else
            {
                // Handling write operations to registers
                if (write_enable.read() == 1)
                {
                    // Clearing specific bits in the IRQ status register
                    if (address.read() == TIMER_IRQ_STATUS_REG)
                    {
                        if (data.read() & (1 << TIMER_IRQ_STATUS_CMP_IRQ_BIT))
                        {
                            timer_reg[TIMER_IRQ_STATUS_REG][TIMER_IRQ_STATUS_CMP_IRQ_BIT] = 0;
                        }

                        if (data.read() & (1 << TIMER_IRQ_STATUS_OV_IRQ_BIT))
                        {
                            timer_reg[TIMER_IRQ_STATUS_REG][TIMER_IRQ_STATUS_OV_IRQ_BIT] = 0;
                        }
                    }
                    // Writing to other registers
                    else if (address.read() < TIMER_REG_COUNT && address.read() != TIMER_VALUE_REG)
                    {
                        timer_reg[address.read()] = data.read();
                    }
                }

                // Incrementing counter and updating value register
                if (timer_reg[TIMER_CONTROL_REG][TIMER_CONTROL_EN_BIT])
                {
                    if (count == 0xff && timer_reg[TIMER_CONTROL_REG][TIMER_CONTROL_OV_BIT])
                    {
                        // Overflow condition
                        timer_reg[TIMER_IRQ_STATUS_REG][TIMER_IRQ_STATUS_OV_IRQ_BIT] = 1;
                        irq1.write(1);
                    }
                    count = count + 1;
                    timer_reg[TIMER_VALUE_REG] = count;

                    // Checking for compare interrupt condition
                    if (timer_reg[TIMER_CONTROL_REG][TIMER_CONTROL_CMP_BIT] && count == timer_reg[TIMER_COMPARE_REG])
                    {
                        irq0.write(1);
                        timer_reg[TIMER_IRQ_STATUS_REG][TIMER_IRQ_STATUS_CMP_IRQ_BIT] = 1;
                    }
                    cout << "@" << sc_time_stamp() << " :: Incremented Counter "
                         << count << endl;
                }
            }
            wait(clock.read());
        }
    }

    // Constructor for the counter module
    SC_CTOR(timer_counter)
    {
        cout << "Executing new" << endl;
        SC_THREAD(update_timer);
    }

}; // End of Module timer_counter

#endif // _TIMER_COUNTER_H_
