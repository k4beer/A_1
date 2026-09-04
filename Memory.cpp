#include "Memory.h"

const sc_core::sc_time Memory::WRITE_DELAY = sc_core::sc_time(50, sc_core::SC_NS);

tlm::tlm_sync_enum Memory::nb_transport_fw(tlm::tlm_generic_payload &trans, tlm::tlm_phase &phase, sc_core::sc_time &delay) {
    trans.set_response_status(tlm::TLM_OK_RESPONSE);
    return tlm::TLM_COMPLETED;
}