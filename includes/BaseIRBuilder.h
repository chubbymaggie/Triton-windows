#ifndef BASEIRBUILDER_H
#define BASEIRBUILDER_H

#include <ostream>
#include <sstream>
#include <tuple>
#include <vector>

#include "ContextHandler.h"
#include "ControlFlow.h"
#include "IRBuilder.h"
#include "types.h"

// Abstract class which is main purpose is too implement common methods
// shared by its children classes.
class BaseIRBuilder: public IRBuilder {
  public:
    // Constructor take the two main informations of an instruction.
    BaseIRBuilder(uint64 address, const std::string &disassembly);

    virtual bool                isBranch(void);
    virtual const std::string   &getDisassembly(void) const;
    virtual int32_t             getOpcodeCategory(void);
    virtual uint32            getOpcode(void) const;
    virtual uint64            getAddress(void) const;
    virtual uint64            getThreadID(void) const;
    virtual void                setNextAddress(uint64 nextAddr);
    virtual void                setOpcode(uint32 op);
    virtual void                setOpcodeCategory(int32_t category);
    virtual void                setThreadID(uint64 threadId);


    virtual const std::vector<TritonOperand> &getOperands(void) const;

    // Add an operand to IRBuilder.
    // If it's type is:
    //  - IMM (Immediate), the value is the immediate value.
    //  - REG (Register), the value is the register ID.
    //  - MEM_*, the value doesn't mean anything and it's unused.
    //    The object will need a setup before any processing.
    virtual void addOperand(const TritonOperand &operand);

    virtual void setup(uint64 mem_value);

    virtual void checkSetup() const;

  protected:
    uint32                    opcode;
    uint64                    address;
    uint64                    nextAddress;
    std::string                 disas;
    bool                        needSetup;
    int32_t                     opcodeCategory;
    uint64                    threadId;
    std::vector<TritonOperand>  operands;
};

#endif // BASEIRBUILDER_H
