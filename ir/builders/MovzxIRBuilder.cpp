#include <algorithm>
#include <sstream>
#include <stdexcept>

#include <MovzxIRBuilder.h>
#include <Registers.h>
#include <SMT2Lib.h>
#include <SymbolicElement.h>



MovzxIRBuilder::MovzxIRBuilder(uint64 address, const std::string &disassembly):
  BaseIRBuilder(address, disassembly) {
}


void MovzxIRBuilder::regImm(AnalysisProcessor &ap, Inst &inst) const {
  TwoOperandsTemplate::stop(this->disas);
}


void MovzxIRBuilder::regReg(AnalysisProcessor &ap, Inst &inst) const {
  SymbolicElement   *se;
  std::stringstream expr, op1;
  uint64            reg1  = this->operands[0].getValue();
  uint64            reg2  = this->operands[1].getValue();
  uint64            size1 = this->operands[0].getSize();
  uint64            size2 = this->operands[1].getSize();

  /* Create the SMT semantic */
  op1 << ap.buildSymbolicRegOperand(reg2, size2);

  /* Final expr */
  expr << smt2lib::zx(op1.str(), (size1 * REG_SIZE) - (size2 * REG_SIZE));

  /* Create the symbolic element */
  se = ap.createRegSE(inst, expr, reg1, size1);

  /* Apply the taint */
  ap.assignmentSpreadTaintRegReg(se, reg1, reg2);
}


void MovzxIRBuilder::regMem(AnalysisProcessor &ap, Inst &inst) const {
  SymbolicElement   *se;
  std::stringstream expr, op1;
  uint32            readSize = this->operands[1].getSize();
  uint64            mem      = this->operands[1].getValue();
  uint64            reg      = this->operands[0].getValue();
  uint64            regSize  = this->operands[0].getSize();

  /* Create the SMT semantic */
  op1 << ap.buildSymbolicMemOperand(mem, readSize);

  /* Final expr */
  expr << smt2lib::zx(op1.str(), (regSize * REG_SIZE) - (readSize * REG_SIZE));

  /* Create the symbolic element */
  se = ap.createRegSE(inst, expr, reg, regSize);

  /* Apply the taint */
  ap.assignmentSpreadTaintRegMem(se, reg, mem, readSize);
}


void MovzxIRBuilder::memImm(AnalysisProcessor &ap, Inst &inst) const {
  TwoOperandsTemplate::stop(this->disas);
}


void MovzxIRBuilder::memReg(AnalysisProcessor &ap, Inst &inst) const {
  TwoOperandsTemplate::stop(this->disas);
}


Inst *MovzxIRBuilder::process(AnalysisProcessor &ap) const {
  checkSetup();

  Inst *inst = new Inst(ap.getThreadID(), this->address, this->disas);

  try {
    this->templateMethod(ap, *inst, this->operands, "MOVZX");
    ap.incNumberOfExpressions(inst->numberOfElements()); /* Used for statistics */
    ControlFlow::rip(*inst, ap, this->nextAddress);
  }
  catch (std::exception &e) {
    delete inst;
    throw;
  }

  return inst;
}

