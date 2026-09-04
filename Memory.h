#ifndef Memory_h
#define Memory_h

#include "systemc.h"
#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"

SC_MODULE(Memory) {
    public:
    tlm_utils::simple_target_socket<Memory> socket;

    SC_CTOR(Memory) : socket("socket") {
        socket.register_nb_transport_fw(this, &Memory::nb_transport_fw);
    }

    private:
    tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload &trans, tlm::tlm_phase &phase, sc_core::sc_time &delay);
    bool m_writing;
    sc_core::sc_time m_busy_until;
    static const sc_core::sc_time WRITE_DELAY;
};



#endif