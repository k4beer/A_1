#include "systemc.h"
#include "Software.h"
#include "Cpu.h"
#include "Memory.h"

int sc_main(int argc, char* argv[]) {
    Software software("software");
    CPU cpu("cpu");
    Memory memory("memory");

    software.socket.bind(cpu.targ_socket);
    cpu.init_socket.bind(memory.socket);

    sc_core::sc_start();
    std::cout << "Simulation finished at " << sc_core::sc_time_stamp() << std::endl;
    return 0;
}