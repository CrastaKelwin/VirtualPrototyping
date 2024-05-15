#include <systemc.h>

SC_MODULE(Tflipflop)
{
	sc_in<bool> T;
	sc_in<bool> Clock;

	sc_out<bool> Q;
	sc_out<bool> Qbar;

	void dflipflop()
	{
		if(T.read())
		{
			bool prev = Q.read();
			Q.write(!prev);
			Qbar.write(prev);
		}
	}
	SC_CTOR(Tflipflop)
	{
		SC_METHOD(dflipflop);
		sensitive << Clock.pos();
	}
};

SC_MODULE(TBTflipflop)
{

	sc_out<bool> T;
	sc_clock Clock;

	sc_in<bool> Q;
	sc_in<bool> Qbar;

	struct inputOutputMap
	{
		int T;
		int Q;
		int Qbar;
	};
	struct inputOutputMap test_input_output[9] = {
		// T,  Q, Qbar
		{1, 1, 0},
		{1, 0, 1},
		{1, 1, 0},
		{0, 1, 0},
		{0, 1, 0},
		{1, 0, 1},
		{0, 0, 1},
		{1, 1, 0},
		{0, 1, 0}
		};
	int loop_count = 0;
	void test()
	{
		if (loop_count == (sizeof(test_input_output) / sizeof(test_input_output[0])))
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
				T.write(test_input_output[loop_count++].T);
			}
		}
	}

	SC_CTOR(TBTflipflop) : Clock("clock", 2, SC_NS)
	{
		SC_METHOD(test);
		sensitive << Clock;
	}
};

int sc_main(int argc, char *argv[])
{
	sc_signal<bool> T, Q, Qbar;
	sc_trace_file *tf = sc_create_vcd_trace_file("trace");

	TBTflipflop tb("TBTflipflop");
	tb.T(T);
	tb.Q(Q);
	tb.Qbar(Qbar);

	Tflipflop dut("Tflipflop");
	dut.T(T);
	dut.Clock(tb.Clock);
	dut.Q(Q);
	dut.Qbar(Qbar);

	sc_trace(tf, T, "T");
	sc_trace(tf, tb.Clock, "Clock");
	sc_trace(tf, Q, "Q");
	sc_trace(tf, Qbar, "Qbar");

	sc_start();
	sc_close_vcd_trace_file(tf);
	return 0;
}
