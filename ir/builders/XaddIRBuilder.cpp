#include <iostream>
#include <sstream>
#include <stdexcept>

#include <XaddIRBuilder.h>
#include <Registers.h>
#include <SMT2Lib.h>
#include <SymbolicElement.h>


XaddIRBuilder::XaddIRBuilder(uint64 address, const std::string &disassembly):
  BaseIRBuilder(address, disassembly) {
}


void XaddIRBuilder::regImm(AnalysisProcessor &ap, Inst &inst) const {
  TwoOperandsTemplate::stop(this->disas);
}


void XaddIRBuilder::regReg(AnalysisProcessor &ap, Inst &inst) const {
  SymbolicElement   *se1, *se2;
  std::stringstream expr1, expr2, op1, op2;
  uint64            reg1          = this->operands[0].getValue();
  uint64            reg2          = this->operands[1].getValue();
  uint32            regSize1      = this->operands[0].getSize();
  uint32            regSize2      = this->operands[1].getSize();
  uint64            tmpReg1Taint  = ap.isRegTainted(reg1);
  uint64            tmpReg2Taint  = ap.isRegTainted(reg2);

  /* Create the SMT semantic */
  op1 << ap.buildSymbolicRegOperand(reg1, regSize1);
  op2 << ap.buildSymbolicRegOperand(reg2, regSize2);

  // Final expr
  expr1 << op2.str();
  expr2 << smt2lib::bvadd(op1.str(), op2.str());

  /* Create the symbolic element */
  se1 = ap.createRegSE(inst, expr1, reg1, regSize1);
  se2 = ap.createRegSE(inst, expr2, reg2, regSize2);

  /* Apply the taint */
  ap.setTaintReg(se1, reg1, tmpReg2Taint);
  ap.setTaintReg(se2, reg2, tmpReg1Taint);

  /* Add the symbolic flags element to the current inst */
  EflagsBuilder::af(inst, se2, ap, regSize2, op1, op2);
  EflagsBuilder::cfAdd(inst, se2, ap, op1);
  EflagsBuilder::ofAdd(inst, se2, ap, regSize2, op1, op2);
  EflagsBuilder::pf(inst, se2, ap);
  EflagsBuilder::sf(inst, se2, ap, regSize2);
  EflagsBuilder::zf(inst, se2, ap, regSize2);
}


void XaddIRBuilder::regMem(AnalysisProcessor &ap, Inst &inst) const {
  TwoOperandsTemplate::stop(this->disas);
}


void XaddIRBuilder::memImm(AnalysisProcessor &ap, Inst &inst) const {
  TwoOperandsTemplate::stop(this->disas);
}


void XaddIRBuilder::memReg(AnalysisProcessor &ap, Inst &inst) const {
  SymbolicElement   *se1, *se2;
  std::stringstream expr1, expr2, op1, op2;
  uint64            mem1          = this->operands[0].getValue();
  uint64            reg2          = this->operands[1].getValue();
  uint32            memSize1      = this->operands[0].getSize();
  uint32            regSize2      = this->operands[1].getSize();
  uint64            tmpMem1Taint  = ap.isMemTainted(mem1);
  uint64            tmpReg2Taint  = ap.isRegTainted(reg2);

  /* Create the SMT semantic */
  op1 << ap.buildSymbolicMemOperand(mem1, memSize1);
  op2 << ap.buildSymbolicRegOperand(reg2, regSize2);

  // Final expr
  expr1 << op2.str();
  expr2 << smt2lib::bvadd(op1.str(), op2.str());

  /* Create the symbolic element */
  se1 = ap.createMemSE(inst, expr1, mem1, memSize1);
  se2 = ap.createRegSE(inst, expr2, reg2, regSize2);

  /* Apply the taint */
  ap.setTaintMem(se1, mem1, tmpReg2Taint);
  ap.setTaintReg(se2, reg2, tmpMem1Taint);

  /* Add the symbolic flags element to the current inst */
  EflagsBuilder::af(inst, se2, ap, memSize1, op1, op2);
  EflagsBuilder::cfAdd(inst, se2, ap, op1);
  EflagsBuilder::ofAdd(inst, se2, ap, memSize1, op1, op2);
  EflagsBuilder::pf(inst, se2, ap);
  EflagsBuilder::sf(inst, se2, ap, memSize1);
  EflagsBuilder::zf(inst, se2, ap, memSize1);
}


Inst *XaddIRBuilder::process(AnalysisProcessor &ap) const {
  this->checkSetup();

  Inst *inst = new Inst(ap.getThreadID(), this->address, this->disas);

  try {
    this->templateMethod(ap, *inst, this->operands, "XADD");
    ap.incNumberOfExpressions(inst->numberOfElements()); /* Used for statistics */
    ControlFlow::rip(*inst, ap, this->nextAddress);
  }
  catch (std::exception &e) {
    delete inst;
    throw;
  }

  return inst;
}

