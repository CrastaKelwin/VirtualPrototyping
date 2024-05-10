#include <systemc.h>

using namespace std;

SC_MODULE(Monitor) {
	sc_in<bool> clk;
	sc_in<bool> data_in;

	void monitor_process() {
    	cout << "@" << sc_time_stamp() << ": Received data = " << data_in.read() << endl;
	}

	SC_CTOR(Monitor) {
    	SC_METHOD(monitor_process);
    	sensitive << clk.pos();
	}
};

SC_MODULE(Driver) {
	sc_out<bool> data_out;
	sc_in<bool> clk;
    	string message = "1011101101010110"; // Example message
    	int index = 0;

	void driver_process() {

        	// Send data
        	data_out.write(message[index] == '1');
        	index = (index + 1);
            if(index == message.length())
            {
                sc_stop();
            }
	}

	SC_CTOR(Driver) {
    	SC_METHOD(driver_process);
        sensitive <<clk.pos();
	}
};

SC_MODULE(SubstrDetector) {
	sc_in<bool> clk;
	sc_in<bool> data_in;
	sc_out<bool> output;

	enum States {IDLE = 0, STATE1, STATE2} state;

	void fsm_process() {
                    	output.write(false);
    	// cout << " prev state = " << state << endl;
        	switch (state) {
            	case IDLE:
                	if (data_in.read()==0) {
                    	state = STATE1;
                	}
                	break;
            	case STATE1:
                	if (data_in.read()) {
                    	state = STATE2;
                	} else {
                    	state = STATE1;
                	}
                	break;
            	case STATE2:
    	// cout << "data rec "<<data_in.read()<<endl;
                	if (data_in.read() ==0 ) {
                    	state = STATE1;
                	} else {
    	cout << "sequence detected"<<endl;
                    	output.write(true);
                    	state = IDLE;
                	}
                	break;
        	}
    	// cout << "@" << sc_time_stamp() << " state changed to = " << state << endl;
	}

	SC_CTOR(SubstrDetector) {
    	SC_METHOD(fsm_process);
    	sensitive << clk.pos();
    	state = IDLE;
	}
};

int sc_main(int argc, char* argv[]) {
	sc_clock clk("clk", 10, SC_NS);
	sc_signal<bool> data;
	sc_signal<bool> output;

    sc_trace_file *tf = sc_create_vcd_trace_file("traces");
    sc_trace(tf,clk,"clk");
    sc_trace(tf,data,"data");
    sc_trace(tf,output,"out");

	Monitor monitor("monitor");
	monitor.clk(clk);
	monitor.data_in(data);

	Driver driver("driver");
	driver.data_out(data);
	driver.clk(clk);

	SubstrDetector detector("detector");
	detector.clk(clk);
	detector.data_in(data);
	detector.output(output);

	sc_start();
    sc_close_vcd_trace_file(tf);

	return 0;
}