#include "Cpu.h"

tlm::tlm_sync_enum CPU::nb_transport_fw(tlm::tlm_generic_payload &trans, tlm::tlm_phase &phase, sc_core::sc_time &delay) {
    if(phase == tlm::BEGIN_REQ) {
        Command *ext = trans.get_extension<Command>();
        ext->result = ext->x + ext->y;
        phase = tlm::BEGIN_RESP;
        return tlm::TLM_COMPLETED;
    }
    return tlm::TLM_ACCEPTED;
}

tlm::tlm_sync_enum CPU::nb_transport_bw(tlm::tlm_generic_payload &trans, tlm::tlm_phase &phase, sc_core::sc_time &delay) {
    return tlm::TLM_ACCEPTED;
}