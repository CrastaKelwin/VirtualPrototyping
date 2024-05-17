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

SC_MODULE(TBAndGate) {
	sc_out<bool> a, b;
	sc_in<bool> out;

	void test() {
    	a.write(false);
    	b.write(false);
    	wait(2, SC_NS);
        assert(out.read()==0);
    	a.write(true);
    	wait(2, SC_NS);
        assert(out.read()==0);
    	a.write(false);
    	b.write(true);
    	wait(2, SC_NS);
        assert(out.read()==0);
    	a.write(true);
    	wait(2, SC_NS);
        assert(out.read()==1);
    	sc_stop();
	}

	SC_CTOR(TBAndGate) {
    	SC_THREAD(test);
	}
};

int sc_main(int argc, char* argv[]) {
	sc_signal<bool> a, b, out;

    sc_trace_file *tf = sc_create_vcd_trace_file("trace");

    sc_trace(tf, a, "a");
    sc_trace(tf, b, "b");
    sc_trace(tf, out, "out");

	AndGate dut("andGate");
	dut.a(a);
	dut.b(b);
	dut.out(out);

	TBAndGate tb("TBAndGate");
	tb.a(a);
	tb.b(b);
    tb.out(out);

	sc_start();
    sc_close_vcd_trace_file(tf);
	return 0;
}
