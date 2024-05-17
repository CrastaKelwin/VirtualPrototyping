#include <systemc.h>

SC_MODULE(Mux)
{
	sc_in<bool> inputA;
	sc_in<bool> inputB;
	sc_in<bool> inputC;
	sc_in<bool> inputD;

	sc_in<bool> inputCtrl0;
	sc_in<bool> inputCtrl1;

	sc_out<bool> outMux;

	void Operation()
	{
		int out = (((int)inputCtrl1.read())<<1)|inputCtrl0.read();
		switch (out)
		{
		case 0:
			outMux.write(inputA.read());
			break;
		case 1:
			outMux.write(inputB.read());
			break;
		case 2:
			outMux.write(inputC.read());
			break;
		case 3:
			outMux.write(inputD.read());
			break;
		
		default:
			break;
		}
	}
	SC_CTOR(Mux)
	{
		SC_METHOD(Operation);
		sensitive << inputA << inputB << inputC << inputD << inputCtrl0 << inputCtrl1;
	}
};

SC_MODULE(TBMux)
{

	sc_out<bool> inputA;
	sc_out<bool> inputB;
	sc_out<bool> inputC;
	sc_out<bool> inputD;

	sc_out<bool> inputCtrl0;
	sc_out<bool> inputCtrl1;

	sc_in<bool> outMux;

	void test()
	{
		struct inputOutputMap{
			int inputD;
			int inputC;
			int inputB;
			int inputA;
			int inputCtrl1;
			int inputCtrl0;
			int outMux;
		};
		struct inputOutputMap test_input_output[] = {
		//inputD,  inputC, inputB,  inputA, inputCtrl1	inputCtrl0 , outMux
			{0,		0,		0,			0,		0,			0,			0},
			{0,		0,		1,			0,		0,			1,			1},
			{0,		0,		0,			0,		1,			0,			0},
			{1,		0,		1,			1,		1,			1,			1},
			{1,		0,		1,			0,		0,			0,			0},
			{0,		1,		1,			0,		0,			1,			1},
			{1,		0,		0,			1,		1,			0,			0},
			{1,		0,		1,			1,		1,			1,			1},
			{1,		0,		1,			1,		0,			0,			1},
			{0,		1,		0,			0,		0,			1,			0},
			{1,		1,		0,			1,		1,			0,			1},
			{0,		0,		1,			1,		1,			1,			0},
		};
		for(int loop_count =0;loop_count<sizeof(test_input_output)/sizeof(test_input_output[0]);loop_count++)
		{
			inputD.write(test_input_output[loop_count].inputD);
			inputC.write(test_input_output[loop_count].inputC);
			inputB.write(test_input_output[loop_count].inputB);
			inputA.write(test_input_output[loop_count].inputA);
			inputCtrl1.write(test_input_output[loop_count].inputCtrl1);
			inputCtrl0.write(test_input_output[loop_count].inputCtrl0);
			wait(2, SC_NS);
			assert(outMux.read() == test_input_output[loop_count].outMux);
		}
		sc_stop();
	}

	SC_CTOR(TBMux)
	{
		SC_THREAD(test);
	}
};

int sc_main(int argc, char *argv[])
{
	sc_signal<bool> inputA, inputB, inputC, inputD, inputCtrl0, inputCtrl1, outMux;

	sc_trace_file *tf = sc_create_vcd_trace_file("trace");

	sc_trace(tf, inputA, "inputA");
	sc_trace(tf, inputB, "inputB");
	sc_trace(tf, inputC, "inputC");
	sc_trace(tf, inputD, "inputD");
	sc_trace(tf, inputCtrl0, "inputCtrl0");
	sc_trace(tf, inputCtrl1, "inputCtrl1");
	sc_trace(tf, outMux, "outMux");

	Mux dut("Mux");
	dut.inputA(inputA);
	dut.inputB(inputB);
	dut.inputC(inputC);
	dut.inputD(inputD);
	dut.inputCtrl0(inputCtrl0);
	dut.inputCtrl1(inputCtrl1);
	dut.outMux(outMux);

	TBMux tb("TBMux");
	tb.inputA(inputA);
	tb.inputB(inputB);
	tb.inputC(inputC);
	tb.inputD(inputD);
	tb.inputCtrl0(inputCtrl0);
	tb.inputCtrl1(inputCtrl1);
	tb.outMux(outMux);

	sc_start();
	sc_close_vcd_trace_file(tf);
	return 0;
}
