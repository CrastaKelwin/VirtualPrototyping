#include <systemc.h>

SC_MODULE(ShiftRegister) {
	sc_in<bool> clock;
	sc_in<bool> reset;
	sc_in<bool> input;
	sc_out<sc_uint<4>> output;

	sc_uint<4> registerValue;

	void shift() {
    	if (reset.read()) {
        	registerValue = 0;
    	} else {
        	registerValue = (registerValue << 1) | input.read();
    	}
    	output.write(registerValue);
	}

	SC_CTOR(ShiftRegister) {
    	SC_METHOD(shift);
    	sensitive << clock.pos() << reset;
	}
};

// Monitor
SC_MODULE(MonitorShiftRegister) {
	sc_in<sc_uint<4>> output;

	void monitor() {
    	cout << "Shift Register Output: " << output.read() << endl;
	}

	SC_CTOR(MonitorShiftRegister) {
    	SC_METHOD(monitor);
    	sensitive << output;
	}
};

// Driver
SC_MODULE(DriverShiftRegister) {
	sc_out<bool> clock, reset;
	sc_out<bool> input;

	void drive() {
    	wait(5, SC_NS);
    	reset.write(true);
    	wait(5, SC_NS);
    	reset.write(false);
    	wait(5, SC_NS);
    	input.write(true);
    	wait(5, SC_NS);
    	clock.write(true);
    	wait(5, SC_NS);
    	input.write(false);
    	wait(5, SC_NS);
    	clock.write(false);
    	wait(5, SC_NS);
    	clock.write(true);
    	wait(5, SC_NS);
    	input.write(true);
    	wait(5, SC_NS);
    	input.write(false);
    	wait(5, SC_NS);
    	clock.write(false);
    	wait(5, SC_NS);
    	clock.write(true);
    	wait(5, SC_NS);
    	input.write(true);
    	wait(5, SC_NS);
    	clock.write(false);
    	wait(5, SC_NS);
    	clock.write(true);
    	wait(5, SC_NS);
    	input.write(true);
    	wait(5, SC_NS);
    	clock.write(false);
    	wait(5, SC_NS);
    	clock.write(true);
    	wait(5, SC_NS);
    	input.write(true);
    	wait(5, SC_NS);
    	clock.write(false);
    	wait(5, SC_NS);
    	sc_stop();
	}

	SC_CTOR(DriverShiftRegister) {
    	SC_THREAD(drive);
	}
};

int sc_main(int argc, char* argv[]) {
	sc_signal<bool> clock, reset, input;
	sc_signal<sc_uint<4>> output;

    sc_trace_file *tf = sc_create_vcd_trace_file("trace");

    sc_trace(tf, clock, "clock");
    sc_trace(tf, reset, "reset");
    sc_trace(tf, input, "input");
    sc_trace(tf, output, "output");
    
	ShiftRegister shiftRegister("shiftRegister");
	shiftRegister.clock(clock);
	shiftRegister.reset(reset);
	shiftRegister.input(input);
	shiftRegister.output(output);

	MonitorShiftRegister monitor("monitor");
	monitor.output(output);

	DriverShiftRegister driver("driver");
	driver.clock(clock);
	driver.reset(reset);
	driver.input(input);

	sc_start();
    sc_close_vcd_trace_file(tf);
	return 0;
}
