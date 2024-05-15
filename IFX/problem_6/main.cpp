#include <systemc.h>

SC_MODULE(FullAdder)
{
	sc_in<bool> a;
	sc_in<bool> b;
	sc_in<bool> cin;
	sc_out<bool> sum;
	sc_out<bool> carry;

	void add()
	{
		sum.write(a.read() ^ b.read() ^ cin.read());
		carry.write((a.read() && b.read()) || (a.read() && cin.read())||(cin.read() && b.read()));
	}
	SC_CTOR(FullAdder)
	{
		SC_METHOD(add);
		sensitive << a << b <<cin;
	}
};

SC_MODULE(TBAlfAdder)
{
	sc_out<bool> a, b , cin;
	sc_in<bool> sum, carry;

	void test()
	{
		struct inputOutputMap{
			int in1;
			int in2;
			int in3;
			int sum;
			int carry;
		};
		struct inputOutputMap test_input_output[8] = {
		// in1, in2 ,in3 , sum, carry
			{0,0,0,0,0},
			{0,0,1,1,0},
			{0,1,0,1,0},
			{0,1,1,0,1},
			{1,0,0,1,0},
			{1,0,1,0,1},
			{1,1,0,0,1},
			{1,1,1,1,1}
		};
		for(int loop_count =0;loop_count<8;loop_count++)
		{
			a.write(test_input_output[loop_count].in1);
			b.write(test_input_output[loop_count].in2);
			cin.write(test_input_output[loop_count].in3);
			wait(2, SC_NS);
			assert(sum.read() == test_input_output[loop_count].sum);
			assert(carry.read() == test_input_output[loop_count].carry);
		}
		sc_stop();
	}

	SC_CTOR(TBAlfAdder)
	{
		SC_THREAD(test);
	}
};

int sc_main(int argc, char *argv[])
{
	sc_signal<bool> a, b, cin, sum, carry;

	sc_trace_file *tf = sc_create_vcd_trace_file("trace");

	sc_trace(tf, a, "a");
	sc_trace(tf, b, "b");
	sc_trace(tf, cin, "cin");
	sc_trace(tf, sum, "sum");
	sc_trace(tf, carry, "carry");

	FullAdder dut("FullAdder");
	dut.a(a);
	dut.b(b);
	dut.cin(cin);
	dut.sum(sum);
	dut.carry(carry);

	TBAlfAdder tb("TBFullAdder");
	tb.a(a);
	tb.b(b);
	tb.cin(cin);
	tb.sum(sum);
	tb.carry(carry);

	sc_start();
	sc_close_vcd_trace_file(tf);
	return 0;
}
