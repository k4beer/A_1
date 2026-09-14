#ifndef Software_h
#define Software_h

#include "systemc.h"
#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "Commands.h"
#include "Trial.h"
#include <vector>

enum class RunMode { TEST, RANDOM, RANDOM_TEMP };

struct SoftwareConfig {
    RunMode mode = RunMode::TEST;
    int iterations = 1;
    unsigned rng_seed = 1;
    bool verbose = true;
    bool exit_on_mismatch = true;
};

SC_MODULE(Software) {
public:

    tlm_utils::simple_initiator_socket<Software> socket;

    SC_HAS_PROCESS(Software);

    Software(sc_core::sc_module_name name, SoftwareConfig cfg)
        : sc_core::sc_module(name),
          socket("socket"),
          m_cfg(cfg),
          m_total_time(sc_core::SC_ZERO_TIME),
          m_total_pass(0),
          m_total_fail(0) {

        socket.register_nb_transport_bw(
            this,
            &Software::nb_transport_bw
        );

        SC_THREAD(run);
    }

private:

    void run();

    void run_once(const std::vector<Opcode> &commands);

    tlm::tlm_sync_enum nb_transport_bw(
        tlm::tlm_generic_payload &trans,
        tlm::tlm_phase &phase,
        sc_core::sc_time &delay
    );

    sc_core::sc_event m_resp_event;

    SoftwareConfig m_cfg;

    sc_core::sc_time m_total_time;

    long m_total_pass;
    long m_total_fail;
};

#endif