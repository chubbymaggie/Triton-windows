#include <algorithm>
#include <sstream>
#include <stdexcept>

#include <CmovpIRBuilder.h>
#include <Registers.h>
#include <SMT2Lib.h>
#include <SymbolicElement.h>


CmovpIRBuilder::CmovpIRBuilder(uint64 address, const std::string &disassembly):
  BaseIRBuilder(address, disassembly){
}


void CmovpIRBuilder::regImm(AnalysisProcessor &ap, Inst &inst) const {
  TwoOperandsTemplate::stop(this->disas);
}


void CmovpIRBuilder::regReg(AnalysisProcessor &ap, Inst &inst) const {
  SymbolicElement   *se;
  std::stringstream expr, reg1e, reg2e, pf;
  uint64            reg1    = this->operands[0].getValue();
  uint64            reg2    = this->operands[1].getValue();
  uint64            size1   = this->operands[0].getSize();
  uint64            size2   = this->operands[1].getSize();

  /* Create the SMT semantic */
  pf << ap.buildSymbolicFlagOperand(ID_PF);
  reg1e << ap.buildSymbolicRegOperand(reg1, size1);
  reg2e << ap.buildSymbolicRegOperand(reg2, size2);

  expr << smt2lib::ite(
            smt2lib::equal(
              pf.str(),
              smt2lib::bvtrue()),
            reg2e.str(),
            reg1e.str());

  /* Create the symbolic element */
  se = ap.createRegSE(inst, expr, reg1, size1);

  /* Apply the taint via the concretization */
  if (ap.getFlagValue(ID_PF) == 1)
    ap.assignmentSpreadTaintRegReg(se, reg1, reg2);

}


void CmovpIRBuilder::regMem(AnalysisProcessor &ap, Inst &inst) const {
  SymbolicElement   *se;
  std::stringstream expr, reg1e, mem1e, pf;
  uint32            readSize = this->operands[1].getSize();
  uint64            mem      = this->operands[1].getValue();
  uint64            reg      = this->operands[0].getValue();
  uint64            regSize  = this->operands[0].getSize();

  /* Create the SMT semantic */
  pf << ap.buildSymbolicFlagOperand(ID_PF);
  reg1e << ap.buildSymbolicRegOperand(reg, regSize);
  mem1e << ap.buildSymbolicMemOperand(mem, readSize);

  expr << smt2lib::ite(
            smt2lib::equal(
              pf.str(),
              smt2lib::bvtrue()),
            mem1e.str(),
            reg1e.str());

  /* Create the symbolic element */
  se = ap.createRegSE(inst, expr, reg, regSize);

  /* Apply the taint via the concretization */
  if (ap.getFlagValue(ID_PF) == 1)
    ap.assignmentSpreadTaintRegMem(se, reg, mem, readSize);

}


void CmovpIRBuilder::memImm(AnalysisProcessor &ap, Inst &inst) const {
  TwoOperandsTemplate::stop(this->disas);
}


void CmovpIRBuilder::memReg(AnalysisProcessor &ap, Inst &inst) const {
  TwoOperandsTemplate::stop(this->disas);
}


Inst *CmovpIRBuilder::process(AnalysisProcessor &ap) const {
  checkSetup();

  Inst *inst = new Inst(ap.getThreadID(), this->address, this->disas);

  try {
    this->templateMethod(ap, *inst, this->operands, "CMOVP");
    ap.incNumberOfExpressions(inst->numberOfElements()); /* Used for statistics */
    ControlFlow::rip(*inst, ap, this->nextAddress);
  }
  catch (std::exception &e) {
    delete inst;
    throw;
  }

  return inst;
}

