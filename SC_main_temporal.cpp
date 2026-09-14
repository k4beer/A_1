#include <chrono>
#include <iostream>
#include "systemc.h"
#include "tlm.h"
#include "tlm_utils/tlm_quantumkeeper.h"
#include "Software.h"
#include "Cpu.h"
#include "Memory.h"
#include "Trial.h"

// Task 3: identical benchmark to Task 2 -- same 14-command sequence
// shape, same rng_seed (so the exact same 10000 randomized sequences
// get generated), same iteration count -- but built against
// SoftwareTemporal.cxx and CpuTemporal.cxx instead of the plain
// Software.cxx/Cpu.cxx. Memory.cxx is reused completely unchanged:
// its busy/ack contract needs real global time, so it can't be
// decoupled (see the writeup for why).
int sc_main(int argc, char *argv[]) {
    // How far local time is allowed to run ahead of the kernel's clock
    // before Software is forced to sync. Generous relative to our
    // per-opcode delays (4-15ns) -- in practice, the pre-write sync
    // fires far more often than this quantum ever will for this
    // particular command sequence.
    tlm::tlm_global_quantum::instance().set(sc_core::sc_time(100, sc_core::SC_NS));

    SoftwareConfig cfg;
    cfg.mode             = RunMode::RANDOM;
    cfg.iterations       = 10000;
    cfg.rng_seed         = 42;    // must match Task 2's seed for a fair comparison
    cfg.verbose          = false; // same as Task 2 -- keep I/O overhead comparable
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