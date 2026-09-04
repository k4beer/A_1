#include "Cpu.h"
#include <iostream>

sc_core::sc_time CPU::opcode_delay(Operation op) {
    switch(op) {
        case Operation::ADD:
            return sc_core::sc_time(10, sc_core::SC_NS);
        case Operation::SUB:
            return sc_core::sc_time(11, sc_core::SC_NS);
        case Operation::REM:
            return sc_core::sc_time(15, sc_core::SC_NS);
        case Operation::EQ:
            return sc_core::sc_time(4, sc_core::SC_NS);
    }
    return sc_core::SC_ZERO_TIME;
}

long CPU::compute(Operation op, long x, long y) {
    switch(op) {
        case Operation::ADD:
            return x + y;
        case Operation::SUB:
            return x - y;
        case Operation::REM:
            return x % y;
        case Operation::EQ:
            return (x == y) ? 1 : 0;
    }
    return 0;
}


tlm::tlm_sync_enum CPU::nb_transport_fw(tlm::tlm_generic_payload &trans, tlm::tlm_phase &phase, sc_core::sc_time &delay) {
    if(phase == tlm::BEGIN_REQ) {
        Command *ext = trans.get_extension<Command>();
        sc_core::sc_time processing_delay = opcode_delay(ext->op);
        std::cout << sc_core::sc_time_stamp() << " CPU BEGIN_REQ accepted respondign in" << processing_delay << std::endl;
        m_peq.notify(trans, processing_delay);
        phase = tlm::END_REQ;
        return tlm::TLM_UPDATED;
    }
    if (phase == tlm::END_RESP) {
        return tlm::TLM_COMPLETED;
    }
    return tlm::TLM_ACCEPTED;
}

void CPU::peq_callback() {
    tlm::tlm_generic_payload *trans;
    while ((trans = m_peq.get_next_transaction()) != nullptr) {
        Command *ext = trans->get_extension<Command>();
        ext->result = compute(ext->op, ext->x, ext->y);

        std::cout << sc_core::sc_time_stamp() << " CPU processing complete, result = " << ext->result << std::endl;
        tlm::tlm_phase phase = tlm::BEGIN_RESP;
        sc_core::sc_time delay = sc_core::SC_ZERO_TIME;
        targ_socket->nb_transport_bw(*trans, phase, delay);
    }
}

tlm::tlm_sync_enum CPU::nb_transport_bw(tlm::tlm_generic_payload &trans, tlm::tlm_phase &phase, sc_core::sc_time &delay) {
    return tlm::TLM_ACCEPTED;
}