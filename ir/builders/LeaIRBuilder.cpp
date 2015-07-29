#include <iostream>
#include <sstream>
#include <stdexcept>

#include <LeaIRBuilder.h>
#include <Registers.h>
#include <SMT2Lib.h>
#include <SymbolicElement.h>


LeaIRBuilder::LeaIRBuilder(uint64 address, const std::string &disassembly):
  BaseIRBuilder(address, disassembly) {
}


void LeaIRBuilder::regImm(AnalysisProcessor &ap, Inst &inst) const {
  TwoOperandsTemplate::stop(this->disas);
}


void LeaIRBuilder::regReg(AnalysisProcessor &ap, Inst &inst) const {
  TwoOperandsTemplate::stop(this->disas);
}


void LeaIRBuilder::regMem(AnalysisProcessor &ap, Inst &inst) const {
  SymbolicElement   *se;
  std::stringstream expr, reg1e, dis2e, base2e, index2e, scale2e;
  uint64            reg           = this->operands[0].getValue();
  uint64            regSize       = this->operands[0].getSize();
  uint64            displacement  = this->operands[1].getDisplacement();
  uint64            baseReg       = this->operands[1].getBaseReg();
  uint64            indexReg      = this->operands[1].getIndexReg();
  uint64            memoryScale   = this->operands[1].getMemoryScale();

  /* Base register */
  if (baseReg)
    base2e << ap.buildSymbolicRegOperand(baseReg, regSize);
  else
    base2e << smt2lib::bv(0, regSize * REG_SIZE);

  /* Index register if it exists */
  if (indexReg)
    index2e << ap.buildSymbolicRegOperand(indexReg, regSize);
  else
    index2e << smt2lib::bv(0, regSize * REG_SIZE);

  /* Displacement */
  dis2e << smt2lib::bv(displacement, regSize * REG_SIZE);

  /* Scale */
  scale2e << smt2lib::bv(memoryScale, regSize * REG_SIZE);

  /* final SMT expression */
  /* Effective address = Displacement + BaseReg + IndexReg * Scale */
  expr << smt2lib::bvadd(dis2e.str(), smt2lib::bvadd(base2e.str(), smt2lib::bvmul(index2e.str(), scale2e.str())));

  /* Create the symbolic element */
  se = ap.createRegSE(inst, expr, reg, regSize);

  /* Apply the taint via the concretization */
  if (ap.isRegTainted(baseReg) == TAINTED)
    ap.assignmentSpreadTaintRegReg(se, reg, baseReg);
  else
    ap.assignmentSpreadTaintRegReg(se, reg, indexReg);

}


void LeaIRBuilder::memImm(AnalysisProcessor &ap, Inst &inst) const {
  TwoOperandsTemplate::stop(this->disas);
}


void LeaIRBuilder::memReg(AnalysisProcessor &ap, Inst &inst) const {
  TwoOperandsTemplate::stop(this->disas);
}


Inst *LeaIRBuilder::process(AnalysisProcessor &ap) const {
  this->checkSetup();

  Inst *inst = new Inst(ap.getThreadID(), this->address, this->disas);

  try {
    this->templateMethod(ap, *inst, this->operands, "LEA");
    ap.incNumberOfExpressions(inst->numberOfElements()); /* Used for statistics */
    ControlFlow::rip(*inst, ap, this->nextAddress);
  }
  catch (std::exception &e) {
    delete inst;
    throw;
  }

  return inst;
}

