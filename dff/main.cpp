#include <systemc.h>

SC_MODULE(DFlipFlop) {
	sc_in<bool> d;
	sc_in<bool> clock;
	sc_out<bool> q;
	sc_out<bool> qBar;

	void flipFlop() {
    	if (clock.posedge()) {
        	q.write(d.read());
        	qBar.write(!d.read());
    	}
	}

	SC_CTOR(DFlipFlop) {
    	SC_METHOD(flipFlop);
    	sensitive << clock.pos();
	}
};

// Monitor
SC_MODULE(MonitorDFlipFlop) {
	sc_in<bool> q, qBar;

	void monitor() {
    	cout << "D Flip Flop Output: Q = " << q.read() << ", QBar = " << qBar.read() << endl;
	}

	SC_CTOR(MonitorDFlipFlop) {
    	SC_METHOD(monitor);
    	sensitive << q << qBar;
	}
};

// Driver
SC_MODULE(DriverDFlipFlop) {
	sc_out<bool> d, clock;

	void drive() {
    	wait(5, SC_NS);
    	d.write(false);
    	clock.write(false);
    	wait(5, SC_NS);
    	d.write(true);
    	wait(5, SC_NS);
    	clock.write(true);
    	wait(5, SC_NS);
    	clock.write(false);
    	wait(5, SC_NS);
    	d.write(false);
    	wait(5, SC_NS);
    	clock.write(true);
    	wait(5, SC_NS);
    	clock.write(false);
    	wait(5, SC_NS);
    	sc_stop();
	}

	SC_CTOR(DriverDFlipFlop) {
    	SC_THREAD(drive);
	}
};

int sc_main(int argc, char* argv[]) {
	sc_signal<bool> d, clock, q, qBar;

    sc_trace_file *tf = sc_create_vcd_trace_file("trace");

    sc_trace(tf, d, "d");
    sc_trace(tf, clock, "clock");
    sc_trace(tf, q, "q");
    sc_trace(tf, qBar, "qBar");

	DFlipFlop flipFlop("flipFlop");
	flipFlop.d(d);
	flipFlop.clock(clock);
	flipFlop.q(q);
	flipFlop.qBar(qBar);

	MonitorDFlipFlop monitor("monitor");
	monitor.q(q);
	monitor.qBar(qBar);

	DriverDFlipFlop driver("driver");
	driver.d(d);
	driver.clock(clock);

	sc_start();
    sc_close_vcd_trace_file(tf);
	return 0;
}
