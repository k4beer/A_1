#include "Cpu.h"
#include <iostream>

//add the custom delay for each opcode. This is used to simulate the time taken by the CPU to 
//process each operation.
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
        case Operation::WRITE:
            break;
    }
    return sc_core::SC_ZERO_TIME;
}

//this is for computation of the result of the operation. This is used to simulate the
// CPU processing the operation and generating a result.
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
        case Operation::WRITE:
            break;
    }
    return 0;
}

//nb_transport_fw is the forward transport method for the CPU. It is called by the Software 
//module when it wants to send a command to the CPU. The CPU will process the command and send a
// response back to the Software module.
tlm::tlm_sync_enum CPU::nb_transport_fw(tlm::tlm_generic_payload &trans, tlm::tlm_phase &phase, sc_core::sc_time &delay) {
    if(phase == tlm::BEGIN_REQ) {
        Command *ext = trans.get_extension<Command>();
        if(ext->op == Operation::WRITE) {
            forward_write(trans);
            return tlm::TLM_ACCEPTED;
        }
        sc_core::sc_time op_delay = opcode_delay(ext->op);
        m_peq.notify(trans, op_delay);
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

void CPU::forward_write(tlm::tlm_generic_payload &sw_trans) {
    Command *ext = sw_trans.get_extension<Command>();
    m_pending_sw_trans = &sw_trans;

    std::memcpy(m_write_buffer, &ext->x, sizeof(ext->x));
    m_mem_trans.set_command(tlm::TLM_WRITE_COMMAND);
    m_mem_trans.set_address(static_cast<sc_dt::uint64>(ext->y));
    m_mem_trans.set_data_ptr(m_write_buffer);
    m_mem_trans.set_data_length(sizeof(ext->x));
    m_mem_trans.set_streaming_width(sizeof(ext->x));
    m_mem_trans.set_byte_enable_ptr(nullptr);
    m_mem_trans.set_dmi_allowed(false);
    m_mem_trans.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);

    std::cout << sc_core::sc_time_stamp() << " Cpu: forwarding write x="
              << ext->x << " to address 0x" << std::hex << ext->y
              << std::dec << std::endl;
    tlm::tlm_phase phase = tlm::BEGIN_REQ;
    sc_core::sc_time delay = sc_core::SC_ZERO_TIME;
    init_socket->nb_transport_fw(m_mem_trans, phase, delay);
}

void CPU::complete_response(tlm::tlm_generic_payload &sw_trans, sc_core::sc_time delay) {
    tlm::tlm_phase phase = tlm::BEGIN_RESP;
    targ_socket->nb_transport_bw(sw_trans, phase, delay);
}

tlm::tlm_sync_enum CPU::nb_transport_bw(tlm::tlm_generic_payload &trans, tlm::tlm_phase &phase, sc_core::sc_time &delay) {
    if (phase == tlm::BEGIN_RESP) {
        Command *ext = m_pending_sw_trans->get_extension<Command>();
        ext->valid = (trans.get_response_status() == tlm::TLM_OK_RESPONSE);
        complete_response(*m_pending_sw_trans, delay);
        m_pending_sw_trans = nullptr;
        phase = tlm::END_RESP;
        return tlm::TLM_COMPLETED;
    }
    return tlm::TLM_ACCEPTED;
}
