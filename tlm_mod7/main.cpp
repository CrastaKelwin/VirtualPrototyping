// Does not show the non-blocking transport interface.


#include "initiator.h"
#include "target.h"

int sc_main(int argc, char* argv[])
{
   Initiator *initiator;
   mod7counter  *counter;
   // Instantiate components
    initiator = new Initiator("initiator");
    counter   = new mod7counter("counter");

    // One initiator is bound directly to one target with no intervening bus

    // Bind initiator socket to target socket
    initiator->socket.bind(counter->socket );
  sc_start();
  
  return 0;
  }
  
  
  
