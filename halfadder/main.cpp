#include <systemc.h>

SC_MODULE(HalfAdder) {
	sc_in<bool> a;
	sc_in<bool> b;
	sc_out<bool> sum;
	sc_out<bool> carry;

	void add() {
    	sum.write(a.read() ^ b.read());
    	carry.write(a.read() && b.read());
	}
	SC_CTOR(HalfAdder) {
    	SC_METHOD(add);
    	sensitive << a << b;
	}
};

// Monitor
SC_MODULE(MonitorHalfAdder) {
	sc_in<bool> sum, carry;

	void monitor() {
    	cout << "HalfAdder Output: Sum = " << sum.read() << ", Carry = " << carry.read() << endl;
	}

	SC_CTOR(MonitorHalfAdder) {
    	SC_METHOD(monitor);
    	sensitive << sum << carry;
	}
};

// Driver
SC_MODULE(DriverHalfAdder) {
	sc_out<bool> a, b;

	void drive() {
    	wait(5, SC_NS);
    	a.write(false);
    	b.write(false);
    	wait(5, SC_NS);
    	a.write(true);
    	wait(5, SC_NS);
    	b.write(true);
    	wait(5, SC_NS);
    	a.write(false);
    	wait(5, SC_NS);
    	sc_stop();
	}

	SC_CTOR(DriverHalfAdder) {
    	SC_THREAD(drive);
	}
};

int sc_main(int argc, char* argv[]) {
	sc_signal<bool> a, b, sum, carry;

    sc_trace_file *tf = sc_create_vcd_trace_file("trace");

    sc_trace(tf, a, "a");
    sc_trace(tf, b, "b");
    sc_trace(tf, sum, "sum");
    sc_trace(tf, carry, "carry");

	HalfAdder halfAdder("halfAdder");
	halfAdder.a(a);
	halfAdder.b(b);
	halfAdder.sum(sum);
	halfAdder.carry(carry);

	MonitorHalfAdder monitor("monitor");
	monitor.sum(sum);
	monitor.carry(carry);

	DriverHalfAdder driver("driver");
	driver.a(a);
	driver.b(b);

	sc_start();
    sc_close_vcd_trace_file(tf);
	return 0;
}