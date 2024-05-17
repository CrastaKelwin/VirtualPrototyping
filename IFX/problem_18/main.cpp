#include <systemc.h>
#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>
#include "registers.h"

using namespace sc_core;
using namespace sc_dt;
using namespace std;
using namespace tlm;
using namespace tlm_utils;

// UART module representing a simple memory
SC_MODULE(UART)
{
    sc_in<bool> rx;
    sc_out<bool> tx;

    sc_signal<bool> clk; // internal generated signal for baudrate

    // Array to store register values
    sc_bv<32> uart_reg[UART_REG_COUNT];
    sc_time wait_time;
    bool is_frame_started = false;
    unsigned char bits_shifted = 0;

    unsigned int data_length = 8; // default 8

    void transmit(uint8_t data)
    {
        tx.write(0);
        wait(wait_time);
        cout << "transmitting " << (int)data << endl;
        for (size_t i = 0; i < data_length; i++)
        {
            tx.write((data >> i) & 0x01);
            cout << "shifting out " << ((data >> i) & 0x01) << endl;
            wait(wait_time);
        }
        cout << "shifting out stop bit" << endl;
        tx.write(1); //  stop bit
        wait(wait_time);
    }
    void sample()
    {
        if (clk.posedge())
        {
            if (is_frame_started)
            {
                uart_reg[UART_RX_REG].range(bits_shifted, bits_shifted) = rx.read();
                bits_shifted++;
                if (bits_shifted == data_length)
                {
                    is_frame_started = false;
                }
            }
            else
            {
                if (rx.read() == 0)
                {
                    uart_reg[UART_RX_REG]=0;
                    bits_shifted = 0;
                    is_frame_started = true;
                }
            }
        }
    }
    // TLM-2 target socket, defaults to 32-bits wide, base protocol
    tlm_utils::simple_target_socket<UART> uart_target_socket;
    // Manual clock generator method
    void generate_clock()
    {
        while (true)
        {
            if (uart_reg[UART_CONTROL_REG][UART_CONTROL_EN_BIT])
            {
                clk.write(0);        // Falling edge
                wait(wait_time / 2); // Half the clock period
                clk.write(1);        // Rising edge
                wait(wait_time / 2); // Half the clock period
            }
            else
            {
                wait(wait_time);
            }
        }
    }
    SC_CTOR(UART) : uart_target_socket("uart_target_socket"), wait_time(10, SC_NS), clk("clk")
    {
        uart_target_socket.register_b_transport(this, &UART::b_transport);
        SC_METHOD(sample);
        sensitive << clk;
        SC_THREAD(generate_clock); // Start the manual clock generator
    }

    // TLM-2 blocking transport method
    virtual void b_transport(tlm_generic_payload & trans, sc_time & delay)
    {
        tlm_command cmd = trans.get_command();
        sc_dt::uint64 adr = trans.get_address();
        unsigned int *ptr = (unsigned int *)trans.get_data_ptr();
        unsigned int len = trans.get_data_length();
        unsigned char *byt = trans.get_byte_enable_ptr();
        unsigned int wid = trans.get_streaming_width();

        // Check address range and unsupported features
        if (adr >= sc_dt::uint64(UART_REG_COUNT) || byt != 0 || len > 4 || wid < len)
            SC_REPORT_ERROR("TLM-2", "Target does not support given generic payload transaction");

        cout << adr << "\t" << len << "\t" << *ptr << endl;
        // Implement read and write commands
        // Implement read and write commands
        if (cmd == TLM_READ_COMMAND)
        {
            if (adr < UART_REG_COUNT)
            {
                *ptr = uart_reg[adr].to_int();
            }
            else
            {
                SC_REPORT_ERROR("TLM-2", "Invalid address for read operation");
            }
        }
        else if (cmd == TLM_WRITE_COMMAND)
        {
            if (adr < UART_REG_COUNT)
            {
                uart_reg[adr] = *ptr;
                switch (adr)
                {
                case UART_CONTROL_REG:
                {
                    int baud = uart_reg[UART_CONTROL_REG].range(31, 16).to_uint();
                    wait_time = sc_time(baud, SC_NS);
                    data_length = uart_reg[UART_CONTROL_REG].range(5, 1).to_uint();
                    cout << "baud" << baud << "\t" << "length = " << data_length << endl;
                    if (uart_reg[UART_CONTROL_REG].range(6, 6).to_uint())
                    {
                        if (uart_reg[UART_CONTROL_REG][UART_CONTROL_EN_BIT])
                        {
                            transmit(uart_reg[UART_TX_REG].range(data_length, 0).to_uint());
                            uart_reg[UART_CONTROL_REG].range(6, 6) = 0;
                        }else{
                            cout<<"UART Module disabled"<<endl;
                        }
                    }else{
                            cout<<"set Start bit CONTROL[6] to start transmit"<<endl;
                    }
                }
                break;
                case UART_TX_REG:
                    cout << "data \t" << uart_reg[UART_TX_REG].to_uint() << endl;
                    printf("data : 0x%X",uart_reg[UART_TX_REG].to_uint());
                    break;
                default:
                    break;
                }
            }
            else
            {
                SC_REPORT_ERROR("TLM-2", "Invalid address for write operation");
            }
        }
        // Set response status
        trans.set_response_status(TLM_OK_RESPONSE);
    }
};

