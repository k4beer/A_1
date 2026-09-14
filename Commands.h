#ifndef Commands_h
#define Commands_h
#include "tlm.h"

//These are the types of operatiosn we will be perfomring. SImilar to opcodes
enum class Operation {ADD, SUB, REM, EQ,WRITE};

//SInce a geernic payload does not have the ability to carry the operations we want, we will 
//use tlm_extension to create a custom payload that can carry the operations we want.
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