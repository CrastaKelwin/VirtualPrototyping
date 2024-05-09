#include <systemc.h>

SC_MODULE(Decoder24) {
	sc_in<bool> enable;
	sc_in<sc_uint<2>> in;
	sc_out<sc_uint<4>> out;

	void decode() {
    	if (enable.read()) {
        	sc_uint<4> value = 0;
        	value[in.read()] = 1;
        	out.write(value);
    	} else {
        	out.write(0);
    	}
	}

	SC_CTOR(Decoder24) {
    	SC_METHOD(decode);
    	sensitive << in << enable;
	}
};

// Monitor
SC_MODULE(MonitorDecoder24) {
	sc_in<sc_uint<4>> out;

	void monitor() {
    	cout << "Decoder24 Output: " << out.read() << endl;
	}

	SC_CTOR(MonitorDecoder24) {
    	SC_METHOD(monitor);
    	sensitive << out;
	}
};

SC_MODULE(DriverDecoder24) {
	sc_out<sc_uint<2>> in;
	sc_out<bool> enable;

	void drive() {
    	wait(5, SC_NS);
    	in.write(0);
    	enable.write(true);
    	wait(5, SC_NS);
    	in.write(1);
    	wait(5, SC_NS);
    	in.write(2);
    	wait(5, SC_NS);
    	in.write(3);
    	wait(5, SC_NS);
    	enable.write(false);
    	wait(5, SC_NS);
    	in.write(2);
    	wait(5, SC_NS);
    	in.write(3);
    	wait(5, SC_NS);
    	enable.write(true);
    	wait(5, SC_NS);
    	in.write(1);
    	wait(5, SC_NS);
    	enable.write(false);
    	wait(5, SC_NS);
    	sc_stop();
	}

	SC_CTOR(DriverDecoder24) {
    	SC_THREAD(drive);
	}
};

int sc_main(int argc, char* argv[]) {
	sc_signal<sc_uint<2>> in;
	sc_signal<bool> enable;
	sc_signal<sc_uint<4>> out;

    sc_trace_file *tf = sc_create_vcd_trace_file("trace");

    sc_trace(tf, enable, "enable");
    sc_trace(tf, in, "in");
    sc_trace(tf, out, "out");

	Decoder24 decoder("decoder24");
	decoder.in(in);
	decoder.enable(enable);
	decoder.out(out);

	MonitorDecoder24 monitor("monitor24");
	monitor.out(out);

	DriverDecoder24 driver("driver24");
	driver.in(in);
	driver.enable(enable);

	sc_start();
    sc_close_vcd_trace_file(tf);
	return 0;
}
