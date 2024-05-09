#ifndef TARGET_H
#define TARGET_H

// Needed for the simple_target_socket
#define SC_INCLUDE_DYNAMIC_PROCESSES

#include "systemc"
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"


// Target module representing a simple mod7counter

struct mod7counter: sc_module
{
  // TLM-2 socket, defaults to 32-bits wide, base protocol
  tlm_utils::simple_target_socket<mod7counter> socket;
  
  
  //sc_in<bool>clk;
  	int rst;
  	
	int counter;
	
	int registervalue;
	void compute()
	{
		if(rst)
		{
			registervalue=0;
			counter=registervalue;
		}
		else{
			registervalue=(registervalue+1)%7;
			counter=registervalue;
		}
		
		
		cout<<"rst="<<rst <<"register_value="<<registervalue<<endl;
		
		//wait(10);
	
	}
  SC_CTOR(mod7counter) : socket("socket")
  {
  		int i=0;
  		//while(i<16){
  		//	i++;
  			SC_THREAD(compute);
  			//wait(10);
			//sensitive <<clk;
    // Register callback for incoming b_transport interface method call
    socket.register_b_transport(this, &mod7counter::b_transport);
 }

  // TLM-2 blocking transport method
  virtual void b_transport( tlm::tlm_generic_payload& trans, sc_time& delay )
  {
    tlm::tlm_command cmd = trans.get_command();
    
    // Obliged to check address range and check for unsupported features,
    //   i.e. byte enables, streaming, and bursts
    // Can ignore DMI hint and extensions
    // Using the SystemC report handler is an acceptable way of signalling an error

    
    if ( cmd == tlm::TLM_READ_COMMAND) rst = false;
    else rst = true;
    
    trans.set_response_status(tlm::TLM_OK_RESPONSE );
    compute();
      }
};

#endif

