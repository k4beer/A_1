#ifndef CPU_H
#define CPU_H
#include "systemc.h"
#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/peq_with_get.h"
#include "Commands.h"

SC_MODULE(CPU) {
    public:
    tlm_utils::simple_target_socket<CPU> targ_socket;
    tlm_utils::simple_initiator_socket<CPU> init_socket;

    SC_CTOR(CPU) : targ_socket("targ_socket"), init_socket("init_socket") , m_peq("m_peq") {
        targ_socket.register_nb_transport_fw(this, &CPU::nb_transport_fw);
        init_socket.register_nb_transport_bw(this, &CPU::nb_transport_bw);
        SC_METHOD(peq_callback);
        sensitive << m_peq.get_event();
        dont_initialize();
    }
    private:
    
    tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload &trans,tlm::tlm_phase &phase, sc_core::sc_time &delay);
    tlm::tlm_sync_enum nb_transport_bw(tlm::tlm_generic_payload &trans, tlm::tlm_phase &phase, sc_core::sc_time &delay);
    void peq_callback();
    void forward_write(tlm::tlm_generic_payload &sw_trans);
    void complete_response(tlm::tlm_generic_payload &sw_trans, sc_core::sc_time delay);
    static sc_core::sc_time opcode_delay(Operation op);
    static long compute(Operation op, long x, long y);
    tlm_utils::peq_with_get<tlm::tlm_generic_payload> m_peq;

    tlm::tlm_generic_payload m_mem_trans;
    unsigned char m_write_buffer[sizeof(long)];
    tlm::tlm_generic_payload *m_pending_sw_trans;
};



#endif