#include <iostream>
#include "Software.h"
 
void Software::run() {
    tlm::tlm_generic_payload trans;
    Command *ext = new Command();
    ext->op = Operation::ADD;
    ext->x  = 2;
    ext->y  = 3;
    trans.set_extension(ext);
 
    tlm::tlm_phase   phase = tlm::BEGIN_REQ;
    sc_core::sc_time delay = sc_core::SC_ZERO_TIME;
 
    tlm::tlm_sync_enum status = socket->nb_transport_fw(trans, phase, delay);
 
    if (status == tlm::TLM_COMPLETED) {
        // CPU answered inline -- the "immediate completion" shortcut the
        // AT base protocol allows when no delay is involved.
        Command *ret = trans.get_extension<Command>();
        std::cout << sc_core::sc_time_stamp()
                  << " Software: got result = " << ret->result << std::endl;
    } else {
        // The real, timed command sequence (Phase 3) will actually take
        // this path: wait for nb_transport_bw to deliver BEGIN_RESP later.
        wait(m_resp_event);
    }
 
    sc_core::sc_stop();
}
 
tlm::tlm_sync_enum Software::nb_transport_bw(tlm::tlm_generic_payload &trans,
                                              tlm::tlm_phase &phase,
                                              sc_core::sc_time &delay) {
    if (phase == tlm::BEGIN_RESP) {
        m_resp_event.notify(delay);
        phase = tlm::END_RESP;
        return tlm::TLM_COMPLETED;
    }
    return tlm::TLM_ACCEPTED;
}