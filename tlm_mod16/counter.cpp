#include "systemc.h"
#include "counter.h"


#include "initiator.h"


int sc_main (int argc, char* argv[]) {
  sc_signal<bool>   clock;
  sc_signal<bool>   enable;
  sc_signal<sc_uint<4> > counter_out;
  int i = 0;
  int rst =0;
  

   Initiator *initiator;
   Counter  *counter;

    // Instantiate components
    initiator = new Initiator("initiator");
    counter   = new Counter("counter");

    // One initiator is bound directly to one target with no intervening bus

    // Bind initiator socket to target socket
    initiator->socket.bind(counter->socket );
    counter->clock(clock);
    counter->enable(enable);
    counter->counter_out(counter_out);
  //sc_start();
  
 sc_start(30,SC_NS);

  // Open VCD file
  sc_trace_file *wf = sc_create_vcd_trace_file("counter");
  // Dump the desired signals
  sc_trace(wf, clock, "clock");
  sc_trace(wf, enable, "enable");
  sc_trace(wf, counter_out, "count");

  // Initialize all variables
  rst = 0;       // initial value of reset
  enable = 0;      // initial value of enable
  for (i=0;i<5;i++) {
    clock = 0; 
    sc_start(30,SC_NS);
    clock = 1; 
   sc_start(30,SC_NS);
  }
  rst = 1;    // Assert the reset
  cout << "@" << sc_time_stamp() <<" Asserting reset\n" << endl;
  for (i=0;i<10;i++) {
    clock = 0; 
    sc_start(30,SC_NS);
    clock = 1; 
    sc_start(30,SC_NS);
  }
  rst = 0;    // De-assert the reset
  cout << "@" << sc_time_stamp() <<" De-Asserting reset\n" << endl;
  for (i=0;i<5;i++) {
    clock = 0; 
   sc_start(30,SC_NS);
    clock = 1; 
   sc_start(30,SC_NS);
  }
  cout << "@" << sc_time_stamp() <<" Asserting Enable\n" << endl;
  enable = 1;  // Assert enable
  for (i=0;i<20;i++) {
    clock = 0; 
   sc_start(30,SC_NS);
    clock = 1; 
   sc_start(30,SC_NS);
  }
  cout << "@" << sc_time_stamp() <<" De-Asserting Enable\n" << endl;
  enable = 0; // De-assert enable

  cout << "@" << sc_time_stamp() <<" Terminating simulation\n" << endl;
  sc_close_vcd_trace_file(wf);
  return 0;// Terminate simulation

 }
