#ifndef _TIMER_COUNTER_H_
#define _TIMER_COUNTER_H_

#include "systemc.h"
#include "registers.h"

#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>

using namespace sc_core;
using namespace sc_dt;
using namespace std;
using namespace tlm;
using namespace tlm_utils;

SC_MODULE(timer_counter)
{
    tlm_utils::simple_target_socket<timer_counter> timer_target_socket;
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
    SC_CTOR(timer_counter) : timer_target_socket("timer_target_socket")
    {
        timer_target_socket.register_b_transport(this, &timer_counter::b_transport);
        cout << "Executing new" << endl;
        SC_THREAD(update_timer);
    }

    virtual void b_transport(tlm_generic_payload & trans, sc_time & delay)
    {
        tlm_command cmd = trans.get_command();
        sc_dt::uint64 adr = trans.get_address();
        unsigned char *ptr = trans.get_data_ptr();
        unsigned int len = trans.get_data_length();
        unsigned char *byt = trans.get_byte_enable_ptr();
        unsigned int wid = trans.get_streaming_width();

        // Check address range and unsupported features
        if (adr >= sc_dt::uint64(TIMER_REG_COUNT) || byt != 0 || len > 4 || wid < len)
            SC_REPORT_ERROR("TLM-2", "Target does not support given generic payload transaction");

        // cout << "TLM" << "\t" << cmd << "\t" << adr << "\t" << (unsigned int)*ptr << endl;
        // Implement read and write commands
        if (cmd == TLM_READ_COMMAND)
        {
            *ptr = timer_reg[adr].to_uint();
        }
        else if (cmd == TLM_WRITE_COMMAND)
        {
            if (adr != TIMER_IRQ_STATUS_REG)
            {
                timer_reg[adr] = *ptr;
            }
            else
            {
                timer_reg[adr]&= ~(*ptr);
            }
        }

        // Set response status
        trans.set_response_status(TLM_OK_RESPONSE);
    }

}; // End of Module timer_counter

#endif // _TIMER_COUNTER_H_
