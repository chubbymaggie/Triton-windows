#include <algorithm>
#include <sstream>
#include <stdexcept>

#include <MovhlpsIRBuilder.h>
#include <Registers.h>
#include <SMT2Lib.h>
#include <SymbolicElement.h>


MovhlpsIRBuilder::MovhlpsIRBuilder(uint64 address, const std::string &disassembly):
  BaseIRBuilder(address, disassembly) {
}


void MovhlpsIRBuilder::regImm(AnalysisProcessor &ap, Inst &inst) const {
  TwoOperandsTemplate::stop(this->disas);
}


void MovhlpsIRBuilder::regReg(AnalysisProcessor &ap, Inst &inst) const {
  SymbolicElement   *se;
  std::stringstream expr, op1, op2;
  uint64            reg1      = this->operands[0].getValue();
  uint64            regSize1  = this->operands[0].getSize();
  uint64            reg2      = this->operands[1].getValue();
  uint64            regSize2  = this->operands[1].getSize();

  /* Create the SMT semantic */
  op1 << ap.buildSymbolicRegOperand(reg1, regSize1);
  op2 << ap.buildSymbolicRegOperand(reg2, regSize2);

  /* Destination[0..63] = Source[64..127] */
  expr << smt2lib::concat(
            smt2lib::extract(127, 64, op1.str()), /* Destination[64..127] unchanged */
            smt2lib::extract(127, 64, op2.str())  /* Destination[0..63] = Source[64..127]; */
          );

  /* Create the symbolic element */
  se = ap.createRegSE(inst, expr, reg1, regSize1);

  /* Apply the taint */
  ap.assignmentSpreadTaintRegReg(se, reg1, reg2);
}


void MovhlpsIRBuilder::regMem(AnalysisProcessor &ap, Inst &inst) const {
  TwoOperandsTemplate::stop(this->disas);
}


void MovhlpsIRBuilder::memImm(AnalysisProcessor &ap, Inst &inst) const {
  TwoOperandsTemplate::stop(this->disas);
}


void MovhlpsIRBuilder::memReg(AnalysisProcessor &ap, Inst &inst) const {
  TwoOperandsTemplate::stop(this->disas);
}


Inst *MovhlpsIRBuilder::process(AnalysisProcessor &ap) const {
  checkSetup();

  Inst *inst = new Inst(ap.getThreadID(), this->address, this->disas);

  try {
    this->templateMethod(ap, *inst, this->operands, "MOVHLPS");
    ap.incNumberOfExpressions(inst->numberOfElements()); /* Used for statistics */
    ControlFlow::rip(*inst, ap, this->nextAddress);
  }
  catch (std::exception &e) {
    delete inst;
    throw;
  }

  return inst;
}

