#include <systemc.h>

SC_MODULE(NandGate) {
	sc_in<bool> a;
	sc_in<bool> b;
	sc_out<bool> out;

	void nandOperation() {
    	out.write(!(a.read() && b.read()));
	}

	SC_CTOR(NandGate) {
    	SC_METHOD(nandOperation);
    	sensitive << a << b;
	}
};

// Monitor
SC_MODULE(MonitorNandGate) {
	sc_in<bool> out;

	void monitor() {
    	cout << "NandGate Output: " << out.read() << endl;
	}

	SC_CTOR(MonitorNandGate) {
    	SC_METHOD(monitor);
    	sensitive << out;
	}
};

// Driver
SC_MODULE(DriverNandGate) {
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

	SC_CTOR(DriverNandGate) {
    	SC_THREAD(drive);
	}
};

int sc_main(int argc, char* argv[]) {
	sc_signal<bool> a, b, out;

    sc_trace_file *tf = sc_create_vcd_trace_file("trace");

    sc_trace(tf, a, "a");
    sc_trace(tf, b, "b");
    sc_trace(tf, out, "out");

	NandGate nandGate("nandGate");
	nandGate.a(a);
	nandGate.b(b);
	nandGate.out(out);

	MonitorNandGate monitor("monitor");
	monitor.out(out);

	DriverNandGate driver("driver");
	driver.a(a);
	driver.b(b);

	sc_start();
    sc_close_vcd_trace_file(tf);
	return 0;
}
