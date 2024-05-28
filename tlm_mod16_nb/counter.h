//-----------------------------------------------------
// This is my second Systemc Example
// Design Name : first_counter
// File Name : first_counter.cpp
// Function : This is a 4 bit up-counter with
// Synchronous active high reset and
// with active high enable signal
//-----------------------------------------------------
#ifndef COUNTER_H
#define COUNTER_H
#include "systemc.h"
// Needed for the simple_target_socket
#define SC_INCLUDE_DYNAMIC_PROCESSES

#include "systemc"
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"

SC_MODULE(Counter)
{
  sc_in_clk clock;                // Clock input of the design
  int rst = 0;                    // active high, synchronous Reset input
  sc_in<bool> enable;             // Active high enable signal for counter
  sc_out<sc_uint<4>> counter_out; // 4 bit vector output of the counter

  //------------Local Variables Here---------------------
  sc_uint<4> count;
  tlm_utils::simple_target_socket<Counter> socket;

  //------------Code Starts Here-------------------------
  // Below function implements actual counter logic
  void incr_count()
  {
    // At every rising edge of clock we check if reset is active
    // If active, we load the counter output with 4'b0000
    cout << "Pos Edge detected: rst" << rst << ", en:" << enable.read() << endl;
    if (rst == 1)
    {
      count = 0;
      counter_out.write(count);
      // If enable is active, then we increment the counter
    }
    else if (enable.read() == 1)
    {
      count = count + 1;
      counter_out.write(count);
      cout << "@" << sc_time_stamp() << " :: Incremented Counter " << count << endl;
    }
    else
    {

      cout << "@ Counter disabled " << endl;
    }
  } // End of function incr_count

  tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload & trans, tlm::tlm_phase & phase, sc_time & delay)
  {
    if (phase == tlm::BEGIN_REQ)
    {

      tlm::tlm_command cmd = trans.get_command();
      cout << "@Transaction received " << sc_time_stamp() << endl;
      cout << "@cmd " << cmd << endl;
      if (cmd == tlm::TLM_READ_COMMAND)
        rst = 0;
      else
        rst = 1;

      delay = sc_time(10, SC_NS);
      phase = tlm::END_REQ;
      return tlm::TLM_UPDATED;
    }
    else if (phase == tlm::END_REQ)
    {
      return tlm::TLM_COMPLETED;
    }
    return tlm::TLM_ACCEPTED;
  }

  // Constructor for the counter
  // Since this counter is a positive edge trigged one,
  // We trigger the below block with respect to positive
  // edge of the clock and also when ever reset changes state
  SC_CTOR(Counter) : socket("socket")
  {
    cout << "Executing new" << endl;
    SC_METHOD(incr_count);
    sensitive << clock.pos();
    socket.register_nb_transport_fw(this, &Counter::nb_transport_fw);
  } // End of Constructor

}; // End of Module counter
#endif // COUNTER_H
