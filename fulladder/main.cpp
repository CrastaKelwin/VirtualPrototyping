#include <systemc.h>

SC_MODULE(FullAdder) {
	sc_in<bool> a;
	sc_in<bool> b;
	sc_in<bool> cin;
	sc_out<bool> sum;
	sc_out<bool> carry;

	void add() {
    	sum.write((a.read() ^ b.read()) ^ cin.read());
    	carry.write((a.read() && b.read()) || (a.read() && cin.read()) || (b.read() && cin.read()));
	}

	SC_CTOR(FullAdder) {
    	SC_METHOD(add);
    	sensitive << a << b << cin;
	}
};

// Monitor
SC_MODULE(MonitorFullAdder) {
	sc_in<bool> sum, carry;

	void monitor() {
    	cout << "FullAdder Output: Sum = " << sum.read() << ", Carry = " << carry.read() << endl;
	}

	SC_CTOR(MonitorFullAdder) {
    	SC_METHOD(monitor);
    	sensitive << sum << carry;
	}
};

// Driver
SC_MODULE(DriverFullAdder) {
	sc_out<bool> a, b, cin;

	void drive() {
    	wait(5, SC_NS);
    	a.write(false);
    	b.write(false);
    	cin.write(false);
    	wait(5, SC_NS);
    	a.write(true);
    	wait(5, SC_NS);
    	b.write(true);
    	wait(5, SC_NS);
    	cin.write(true);
    	wait(5, SC_NS);
    	a.write(false);
    	wait(5, SC_NS);
    	sc_stop();
	}

	SC_CTOR(DriverFullAdder) {
    	SC_THREAD(drive);
	}
};

int sc_main(int argc, char* argv[]) {
	sc_signal<bool> a, b, cin, sum, carry;

    sc_trace_file *tf = sc_create_vcd_trace_file("trace");

    sc_trace(tf, a, "a");
    sc_trace(tf, b, "b");
    sc_trace(tf, cin, "cin");
    sc_trace(tf, sum, "sum");
    sc_trace(tf, carry, "carry");

	FullAdder fullAdder("fullAdder");
	fullAdder.a(a);
	fullAdder.b(b);
	fullAdder.cin(cin);
	fullAdder.sum(sum);
	fullAdder.carry(carry);

	MonitorFullAdder monitor("monitor");
	monitor.sum(sum);
	monitor.carry(carry);

	DriverFullAdder driver("driver");
	driver.a(a);
	driver.b(b);
	driver.cin(cin);

	sc_start();
    sc_close_vcd_trace_file(tf);
	return 0;
}


