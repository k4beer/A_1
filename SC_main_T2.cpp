#include <chrono>
#include <iostream>
#include "systemc.h"
#include "Software.h"
#include "Cpu.h"
#include "Memory.h"

//Task 2 involves usign random inputs to test the system. We will run 10,000 iterations of random inputs and measure the wall-clock time taken for the simulation to complete. The random inputs will be generated using a fixed seed for reproducibility.
int sc_main(int argc, char *argv[]) {
    SoftwareConfig cfg;
    cfg.mode             = RunMode::RANDOM;
    cfg.iterations       = 10000;
    cfg.rng_seed         = 42;    
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
    std::cout << "Wall-clock time for " << cfg.iterations << " iterations: "
              << wall_seconds << " s" << std::endl;
    std::cout << "Simulated time reached: " << sc_core::sc_time_stamp() << std::endl;

    return 0;
}