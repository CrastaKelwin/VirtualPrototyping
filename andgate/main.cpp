#include <systemc.h>

SC_MODULE(AndGate) {
	sc_in<bool> a;
	sc_in<bool> b;
	sc_out<bool> out;

	void andOperation() {
    	out.write(a.read() && b.read());
	}

	SC_CTOR(AndGate) {
    	SC_METHOD(andOperation);
    	sensitive << a << b;
	}
};

// Monitor
SC_MODULE(MonitorAndGate) {
	sc_in<bool> out;

	void monitor() {
    	cout << "AndGate Output: " << out.read() << endl;
	}

	SC_CTOR(MonitorAndGate) {
    	SC_METHOD(monitor);
    	sensitive << out;
	}
};
// Driver
SC_MODULE(DriverAndGate) {
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

	SC_CTOR(DriverAndGate) {
    	SC_THREAD(drive);
	}
};

int sc_main(int argc, char* argv[]) {
	sc_signal<bool> a, b, out;

    sc_trace_file *tf = sc_create_vcd_trace_file("trace");

    sc_trace(tf, a, "a");
    sc_trace(tf, b, "b");
    sc_trace(tf, out, "out");

	AndGate andGate("andGate");
	andGate.a(a);
	andGate.b(b);
	andGate.out(out);

	MonitorAndGate monitor("monitor");
	monitor.out(out);

	DriverAndGate driver("driver");
	driver.a(a);
	driver.b(b);

	sc_start();
    sc_close_vcd_trace_file(tf);
	return 0;
}
