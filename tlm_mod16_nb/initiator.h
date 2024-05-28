#ifndef INITIATOR_H
#define INITIATOR_H

#include "systemc"
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"

// Initiator module generating generic payload transactions

struct Initiator : sc_module
{

  tlm_utils::simple_initiator_socket<Initiator> socket;

  SC_CTOR(Initiator)
      : socket("socket") // Construct and name socket
  {
    SC_THREAD(thread_process);
  }

  void thread_process()
  {
    // TLM-2 generic payload transaction, reused across calls to b_transport
    tlm::tlm_generic_payload *trans = new tlm::tlm_generic_payload;
    sc_time delay = sc_time(250, SC_NS);

    // Generate a random sequence of reads and writes
    for (int i = 32; i <= 96; i += 4)
    {
      tlm::tlm_command cmd = static_cast<tlm::tlm_command>((rand() % 1) + i / 64);
      tlm::tlm_phase phase = tlm::BEGIN_REQ;

      trans->set_command(cmd);

      trans->set_response_status(tlm::TLM_INCOMPLETE_RESPONSE); // Mandatory initial value

      socket->nb_transport_fw(*trans, phase, delay); // Blocking transport call

      cout << "trans = { " << (cmd ? 'W' : 'R') << ", " << hex << cmd << "}" << endl;

      // Wait for 10 ns before the next transaction
      wait(250, SC_NS);

      // Realize the delay annotated onto the transport call
      wait(delay);
    }
  }

  // Internal data buffer used by initiator with generic payload
  int data;
};

#endif
