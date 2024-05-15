#include <systemc.h>

SC_MODULE(XnorGate) {
	sc_in<bool> a;
	sc_in<bool> b;
	sc_out<bool> out;

	void XnorOperation() {
    	out.write(!(a.read() ^ b.read()));
	}

	SC_CTOR(XnorGate) {
    	SC_METHOD(XnorOperation);
    	sensitive << a << b;
	}
};

// Monitor
SC_MODULE(MonitorXnorGate) {
	sc_in<bool> out;

	void monitor() {
    	cout << "XnorGate Output: " << out.read() << endl;
	}

	SC_CTOR(MonitorXnorGate) {
    	SC_METHOD(monitor);
    	sensitive << out;
	}
};
// Driver
SC_MODULE(DriverXnorGate) {
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

	SC_CTOR(DriverXnorGate) {
    	SC_THREAD(drive);
	}
};

int sc_main(int argc, char* argv[]) {
	sc_signal<bool> a, b, out;

    sc_trace_file *tf = sc_create_vcd_trace_file("trace");

    sc_trace(tf, a, "a");
    sc_trace(tf, b, "b");
    sc_trace(tf, out, "out");

	XnorGate andGate("XnorGate");
	andGate.a(a);
	andGate.b(b);
	andGate.out(out);

	MonitorXnorGate monitor("monitor");
	monitor.out(out);

	DriverXnorGate driver("driver");
	driver.a(a);
	driver.b(b);

	sc_start();
    sc_close_vcd_trace_file(tf);
	return 0;
}
