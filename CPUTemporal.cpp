#include <cstring>
#include <iostream>
#include "Cpu.h"

// Temporal-decoupling variant of Cpu 
//arithematic oeprations now dont go through m_peq. Instead they are processed and handed back 
//with delay annotated. now it is upto the software if it wants to sync with kernel or it wants to
//keep the time as a quantum keeper and then sync later on. memory remians same as previous 
//implementations

sc_core::sc_time CPU::opcode_delay(Operation op) {
    switch (op) {
        case Operation::ADD: return sc_core::sc_time(10, sc_core::SC_NS);
        case Operation::SUB: return sc_core::sc_time(11, sc_core::SC_NS);
        case Operation::EQ:  return sc_core::sc_time(4,  sc_core::SC_NS);
        case Operation::REM: return sc_core::sc_time(15, sc_core::SC_NS);
        case Operation::WRITE: break; // handled separately, see forward_write()
    }
    return sc_core::SC_ZERO_TIME;
}

long CPU::compute(Operation op, long x, long y) {
    switch (op) {
        case Operation::ADD: return x + y;
        case Operation::SUB: return x - y;
        case Operation::EQ:  return (x == y) ? 1 : 0;
        case Operation::REM:
            if (y == 0) {
                std::cerr << sc_core::sc_time_stamp()
                          << " Cpu: REM by zero (x=" << x << ", y=" << y
                          << ") -- check the random generator bounds"
                          << std::endl;
                return 0;
            }
            return x % y;
        case Operation::WRITE: break; // handled separately see forward_write()
    }
    return 0;
}

tlm::tlm_sync_enum CPU::nb_transport_fw(tlm::tlm_generic_payload &trans,
                                         tlm::tlm_phase &phase,
                                         sc_core::sc_time &delay) {
    if (phase == tlm::BEGIN_REQ) {
        Command *ext = trans.get_extension<Command>();

        if (ext->op == Operation::WRITE) {
            forward_write(trans);
            return tlm::TLM_ACCEPTED; // response comes later via Memory's ack
        }

        // Decoupled path
        ext->result = compute(ext->op, ext->x, ext->y);
        delay += opcode_delay(ext->op);
        phase = tlm::BEGIN_RESP;
        return tlm::TLM_COMPLETED;
    }
    return tlm::TLM_ACCEPTED;
}

void CPU::peq_callback() {
    // Unused in the decoupled model 
}

void CPU::forward_write(tlm::tlm_generic_payload &sw_trans) {
    Command *ext = sw_trans.get_extension<Command>();
    m_pending_sw_trans = &sw_trans;

    //generic payload uses a native memory extension. 
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

    tlm::tlm_phase   mem_phase = tlm::BEGIN_REQ;
    sc_core::sc_time mem_delay = sc_core::SC_ZERO_TIME;
    init_socket->nb_transport_fw(m_mem_trans, mem_phase, mem_delay);
}

void CPU::complete_response(tlm::tlm_generic_payload &sw_trans,
                             sc_core::sc_time delay) {
    tlm::tlm_phase phase = tlm::BEGIN_RESP;
    targ_socket->nb_transport_bw(sw_trans, phase, delay);
}

tlm::tlm_sync_enum CPU::nb_transport_bw(tlm::tlm_generic_payload &trans,
                                         tlm::tlm_phase &phase,
                                         sc_core::sc_time &delay) {
    if (phase == tlm::BEGIN_RESP) {
        Command *ext = m_pending_sw_trans->get_extension<Command>();
        ext->valid = (trans.get_response_status() == tlm::TLM_OK_RESPONSE);

        std::cout << sc_core::sc_time_stamp()
                  << " Cpu: write acked by Memory, forwarding BEGIN_RESP"
                  << std::endl;

        complete_response(*m_pending_sw_trans, delay);
        m_pending_sw_trans = nullptr;

        phase = tlm::END_RESP;
        return tlm::TLM_COMPLETED;
    }
    return tlm::TLM_ACCEPTED;
}