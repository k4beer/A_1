#include "Memory.h"

//this is the wroite delay given in the assignemnt 
const sc_core::sc_time Memory::WRITE_DELAY = sc_core::sc_time(50, sc_core::SC_NS);

// The nb_transport_fw method is called by the CPU to send a transaction to the Memory module.
// If the Memory is busy, it defers the acknowledgment by notifying the m_ack_peq with a delay.
tlm::tlm_sync_enum Memory::nb_transport_fw(tlm::tlm_generic_payload &trans, tlm::tlm_phase &phase, sc_core::sc_time &delay) {
    if (phase == tlm::BEGIN_REQ) {
        sc_core::sc_time current_time = sc_core::sc_time_stamp();
        sc_core::sc_time ack_delay = (current_time < m_busy_until)
                                  ? (m_busy_until - current_time)
                                  : sc_core::SC_ZERO_TIME; //this is the delay for the ack if the memory is busy, if the memory is not busy then the delay is zero
       if (ack_delay == sc_core::SC_ZERO_TIME) {
            std::cout << current_time << " Memory: free, ack now" << std::endl;
        } else {
            std::cout << current_time << " Memory: busy until " << m_busy_until
                      << ", deferring ack by " << ack_delay << std::endl;
        }
        m_ack_peq.notify(trans, ack_delay); //notify the m_ack_peq with the transaction and the delay
        return tlm::TLM_ACCEPTED;
    }
    return tlm::TLM_ACCEPTED;
}


// The ack_peq_callback method is called when the m_ack_peq event queue has a transaction ready to be processed.
// It updates the busy time, sets the response status, and sends the response back to the CPU. This allows the 
// Memory to handle multiple transactions in a pipelined manner, respecting the WRITE_DELAY.
void Memory::ack_peq_callback() {
    tlm::tlm_generic_payload *trans;
    while ((trans = m_ack_peq.get_next_transaction()) != nullptr) {
        m_busy_until = sc_core::sc_time_stamp() + WRITE_DELAY;
        trans->set_response_status(tlm::TLM_OK_RESPONSE);
        std::cout << sc_core::sc_time_stamp() << " Memory: acking write, busy until " << m_busy_until << std::endl;
        tlm::tlm_phase phase = tlm::BEGIN_RESP;
        sc_core::sc_time delay = sc_core::SC_ZERO_TIME;
        socket->nb_transport_bw(*trans, phase, delay);
    }
}