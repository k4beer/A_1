#ifndef Software_h
#define Software_h

#include "systemc.h"
#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "Commands.h"

SC_MODULE(Software) {
    public:
    tlm_utils::simple_initiator_socket<Software> socket; //creating a simple socket for the software module. 

    SC_CTOR(Software) : socket("socket") {
        socket.register_nb_transport_bw(this, &Software::nb_transport_bw); 
        SC_THREAD(run); //creating a thread for the software module. 
    }

    private:
    void run();
    tlm::tlm_sync_enum nb_transport_bw(tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, 
        sc_core::sc_time& delay);
    sc_core::sc_time m_delay; //delay for the software module.
};



#endif