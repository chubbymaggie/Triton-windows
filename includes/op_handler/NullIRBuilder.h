#ifndef NULLIRBUILDER_H
#define NULLIRBUILDER_H

#include "types.h"

#include <string>

#include "BaseIRBuilder.h"
#include "TritonOperand.h"


// Null object, it's purpose is to handle "nicely" not implemented instructions.
class NullIRBuilder: public BaseIRBuilder {
  public:
    NullIRBuilder(uint64 address, const std::string &disas):
      BaseIRBuilder(address, disas) { }

    void addOperand(const TritonOperand &operand){
    }

    using BaseIRBuilder::getOperands;
    const std::vector<TritonOperand> &getOperands(void){
      return this->operands;
    }

    Inst *process(AnalysisProcessor &ap) const {
      ap.incNumberOfUnknownInstruction(); /* Used for statistics */
      return new Inst(ap.getCurrentCtxH()->getThreadID(), this->address, this->disas);
    }
};

#endif // NULLIRBUILDER_H
