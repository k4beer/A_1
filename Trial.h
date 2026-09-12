#ifndef TRIAL_H
#define TRIAL_H
#include "Commands.h"
#include <vector>

struct Opcode {
    Operation cmd; //calls this from the Commands file
    long x;
    long y;
};

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
    };
}


#endif