// Testbench module
SC_MODULE(Testbench)
{

    sc_out<bool> rx;
    sc_in<bool> tx;

    void transmit(uint8_t data, sc_time delay, uint8_t data_length)
    {
        rx.write(0);
        wait(delay);
        for (size_t i = 0; i < data_length; i++)
        {
            rx.write((data >> i) & 0x01);
            wait(delay);
        }
        rx.write(1); //  stop bit
        wait(delay);
    }
    // TLM-2 initiator socket, defaults to 32-bits wide, base protocol
    tlm_utils::simple_initiator_socket<Testbench> tb_initiator_socket;

    SC_CTOR(Testbench)
        : tb_initiator_socket("tb_initiator_socket")
    {
        SC_THREAD(run);
        rx.initialize(1);
    }

    void run()
    {
        struct UARTData
        {
            uint32_t data;
            int baud_rate;
            int frame_width;
        };
        // Define UART data array
        UARTData data_array[] = {
            {0b10101010, 100, 8},
            {0b10110110, 50, 8},
            {0b11010, 200, 5} ,
            {0b1101101010, 50, 10},
            {0b0010011, 200, 7} 
        };

        // Transmit each data packet in the array
        for (size_t i = 0; i < sizeof(data_array) / sizeof(data_array[0]); ++i)
        {
            // Extract UART data from the array
            UARTData data = data_array[i];

            tlm_generic_payload trans;
            trans.set_command(TLM_WRITE_COMMAND);
            trans.set_address(UART_TX_REG);
            cout<<"TLM: Sending "<<(int)data.data<<endl;
            trans.set_data_ptr(reinterpret_cast<unsigned char *>(&data.data));
            trans.set_data_length(4);
            trans.set_streaming_width(4);
            trans.set_byte_enable_ptr(0);
            trans.set_dmi_allowed(false);
            trans.set_response_status(TLM_INCOMPLETE_RESPONSE);

            sc_time delay = sc_time(10, SC_NS);
            tb_initiator_socket->b_transport(trans, delay);

            if (trans.is_response_error())
                SC_REPORT_ERROR("TLM-2", "Response error from b_transport");
                
            trans.set_command(TLM_WRITE_COMMAND);
            trans.set_address(UART_CONTROL_REG);
            uint32_t ctrl= (data.frame_width)<<1|(data.baud_rate)<<16|(1 << 6)|(1);
            printf("ctrl:%X\n",ctrl);
            trans.set_data_ptr(reinterpret_cast<unsigned char *>(&ctrl));
            trans.set_data_length(4);
            trans.set_streaming_width(4);
            trans.set_byte_enable_ptr(0);
            trans.set_dmi_allowed(false);
            trans.set_response_status(TLM_INCOMPLETE_RESPONSE);

            tb_initiator_socket->b_transport(trans, delay);

            if (trans.is_response_error())
                SC_REPORT_ERROR("TLM-2", "Response error from b_transport");

            transmit(data.data,sc_time(data.baud_rate,SC_NS),data.frame_width);
        }
        
    }
};

int sc_main(int argc, char *argv[])
{
    sc_signal<bool> rx, tx;
    // Instantiate UART module and Testbench
    sc_trace_file *tf = sc_create_vcd_trace_file("traces");

    UART uart("uart");
    Testbench testbench("testbench");

    uart.rx(rx);
    uart.tx(tx);

    testbench.tx(tx);
    testbench.rx(rx);

    // Connect testbench to memory
    testbench.tb_initiator_socket.bind(uart.uart_target_socket);

    sc_trace(tf, rx, "rx");
    sc_trace(tf, tx, "tx");
    sc_trace(tf, uart.clk, "clk");
    sc_trace(tf, uart.uart_reg[UART_CONTROL_REG], "UART_CONTROL_REG");
    sc_trace(tf, uart.uart_reg[UART_RX_REG], "UART_RX_REG");
    sc_trace(tf, uart.uart_reg[UART_TX_REG], "UART_TX_REG");

    // Start simulation
    sc_start(1, SC_MS);

    return 0;
}
