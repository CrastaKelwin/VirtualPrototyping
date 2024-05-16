

#include "systemc"

using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"

// Initiator module generating generic payload transactions

struct Initiator : sc_module
{
    // TLM-2 socket, defaults to 32-bits wide, base protocol
    tlm_utils::simple_initiator_socket<Initiator> socket;

    SC_CTOR(Initiator) : socket("socket") // Construct and name socket
    {
        SC_THREAD(thread_process);
    }

    void thread_process()
    {
        // TLM-2 generic payload transaction, reused across calls to b_transport
        tlm::tlm_generic_payload *trans = new tlm::tlm_generic_payload;
        sc_time delay = sc_time(10, SC_NS);

        // Generate a random sequence of reads and writes
        for (int i = 1; i < 10; i++)
        {

            tlm::tlm_command cmd = static_cast<tlm::tlm_command>(i);

            // Initialize 8 out of the 10 attributes, byte_enable_length and extensions being unused
            trans->set_command(cmd);
            trans->set_response_status(tlm::TLM_INCOMPLETE_RESPONSE); // Mandatory initial value

            socket->b_transport(*trans, delay); // Blocking transport call

            // Initiator obliged to check response status and delay
            if (trans->is_response_error())
                SC_REPORT_ERROR("TLM-2", "Response error from b_transport");

            cout << "Transaction sent " << cmd << endl;

            // Realize the delay annotated onto the transport call
            wait(delay);
        }
    }

    // Internal data buffer used by initiator with generic payload
    int data;
};

// Needed for the simple_target_socket
#define SC_INCLUDE_DYNAMIC_PROCESSES

// Target module representing a simple memory

struct Target : sc_module
{
    // TLM-2 socket, defaults to 32-bits wide, base protocol
    tlm_utils::simple_target_socket<Target> socket;

    SC_CTOR(Target)
        : socket("socket")
    {
        // Register callback for incoming b_transport interface method call
        socket.register_b_transport(this, &Target::b_transport);
    }

    // TLM-2 blocking transport method
    virtual void b_transport(tlm::tlm_generic_payload &trans, sc_time &delay)
    {
        tlm::tlm_command cmd = trans.get_command();

        // Obliged to implement read and write commands
        cout << "Command Received " << "Hello World! " << cmd << endl;

        // Obliged to set response status to indicate successful completion
        trans.set_response_status(tlm::TLM_OK_RESPONSE);
    }
};

int sc_main(int argc, char *argv[])
{
    Initiator *initiator = new Initiator("initiator");
    Target *memory = new Target("memory");

    // Bind initiator socket to target socket
    initiator->socket.bind(memory->socket);
    sc_start();
    return 0;
}