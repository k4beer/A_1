#include <iostream>
#include "Software.h"
#include "Commands.h"
#include "Trial.h"

void Software::run() {
    auto commands = test_sequence();
    int pass = 0, fail = 0;

    for (auto &cmd : commands) {
        tlm::tlm_generic_payload trans;
        Command *ext = new Command();
        ext->op = cmd.cmd;
        ext->x  = cmd.x;
        ext->y  = cmd.y;
        trans.set_extension(ext);

        tlm::tlm_phase   phase = tlm::BEGIN_REQ;
        sc_core::sc_time delay = sc_core::SC_ZERO_TIME;

        std::cout << sc_core::sc_time_stamp() << " Software: sending command"
                  << std::endl;
        tlm::tlm_sync_enum status = socket->nb_transport_fw(trans, phase, delay);

        if (status != tlm::TLM_COMPLETED) {
            // The normal path now that Cpu schedules a real delay: wait
            // for nb_transport_bw to deliver BEGIN_RESP later.
            wait(m_resp_event);
        }

        Command *ret = trans.get_extension<Command>();

        // Lightweight check for now, just to eyeball correctness while
        // verifying timing. The formal "print + exit on mismatch"
        // self-check contract from the assignment lands in Phase 5.
        long expected = 0;
        switch (cmd.cmd) {
            case Operation::ADD: expected = cmd.x + cmd.y;              break;
            case Operation::SUB: expected = cmd.x - cmd.y;              break;
            case Operation::EQ:  expected = (cmd.x == cmd.y) ? 1 : 0;   break;
            case Operation::REM: expected = cmd.x % cmd.y;              break;
        }
        bool ok = (ret->result == expected);
        ok ? ++pass : ++fail;
        std::cout << sc_core::sc_time_stamp() << " Software: result = "
                  << ret->result << " (expected " << expected << ") "
                  << (ok ? "OK" : "MISMATCH") << std::endl;
    }

    std::cout << "Done: " << pass << " passed, " << fail << " failed"
              << std::endl;
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