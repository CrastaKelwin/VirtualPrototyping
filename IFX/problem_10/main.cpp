#include <systemc.h>

SC_MODULE(Dflipflop)
{
	sc_in<bool> D;
	sc_in<bool> Clock;

	sc_out<bool> Q;
	sc_out<bool> Qbar;

	void dflipflop()
	{
		Q.write(D.read());
		Qbar.write(!D.read());
	}
	SC_CTOR(Dflipflop)
	{
		SC_METHOD(dflipflop);
		sensitive << Clock.pos();
	}
};

SC_MODULE(TBDflipflop)
{

	sc_out<bool> D;
	sc_clock Clock;

	sc_in<bool> Q;
	sc_in<bool> Qbar;

	struct inputOutputMap
	{
		int D;
		int Q;
		int Qbar;
	};
	struct inputOutputMap test_input_output[8] = {
		// D,  Q, Qbar
		{0, 0, 1},
		{1, 1, 0},
		{0, 0, 1},
		{0, 0, 1},
		{1, 1, 0},
		{0, 0, 1},
		{1, 1, 0},
		{0, 0, 1}};
	int loop_count = 0;
	void test()
	{
		if (loop_count == sizeof(test_input_output) / sizeof(test_input_output[0]))
		{
			sc_stop();
		}
		else
		{
			if (!Clock.read())
			{
				if (loop_count > 0)// verify on neg edge
				{
					assert(Q.read() == test_input_output[loop_count - 1].Q);
					assert(Qbar.read() == test_input_output[loop_count - 1].Qbar);
				}
				D.write(test_input_output[loop_count++].D);
			}
		}
	}

	SC_CTOR(TBDflipflop) : Clock("clock", 2, SC_NS)
	{
		SC_METHOD(test);
		sensitive << Clock;
	}
};

int sc_main(int argc, char *argv[])
{
	sc_signal<bool> D, Q, Qbar;
	sc_trace_file *tf = sc_create_vcd_trace_file("trace");

	TBDflipflop tb("TBDflipflop");
	tb.D(D);
	tb.Q(Q);
	tb.Qbar(Qbar);

	Dflipflop dut("Dflipflop");
	dut.D(D);
	dut.Clock(tb.Clock);
	dut.Q(Q);
	dut.Qbar(Qbar);

	sc_trace(tf, D, "D");
	sc_trace(tf, tb.Clock, "Clock");
	sc_trace(tf, Q, "Q");
	sc_trace(tf, Qbar, "Qbar");

	sc_start();
	sc_close_vcd_trace_file(tf);
	return 0;
}
