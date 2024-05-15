#include <systemc.h>

SC_MODULE(Decoder)
{
	sc_in<bool> inputA0;
	sc_in<bool> inputA1;
	sc_in<bool> EnableInput;
	sc_out<bool> Y0;
	sc_out<bool> Y1;
	sc_out<bool> Y2;
	sc_out<bool> Y3;

	void decode()
	{
		sc_uint<4> out = 0;
		if(EnableInput.read()==1)
		{
			out[((int)inputA1.read()) <<1 | inputA0.read()] = 1;
		}
		Y0.write(out[0]);
		Y1.write(out[1]);
		Y2.write(out[2]);
		Y3.write(out[3]);
	}
	SC_CTOR(Decoder)
	{
		SC_METHOD(decode);
		sensitive << inputA0 << inputA1 << EnableInput;
	}
};

SC_MODULE(TBDecoder)
{
	sc_out<bool> inputA0;
	sc_out<bool> inputA1;
	sc_out<bool> EnableInput;
	sc_in<bool> Y0;
	sc_in<bool> Y1;
	sc_in<bool> Y2;
	sc_in<bool> Y3;

	void test()
	{
		struct inputOutputMap{
			int inputA1;
			int inputA0;
			int EnableInput;
			int Y3;
			int Y2;
			int Y1;
			int Y0;
		};
		struct inputOutputMap test_input_output[8] = {
		// inputA1, inputA0 ,EnableInput , Y3, Y2, Y1, Y0
			{0,		0,			0,			0,	0,	0,	0},
			{0,		0,			1,			0,	0,	0,	1},
			{0,		1,			0,			0,	0,	0,	0},
			{0,		1,			1,			0,	0,	1,	0},
			{1,		0,			0,			0,	0,	0,	0},
			{1,		0,			1,			0,	1,	0,	0},
			{1,		1,			0,			0,	0,	0,	0},
			{1,		1,			1,			1,	0,	0,	0}
		};
		for(int loop_count =0;loop_count<8;loop_count++)
		{
			inputA0.write(test_input_output[loop_count].inputA0);
			inputA1.write(test_input_output[loop_count].inputA1);
			EnableInput.write(test_input_output[loop_count].EnableInput);
			wait(2, SC_NS);
			assert(Y0.read() == test_input_output[loop_count].Y0);
			assert(Y1.read() == test_input_output[loop_count].Y1);
			assert(Y2.read() == test_input_output[loop_count].Y2);
			assert(Y3.read() == test_input_output[loop_count].Y3);
		}
		sc_stop();
	}

	SC_CTOR(TBDecoder)
	{
		SC_THREAD(test);
	}
};

int sc_main(int argc, char *argv[])
{
	sc_signal<bool> inputA0, inputA1, EnableInput, Y0, Y1, Y2, Y3;

	sc_trace_file *tf = sc_create_vcd_trace_file("trace");

	sc_trace(tf, inputA1, "inputA1");
	sc_trace(tf, inputA0, "inputA0");
	sc_trace(tf, EnableInput, "EnableInput");
	sc_trace(tf, Y0, "Y0");
	sc_trace(tf, Y1, "Y1");
	sc_trace(tf, Y2, "Y2");
	sc_trace(tf, Y3, "Y3");

	Decoder dut("Decoder");
	dut.inputA1(inputA1);
	dut.inputA0(inputA0);
	dut.EnableInput(EnableInput);
	dut.Y0(Y0);
	dut.Y1(Y1);
	dut.Y2(Y2);
	dut.Y3(Y3);

	TBDecoder tb("TBDecoder");
	tb.inputA1(inputA1);
	tb.inputA0(inputA0);
	tb.EnableInput(EnableInput);
	tb.Y0(Y0);
	tb.Y1(Y1);
	tb.Y2(Y2);
	tb.Y3(Y3);

	sc_start();
	sc_close_vcd_trace_file(tf);
	return 0;
}
