#include "systemc.h"
#include "timer_counter.h"

SC_MODULE(testbench) {
    tlm_utils::simple_initiator_socket<testbench> tb_initiator_socket;
    sc_signal<sc_time> clk;        // Reset signal
    sc_signal<bool> rst;        // Reset signal
    sc_signal<sc_uint<8>> addr; // Address signal
    sc_signal<sc_uint<8>> d; // Data signal
    sc_signal<bool> wen;        // Write enable signal
    sc_signal<bool> irq0;       // IRQ 0 signal
    sc_signal<bool> irq1;       // IRQ 1 signal

    timer_counter timer_counter_inst; // Timer counter instance
    void reg_access(int address,tlm_command type, unsigned int *data)
    {
        tlm_generic_payload trans;
        trans.set_command(type);
        trans.set_address(address);
        trans.set_data_ptr(reinterpret_cast<unsigned char *>(data));
        trans.set_data_length(4);
        trans.set_streaming_width(4);
        trans.set_byte_enable_ptr(0);
        trans.set_dmi_allowed(false);
        trans.set_response_status(TLM_INCOMPLETE_RESPONSE);

        sc_time delay = sc_time(1, SC_NS);
        tb_initiator_socket->b_transport(trans, delay);

        if (trans.is_response_error())
            SC_REPORT_ERROR("TLM-2", "Response error from b_transport");
    }
    // Test function to verify the functionality of the timer counter
    void test() {

unsigned int data;
        // Reset
        rst = false;
        wait(1, SC_NS);
        rst = true;

        // Test writing to registers
        data = 0b000001; // Enable timer
        reg_access(TIMER_CONTROL_REG,TLM_WRITE_COMMAND,&data);
        wait(1, SC_NS); // count =1
        cout<<"REG "<<timer_reg[TIMER_VALUE_REG].to_uint()<<endl;


        data = 10; // Set compare value to 10
        reg_access(TIMER_COMPARE_REG,TLM_WRITE_COMMAND,&data);
        wait(1, SC_NS); // count =2

        data = 0; // Clear IRQ status
        reg_access(TIMER_IRQ_STATUS_REG,TLM_WRITE_COMMAND,&data);
        wait(1, SC_NS); // count =3

        // Start timer
        data = 0b000000111; // Enable timer and compare interrupt
        reg_access(TIMER_CONTROL_REG,TLM_WRITE_COMMAND,&data);
        wait(1, SC_NS); // count =4
        cout<<"REG "<<timer_reg[TIMER_VALUE_REG].to_uint()<<endl;

        wait(6, SC_NS); // count =10
        // Wait for interrupt

        wait(SC_ZERO_TIME);
        // Check IRQ status
        reg_access(TIMER_IRQ_STATUS_REG,TLM_READ_COMMAND,&data);
        cout << "Checking IRQ0 status " << irq0.read() << endl;
        cout << "Checking IRQ1 status " << irq1.read() << endl;
        cout << "Checking TimerControl Value " << timer_reg[TIMER_CONTROL_REG] << endl;
        cout << "Checking Timer Value " << timer_reg[TIMER_VALUE_REG].to_int() << endl;
        assert(irq0.read() == true); // Timer compare interrupt asserted
        wait(1, SC_NS);              // count =11

        // Overflow interrupt test
        data = 0b000000101; // Enable timer and overflow interrupt
        reg_access(TIMER_CONTROL_REG,TLM_WRITE_COMMAND,&data);
        wait(1, SC_NS); // count =12

        // Wait for overflow
        wait(244, SC_NS); // count =256

        // Check IRQ status
        reg_access(TIMER_IRQ_STATUS_REG,TLM_READ_COMMAND,&data);
        wait(SC_ZERO_TIME);       
        cout << "Checking IRQ0 status after overflow " << irq0.read() << endl;
        cout << "Checking IRQ1 status after overflow " << irq1.read() << endl;
        assert(irq1.read() == true); // Timer overflow interrupt asserted
        wait(1, SC_NS);              // count =1

        // Stop timer
        data = 0b0000000000; // Disable timer
        reg_access(TIMER_CONTROL_REG,TLM_WRITE_COMMAND,&data);
        wait(1, SC_NS); // count =1

        // Wait for a few clock cycles
        wait(10, SC_NS); // count =1

        cout << "Checking Timer Value " << timer_reg[TIMER_VALUE_REG].to_int() << endl;

        assert(timer_reg[TIMER_VALUE_REG] == 1);

        // Check if no interrupts are asserted
        reg_access(TIMER_IRQ_STATUS_REG,TLM_READ_COMMAND,&data);
        wait(1, SC_NS);
        assert(irq0.read() == false);
        assert(irq1.read() == false);

        cout << "Checking Timer status " << timer_reg[TIMER_IRQ_STATUS_REG] << endl;

        assert(timer_reg[TIMER_IRQ_STATUS_REG] == 0b11);

        data = 0b0000000001; // clear TIMER_IRQ_STATUS_CMP_IRQ_BIT
        reg_access(TIMER_IRQ_STATUS_REG,TLM_WRITE_COMMAND,&data);
        wait(1, SC_NS);
        wait(SC_ZERO_TIME);
        cout << "Checking Timer status " << timer_reg[TIMER_IRQ_STATUS_REG] << endl;

        reg_access(TIMER_IRQ_STATUS_REG,TLM_READ_COMMAND,&data);
        assert(timer_reg[TIMER_IRQ_STATUS_REG] == 0b10);
        assert(data == 0b10);
        data = 0b00000000010; // clear TIMER_IRQ_STATUS_OV_IRQ_BIT
        reg_access(TIMER_IRQ_STATUS_REG,TLM_WRITE_COMMAND,&data);
        wait(1, SC_NS);
        cout << "Checking Timer status " << timer_reg[TIMER_IRQ_STATUS_REG] << endl;

        reg_access(TIMER_IRQ_STATUS_REG,TLM_READ_COMMAND,&data);
        wait(1, SC_NS);
        assert(timer_reg[TIMER_IRQ_STATUS_REG] == 0b00);
        assert(data == 0b00);

        // Stop simulation
        sc_stop();
    }

    // Constructor for the testbench
    SC_CTOR(testbench) : timer_counter_inst("timer_counter_inst") ,tb_initiator_socket("tb_initiator_socket"){
        // Connect signals to timer counter instance
        clk.write(sc_time(1,SC_NS));
        timer_counter_inst.clock(clk);
        timer_counter_inst.reset(rst);
        timer_counter_inst.address(addr);
        timer_counter_inst.data(d);
        timer_counter_inst.write_enable(wen);
        timer_counter_inst.irq0(irq0);
        timer_counter_inst.irq1(irq1);

        // Open VCD trace file
        sc_trace_file *tf = sc_create_vcd_trace_file("trace");
        // Trace signals
        sc_trace(tf, rst, "rst");
        sc_trace(tf, addr, "addr");
        sc_trace(tf, d, "data");
        sc_trace(tf, wen, "wen");
        sc_trace(tf, irq0, "irq0");
        sc_trace(tf, irq1, "irq1");
        sc_trace(tf, timer_reg[TIMER_CONTROL_REG], "TIMER_CONTROL_REG");
        sc_trace(tf, timer_reg[TIMER_VALUE_REG], "TIMER_VALUE_REG");
        sc_trace(tf, timer_reg[TIMER_COMPARE_REG], "TIMER_COMPARE_REG");
        sc_trace(tf, timer_reg[TIMER_IRQ_STATUS_REG], "TIMER_IRQ_STATUS_REG");

        // Start test function as a thread
        SC_THREAD(test);
    }

};

// Main function
int sc_main(int argc, char* argv[]) {
    testbench tb("tb"); // Instantiate the testbench
    tb.tb_initiator_socket.bind(tb.timer_counter_inst.timer_target_socket);
    sc_start(1,SC_MS); // Start simulation
    return 0;
}
