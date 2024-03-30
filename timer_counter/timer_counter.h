#ifndef _TIMER_COUNTER_H_
#define _TIMER_COUNTER_H_
#include "systemc.h"
#include "registers.h"

SC_MODULE(timer_counter)
{
    sc_in<bool> clock;         // Clock input of the design
    sc_in<bool> reset;         // active high, synchronous Reset all registers
    sc_in<sc_uint<8>> address; // read enable and write enable addresses
    sc_in<sc_uint<8>> data;    //  data to write to register
    // sc_in<bool> read_enable; // not used
    sc_in<bool> write_enable; // when high the data value will be written to the sfr address

    sc_out<bool> irq0;
    sc_out<bool> irq1;

    sc_uint<8> count = 0;

    void update_timer()
    {
        irq0.write(0);
        irq1.write(0);
        if (reset.read() == 0)
        {
            // resetting all registers
            timer_reg[TIMER_CONTROL_REG] = 0;
            timer_reg[TIMER_VALUE_REG] = 0;
            timer_reg[TIMER_COMPARE_REG] = 0;
            timer_reg[TIMER_IRQ_STATUS_REG] = 0;
            count = 0;
        }
        else
        {
            if (write_enable.read() == 1)
            {
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
                }else if (address.read() < TIMER_REG_COUNT && address.read() != TIMER_VALUE_REG)
                {
                    timer_reg[address.read()] = data.read();
                }
            }
            if (timer_reg[TIMER_CONTROL_REG][TIMER_CONTROL_EN_BIT])
            {
                if (count == 0xff && timer_reg[TIMER_CONTROL_REG][TIMER_CONTROL_OV_BIT])
                {
                    // count is going to overflow now
                    timer_reg[TIMER_IRQ_STATUS_REG][TIMER_IRQ_STATUS_OV_IRQ_BIT] = 1;
                    irq1.write(1);
                }
                count = count + 1;
                timer_reg[TIMER_VALUE_REG] = count;

                if (timer_reg[TIMER_CONTROL_REG][TIMER_CONTROL_CMP_BIT] && count == timer_reg[TIMER_COMPARE_REG])
                {
                    irq0.write(1);
                    timer_reg[TIMER_IRQ_STATUS_REG][TIMER_IRQ_STATUS_CMP_IRQ_BIT] = 1;
                }
                cout << "@" << sc_time_stamp() << " :: Incremented Counter "
                     << count << endl;
            }
        }
    }

    // Constructor for the counter
    // Since this counter is a positive edge trigged one,
    // We trigger the below block with respect to positive
    // edge of the clock and also when ever reset changes state
    SC_CTOR(timer_counter)
    {
        cout << "Executing new" << endl;
        SC_METHOD(update_timer);
        sensitive << reset;
        sensitive << clock.pos();
    } // End of Constructor

}; // End of Module counter

#endif // _TIMER_COUNTER_H_