#include <iostream>
#include <random>
#include "Software.h"
#include "tlm_utils/tlm_quantumkeeper.h"

// Temporal-decoupling variant of Software (same class, same Software.h
// -- this file is linked INSTEAD OF Software.cxx for Task 3).
//
// The quantum keeper lives here as a file-scope object rather than as a
// Software member, specifically so Software.h never has to change: only
// one Software object ever exists in any of these simulations, so a
// file-scope instance behaves exactly like a member would, without
// touching the already-tested header.
namespace {
tlm_utils::tlm_quantumkeeper g_qk;
}

void Software::run() {
    if (m_cfg.mode == RunMode::TEST) {
        run_once(test_sequence());
        std::cout << "Total time for sequence: " << m_total_time << std::endl;
    } else {
        std::mt19937 rng(m_cfg.rng_seed);
        for (int i = 0; i < m_cfg.iterations; ++i) {
            run_once(randomized_sequence(rng));
        }
        std::cout << "Ran " << m_cfg.iterations << " iterations, "
                  << m_total_pass << " passed, " << m_total_fail << " failed"
                  << std::endl;
    }
    sc_core::sc_stop();
}

void Software::run_once(const std::vector<Opcode> &commands) {
    sc_core::sc_time start = sc_core::sc_time_stamp();

    for (auto &cmd : commands) {
        tlm::tlm_generic_payload trans;
        Command *ext = new Command();
        ext->op = cmd.cmd;
        ext->x  = cmd.x;
        ext->y  = cmd.y;
        trans.set_extension(ext);

        if (cmd.cmd == Operation::WRITE && g_qk.get_local_time() != sc_core::SC_ZERO_TIME) {
            // Memory's busy contract needs the real kernel clock -- flush
            // any accumulated local time before this request can reach it.
            if (m_cfg.verbose) {
                std::cout << sc_core::sc_time_stamp()
                          << " Software: syncing " << g_qk.get_local_time()
                          << " of accumulated local time before a write"
                          << std::endl;
            }
            g_qk.sync();
        }

        tlm::tlm_phase   phase = tlm::BEGIN_REQ;
        sc_core::sc_time delay = g_qk.get_local_time();

        if (m_cfg.verbose) {
            std::cout << sc_core::sc_time_stamp() << " Software: sending command"
                      << std::endl;
        }
        tlm::tlm_sync_enum status = socket->nb_transport_fw(trans, phase, delay);

        if (status == tlm::TLM_COMPLETED) {
            // Arithmetic path: Cpu answered inline and handed back the
            // updated local-time offset via 'delay'. Store it instead of
            // calling wait() for it -- that's the actual decoupling.
            g_qk.set(delay);
            if (g_qk.need_sync()) {
                g_qk.sync();
            }
        } else {
            // Write path: really has to block for Memory's real ack.
            wait(m_resp_event);
            g_qk.reset(); // kernel clock and our local "now" agree again
        }

        Command *ret = trans.get_extension<Command>();

        bool ok;
        if (ret->op == Operation::WRITE) {
            ok = ret->valid;
            if (m_cfg.verbose) {
                std::cout << sc_core::sc_time_stamp() << " Software: write "
                          << (ok ? "acked OK" : "FAILED") << std::endl;
            }
        } else {
            long expected = 0;
            switch (ret->op) {
                case Operation::ADD: expected = cmd.x + cmd.y;            break;
                case Operation::SUB: expected = cmd.x - cmd.y;            break;
                case Operation::EQ:  expected = (cmd.x == cmd.y) ? 1 : 0; break;
                case Operation::REM: expected = cmd.x % cmd.y;            break;
                case Operation::WRITE: break; // handled above
            }
            ok = (ret->result == expected);
            if (m_cfg.verbose) {
                std::cout << sc_core::sc_time_stamp() << " Software: result = "
                          << ret->result << " (expected " << expected << ") "
                          << (ok ? "OK" : "MISMATCH") << std::endl;
            }
        }

        if (!ok) {
            std::cerr << sc_core::sc_time_stamp()
                      << " Software: VERIFICATION FAILED, command mismatch"
                      << std::endl;
            ++m_total_fail;
            if (m_cfg.exit_on_mismatch) {
                sc_core::sc_stop();
                return;
            }
            continue;
        }
        ++m_total_pass;
    }

    m_total_time += sc_core::sc_time_stamp() - start;
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