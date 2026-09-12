#ifndef Commands_h
#define Commands_h
#include "tlm.h"

enum class Operation {ADD, SUB, REM, EQ,WRITE};

class Command : public tlm::tlm_extension<Command> {
    public :
    Operation op;
    long x;
    long y;
    long result;
    bool valid;

    Command() : op(Operation::ADD), x(0), y(0), result(0), valid(false) {}

    //override the clone and copy_from methods from tlm_extension_base
    tlm::tlm_extension_base *clone() const override {
        return new Command(*this);
    }

    void copy_from(tlm::tlm_extension_base const &ext) override {
        *this = static_cast<Command const &>(ext);
    }
};


#endif