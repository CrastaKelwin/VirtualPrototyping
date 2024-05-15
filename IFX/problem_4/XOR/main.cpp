#include <systemc.h>

SC_MODULE(XorGate) {
	sc_in<bool> a;
	sc_in<bool> b;
	sc_out<bool> out;

	void XorOperation() {
    	out.write((a.read() ^ b.read()));
	}

	SC_CTOR(XorGate) {
    	SC_METHOD(XorOperation);
    	sensitive << a << b;
	}
};

// Monitor
SC_MODULE(MonitorXorGate) {
	sc_in<bool> out;

	void monitor() {
    	cout << "XorGate Output: " << out.read() << endl;
	}

	SC_CTOR(MonitorXorGate) {
    	SC_METHOD(monitor);
    	sensitive << out;
	}
};
// Driver
SC_MODULE(DriverXorGate) {
	sc_out<bool> a, b;

	void drive() {
    	a.write(false);
    	b.write(false);
    	wait(2, SC_NS);
    	a.write(true);
    	wait(2, SC_NS);
    	a.write(false);
    	b.write(true);
    	wait(2, SC_NS);
    	a.write(true);
    	wait(2, SC_NS);
    	sc_stop();
	}

	SC_CTOR(DriverXorGate) {
    	SC_THREAD(drive);
	}
};

int sc_main(int argc, char* argv[]) {
	sc_signal<bool> a, b, out;

    sc_trace_file *tf = sc_create_vcd_trace_file("trace");

    sc_trace(tf, a, "a");
    sc_trace(tf, b, "b");
    sc_trace(tf, out, "out");

	XorGate andGate("XorGate");
	andGate.a(a);
	andGate.b(b);
	andGate.out(out);

	MonitorXorGate monitor("monitor");
	monitor.out(out);

	DriverXorGate driver("driver");
	driver.a(a);
	driver.b(b);

	sc_start();
    sc_close_vcd_trace_file(tf);
	return 0;
}
