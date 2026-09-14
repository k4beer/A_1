#ifndef TRIAL_H
#define TRIAL_H
#include "Commands.h"
#include <vector>
#include <random>

struct Opcode {
    Operation cmd; //calls this from the Commands file
    long x;
    long y;
};

//this is task 1 sequence. These commands are called when SC_main is executed. The commands 
//are executed in the order they are listed here.
inline std::vector<Opcode> test_sequence() {
    return {
        {Operation::ADD,   4,   36},
        {Operation::EQ,    49,  55},
        {Operation::REM,   20,  2},
        {Operation::WRITE, 100, 0xfff0},
        {Operation::SUB,   9,   12},
        {Operation::WRITE, 21,  0xff00},
        {Operation::ADD,   12,  1000},
        {Operation::REM,   5,   8},
        {Operation::WRITE, 21,  0xff40},
        {Operation::REM,   100, 7},
        {Operation::SUB,   125, 25},
        {Operation::EQ,    47,  47},
        {Operation::ADD,   5,   6},
        {Operation::WRITE, 21,  0xff80},
    };}

    //this allows randomised vectors to be generated for task 2. The randomised vectors are 
    //generated using a fixed seed for reproducibility. We use a uniform distribution to generate random
    // numbers between 1 and 100 for the x and y operands of the commands. The write commands are not randomized,
    // as they are used to write specific values to memory.
    inline std::vector<Opcode> randomized_sequence(std::mt19937 &rng) {
        static std::uniform_int_distribution<long> dist(1, 100);
        std::vector<Opcode> seq = test_sequence();
        for (auto &cmd : seq) {
            if(cmd.cmd != Operation::WRITE) {
                cmd.x = dist(rng);
                cmd.y = dist(rng);
            }
        }
        return seq;
    }


#endif