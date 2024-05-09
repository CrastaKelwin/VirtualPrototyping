#include <systemc.h>

SC_MODULE(SRFlipFlop) {
	sc_in<bool> s;
	sc_in<bool> r;
	sc_in<bool> clock;
	sc_out<bool> q;
	sc_out<bool> qBar;

	bool currentState;

	void flipFlop() {
    	if (clock.posedge()) {
        	if (s.read() && !r.read()) {
            	currentState = true;
        	} else if (!s.read() && r.read()) {
            	currentState = false;
        	}
        	q.write(currentState);
        	qBar.write(!currentState);
    	}
	}

	SC_CTOR(SRFlipFlop) {
    	SC_METHOD(flipFlop);
    	sensitive << clock.pos();
	}
};

// Monitor
SC_MODULE(MonitorSRFlipFlop) {
	sc_in<bool> q, qBar;

	void monitor() {
    	cout << "SR Flip Flop Output: Q = " << q.read() << ", QBar = " << qBar.read() << endl;
	}

	SC_CTOR(MonitorSRFlipFlop) {
    	SC_METHOD(monitor);
    	sensitive << q << qBar;
	}
};

// Driver
SC_MODULE(DriverSRFlipFlop) {
	sc_out<bool> s, r, clock;

	void drive() {
    	wait(5, SC_NS);
    	s.write(false);
    	r.write(false);
    	clock.write(false);
    	wait(5, SC_NS);
    	s.write(true);
    	wait(5, SC_NS);
    	clock.write(true);
    	wait(5, SC_NS);
    	clock.write(false);
    	wait(5, SC_NS);
    	r.write(true);
    	wait(5, SC_NS);
    	s.write(false);
    	wait(5, SC_NS);
    	clock.write(true);
    	wait(5, SC_NS);
    	clock.write(false);
    	wait(5, SC_NS);
    	sc_stop();
	}

	SC_CTOR(DriverSRFlipFlop) {
    	SC_THREAD(drive);
	}
};

int sc_main(int argc, char* argv[]) {
	sc_signal<bool> s, r, clock, q, qBar;

    sc_trace_file *tf = sc_create_vcd_trace_file("trace");

    sc_trace(tf, s, "s");
    sc_trace(tf, r, "r");
    sc_trace(tf, clock, "clock");
    sc_trace(tf, q, "q");
    sc_trace(tf, qBar, "qBar");

	SRFlipFlop flipFlop("flipFlop");
	flipFlop.s(s);
	flipFlop.r(r);
	flipFlop.clock(clock);
	flipFlop.q(q);
	flipFlop.qBar(qBar);

	MonitorSRFlipFlop monitor("monitor");
	monitor.q(q);
	monitor.qBar(qBar);

	DriverSRFlipFlop driver("driver");
	driver.s(s);
	driver.r(r);
	driver.clock(clock);

	sc_start();
    sc_close_vcd_trace_file(tf);
	return 0;
}
