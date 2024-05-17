#include <systemc.h>

SC_MODULE(Encoder)
{
	sc_out<bool> outputA0;
	sc_out<bool> outputA1;
	sc_in<bool> EnableInput;
	sc_in<bool> Y0;
	sc_in<bool> Y1;
	sc_in<bool> Y2;
	sc_in<bool> Y3;

	void decode()
	{
		if(EnableInput.read()==1)
		{
			outputA0.write(Y3.read() || Y1.read());
			outputA1.write(Y3.read() || Y2.read());
		}
	}
	SC_CTOR(Encoder)
	{
		SC_METHOD(decode);
		sensitive << Y0 << Y1 << Y2 << Y3 << EnableInput;
	}
};

SC_MODULE(TBEncoder)
{
	sc_in<bool> outputA0;
	sc_in<bool> outputA1;
	sc_out<bool> EnableInput;
	sc_out<bool> Y0;
	sc_out<bool> Y1;
	sc_out<bool> Y2;
	sc_out<bool> Y3;

	void test()
	{
		struct inputOutputMap{
			int EnableInput;
			int Y3;
			int Y2;
			int Y1;
			int Y0;
			int A1;
			int A0;
		};
		struct inputOutputMap test_input_output[11] = {
		//Enable,  Y3, Y2,  Y1, Y0	A1 , A0
			{0,		0,	0,	0,	0,	0,	0},
			{0,		0,	0,	0,	1,	0,	0},
			{0,		0,	0,	1,	0,	0,	0},
			{0,		0,	1,	0,	0,	0,	0},
			{0,		1,	0,	0,	0,	0,	0},
			{1,		0,	0,	0,	0,	0,	0},
			{1,		0,	0,	0,	1,	0,	0},
			{1,		0,	0,	1,	0,	0,	1},
			{1,		0,	1,	0,	0,	1,	0},
			{1,		1,	0,	0,	0,	1,	1},
			{0,		0,	0,	1,	0,	1,	1}
		};
		for(int loop_count =0;loop_count<11;loop_count++)
		{
			Y0.write(test_input_output[loop_count].Y0);
			Y1.write(test_input_output[loop_count].Y1);
			Y2.write(test_input_output[loop_count].Y2);
			Y3.write(test_input_output[loop_count].Y3);
			EnableInput.write(test_input_output[loop_count].EnableInput);
			wait(2, SC_NS);
			assert(outputA0.read() == test_input_output[loop_count].A0);
			assert(outputA1.read() == test_input_output[loop_count].A1);
		}
		sc_stop();
	}

	SC_CTOR(TBEncoder)
	{
		SC_THREAD(test);
	}
};

int sc_main(int argc, char *argv[])
{
	sc_signal<bool> A0, A1, EnableInput, Y0, Y1, Y2, Y3;

	sc_trace_file *tf = sc_create_vcd_trace_file("trace");

	sc_trace(tf, A1, "A1");
	sc_trace(tf, A0, "A0");
	sc_trace(tf, EnableInput, "EnableInput");
	sc_trace(tf, Y0, "Y0");
	sc_trace(tf, Y1, "Y1");
	sc_trace(tf, Y2, "Y2");
	sc_trace(tf, Y3, "Y3");

	Encoder dut("Encoder");
	dut.outputA0(A0);
	dut.outputA1(A1);
	dut.EnableInput(EnableInput);
	dut.Y0(Y0);
	dut.Y1(Y1);
	dut.Y2(Y2);
	dut.Y3(Y3);

	TBEncoder tb("TBEncoder");
	tb.outputA0(A0);
	tb.outputA1(A1);
	tb.EnableInput(EnableInput);
	tb.Y0(Y0);
	tb.Y1(Y1);
	tb.Y2(Y2);
	tb.Y3(Y3);

	sc_start();
	sc_close_vcd_trace_file(tf);
	return 0;
}
