#include <systemc.h>

SC_MODULE(CounterMod8) {
	sc_in<bool> clock;
	sc_out<sc_uint<3>> count;

	sc_uint<3> counter;

	void countMod8() {
    	if (clock.posedge()) {
        	counter = (counter + 1) % 8;
        	count.write(counter);
    	}
	}

	SC_CTOR(CounterMod8) {
    	SC_METHOD(countMod8);
    	sensitive << clock.pos();
	}
};

// Monitor
SC_MODULE(MonitorCounterMod8) {
	sc_in<sc_uint<3>> count;

	void monitor() {
    	cout << "Counter Mod 8 Output: " << count.read() << endl;
	}

	SC_CTOR(MonitorCounterMod8) {
    	SC_METHOD(monitor);
    	sensitive << count;
	}
};

// Driver
SC_MODULE(DriverCounterMod8) {
	sc_out<bool> clock;

	void drive() {
    	for (int i = 0; i < 10; ++i) {
        	wait(5, SC_NS);
        	clock.write(false);
        	wait(5, SC_NS);
        	clock.write(true);
    	}
    	sc_stop();
	}

	SC_CTOR(DriverCounterMod8) {
    	SC_THREAD(drive);
	}
};

int sc_main(int argc, char* argv[]) {
	sc_signal<bool> clock;
	sc_signal<sc_uint<3>> count;

    sc_trace_file *tf = sc_create_vcd_trace_file("trace");

    sc_trace(tf, count, "count");
    sc_trace(tf, clock, "clock");

	CounterMod8 counter("counter");
	counter.clock(clock);
	counter.count(count);

	MonitorCounterMod8 monitor("monitor");
	monitor.count(count);

	DriverCounterMod8 driver("driver");
	driver.clock(clock);

	sc_start();
    sc_close_vcd_trace_file(tf);
	return 0;
}
