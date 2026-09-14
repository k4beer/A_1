#include <chrono>
#include <iostream>
#include "systemc.h"
#include "tlm.h"
#include "tlm_utils/tlm_quantumkeeper.h"
#include "Software.h"
#include "Cpu.h"
#include "Memory.h"
#include "Trial.h"


int sc_main(int argc, char *argv[]) {
    tlm::tlm_global_quantum::instance().set(sc_core::sc_time(100, sc_core::SC_NS));

    SoftwareConfig cfg;
    cfg.mode             = RunMode::RANDOM;
    cfg.iterations       = 10000;
    cfg.rng_seed         = 42;    // matches task 2 seed
    cfg.verbose          = false; 
    cfg.exit_on_mismatch = false;

    Software sw("sw", cfg);
    CPU      cpu("cpu");
    Memory   mem("mem");

    sw.socket.bind(cpu.targ_socket);
    cpu.init_socket.bind(mem.socket);

    auto wall_start = std::chrono::steady_clock::now();
    sc_start();
    auto wall_end = std::chrono::steady_clock::now();

    double wall_seconds = std::chrono::duration<double>(wall_end - wall_start).count();
    std::cout << "Wall-clock time for " << cfg.iterations
              << " decoupled iterations: " << wall_seconds << " s" << std::endl;
    std::cout << "Simulated time reached: " << sc_core::sc_time_stamp() << std::endl;

    return 0;
}