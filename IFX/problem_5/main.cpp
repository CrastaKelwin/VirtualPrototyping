#include <systemc.h>

SC_MODULE(HalfAdder)
{
	sc_in<bool> a;
	sc_in<bool> b;
	sc_out<bool> sum;
	sc_out<bool> carry;

	void add()
	{
		sum.write(a.read() ^ b.read());
		carry.write(a.read() && b.read());
	}
	SC_CTOR(HalfAdder)
	{
		SC_METHOD(add);
		sensitive << a << b;
	}
};

SC_MODULE(TBAlfAdder)
{
	sc_out<bool> a, b;
	sc_in<bool> sum, carry;

	void test()
	{
		a.write(false);
		b.write(false);
		wait(2, SC_NS);
		assert(sum.read() == 0);
		assert(carry.read() == 0);
		a.write(true);
		b.write(false);
		wait(2, SC_NS);
		assert(sum.read() == 1);
		assert(carry.read() == 0);
		a.write(false);
		b.write(true);
		wait(2, SC_NS);
		assert(sum.read() == 1);
		assert(carry.read() == 0);
		a.write(true);
		b.write(true);
		wait(2, SC_NS);
		assert(sum.read() == 0);
		assert(carry.read() == 1);
		sc_stop();
	}

	SC_CTOR(TBAlfAdder)
	{
		SC_THREAD(test);
	}
};

int sc_main(int argc, char *argv[])
{
	sc_signal<bool> a, b, sum, carry;

	sc_trace_file *tf = sc_create_vcd_trace_file("trace");

	sc_trace(tf, a, "a");
	sc_trace(tf, b, "b");
	sc_trace(tf, sum, "sum");
	sc_trace(tf, carry, "carry");

	HalfAdder dut("HalfAdder");
	dut.a(a);
	dut.b(b);
	dut.sum(sum);
	dut.carry(carry);

	TBAlfAdder tb("TBAlfAdder");
	tb.a(a);
	tb.b(b);
	tb.sum(sum);
	tb.carry(carry);

	sc_start();
	sc_close_vcd_trace_file(tf);
	return 0;
}
