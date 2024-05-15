#include <systemc.h>

SC_MODULE(OrGate) {
	sc_in<bool> a;
	sc_in<bool> b;
	sc_out<bool> out;

	void OrOperation() {
    	out.write((a.read() || b.read()));
	}

	SC_CTOR(OrGate) {
    	SC_METHOD(OrOperation);
    	sensitive << a << b;
	}
};
SC_MODULE(TBOrGate) {
	sc_out<bool> a, b;
	sc_in<bool> out;

	void test() {
    	a.write(false);
    	b.write(false);
    	wait(2, SC_NS);
        assert(out.read()==0);
    	a.write(true);
    	wait(2, SC_NS);
        assert(out.read()==1);
    	a.write(false);
    	b.write(true);
    	wait(2, SC_NS);
        assert(out.read()==1);
    	a.write(true);
    	wait(2, SC_NS);
        assert(out.read()==1);
    	sc_stop();
	}

	SC_CTOR(TBOrGate) {
    	SC_THREAD(test);
	}
};

int sc_main(int argc, char* argv[]) {
	sc_signal<bool> a, b, out;

    sc_trace_file *tf = sc_create_vcd_trace_file("trace");

    sc_trace(tf, a, "a");
    sc_trace(tf, b, "b");
    sc_trace(tf, out, "out");

	OrGate andGate("OrGate");
	andGate.a(a);
	andGate.b(b);
	andGate.out(out);

	TBOrGate tb("TBOrGate");
	tb.a(a);
	tb.b(b);
    tb.out(out);

	sc_start();
    sc_close_vcd_trace_file(tf);
	return 0;
}
