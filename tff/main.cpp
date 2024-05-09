#include <systemc.h>

SC_MODULE(TFlipFlop) {
	sc_in<bool> t;
	sc_in<bool> clock;
	sc_out<bool> q;
	sc_out<bool> qBar;

	bool currentState;

	void flipFlop() {
    	if (clock.posedge()) {
        	if (t.read() == 1) {
            	currentState = !currentState;
        	}
        	q.write(currentState);
        	qBar.write(!currentState);
    	}
	}

	SC_CTOR(TFlipFlop) {
    	SC_METHOD(flipFlop);
    	sensitive << clock.pos();
	}
};

// Monitor
SC_MODULE(MonitorTFlipFlop) {
	sc_in<bool> q, qBar;

	void monitor() {
    	cout << "T Flip Flop Output: Q = " << q.read() << ", QBar = " << qBar.read() << endl;
	}

	SC_CTOR(MonitorTFlipFlop) {
    	SC_METHOD(monitor);

    	sensitive << q << qBar;
	}
};

// Driver
SC_MODULE(DriverTFlipFlop) {
	sc_out<bool> t, clock;

	void drive() {
    	wait(5, SC_NS);
    	t.write(false);
    	clock.write(false);
    	wait(5, SC_NS);
    	t.write(true);
    	wait(5, SC_NS);
    	clock.write(true);
    	wait(5, SC_NS);
    	clock.write(false);
    	wait(5, SC_NS);
    	t.write(false);
    	wait(5, SC_NS);
    	clock.write(true);
    	wait(5, SC_NS);
    	clock.write(false);
    	wait(5, SC_NS);
    	t.write(true);
    	wait(5, SC_NS);
    	clock.write(true);
    	wait(5, SC_NS);
    	clock.write(false);
    	wait(5, SC_NS);
    	sc_stop();
	}

	SC_CTOR(DriverTFlipFlop) {
    	SC_THREAD(drive);
	}
};

int sc_main(int argc, char* argv[]) {
	sc_signal<bool> t, clock, q, qBar;

    sc_trace_file *tf = sc_create_vcd_trace_file("trace");

    sc_trace(tf, t, "t");
    sc_trace(tf, clock, "clock");
    sc_trace(tf, q, "q");
    sc_trace(tf, qBar, "qBar");

	TFlipFlop flipFlop("flipFlop");
	flipFlop.t(t);
	flipFlop.clock(clock);
	flipFlop.q(q);
	flipFlop.qBar(qBar);

	MonitorTFlipFlop monitor("monitor");
	monitor.q(q);
	monitor.qBar(qBar);

	DriverTFlipFlop driver("driver");
	driver.t(t);
	driver.clock(clock);

	sc_start();
    sc_close_vcd_trace_file(tf);
	return 0;
}
