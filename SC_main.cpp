#include "systemc.h"
#include "Software.h"
#include "Cpu.h"
#include "Memory.h"

int sc_main(int argc, char *argv[]) {
    SoftwareConfig cfg;
    cfg.mode             = RunMode::TEST;
    cfg.verbose          = true;
    cfg.exit_on_mismatch = true;

    Software sw("sw", cfg);
    CPU      cpu("cpu");
    Memory   mem("mem");

    sw.socket.bind(cpu.targ_socket);
    cpu.init_socket.bind(mem.socket);

    sc_start();

    std::cout << "Simulation finished at " << sc_core::sc_time_stamp() << std::endl;
    return 0;
}