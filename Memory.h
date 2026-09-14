#ifndef Memory_h
#define Memory_h

#include "systemc.h"
#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"

SC_MODULE(Memory) {
    public:
    tlm_utils::simple_target_socket<Memory> socket; //making the socket public so the CPU can bind to it

    SC_CTOR(Memory) : socket("socket") , m_busy_until(sc_core::SC_ZERO_TIME), m_ack_peq("m_ack_peq") {
        socket.register_nb_transport_fw(this, &Memory::nb_transport_fw);
        SC_METHOD(ack_peq_callback);
        sensitive << m_ack_peq.get_event(); // making memory sensitive to the ack_peq event queue
        dont_initialize(); // prevent the method from being called at time 0
    }

    private:
    tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload &trans, tlm::tlm_phase &phase, sc_core::sc_time &delay);
    void ack_peq_callback();
    tlm_utils::peq_with_get<tlm::tlm_generic_payload> m_ack_peq;
    sc_core::sc_time m_busy_until;
    static const sc_core::sc_time WRITE_DELAY;
};



#endif