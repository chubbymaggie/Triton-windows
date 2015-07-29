#include <iostream>
#include <sstream>
#include <stdexcept>

#include <RcrIRBuilder.h>
#include <Registers.h>
#include <SMT2Lib.h>
#include <SymbolicElement.h>


RcrIRBuilder::RcrIRBuilder(uint64 address, const std::string &disassembly):
  BaseIRBuilder(address, disassembly) {
}


void RcrIRBuilder::regImm(AnalysisProcessor &ap, Inst &inst) const {
  SymbolicElement   *se1, *se2;
  std::stringstream expr, op1, op2, cf, res;
  uint64            reg     = this->operands[0].getValue();
  uint64            imm     = this->operands[1].getValue();
  uint32            regSize = this->operands[0].getSize();

  /* Create the SMT semantic */
  cf << ap.buildSymbolicFlagOperand(ID_CF);
  op1 << ap.buildSymbolicRegOperand(reg, regSize);
  /*
   * Note that SMT2-LIB doesn't support expression as rotate's value.
   * The op2 must be the concretization's value.
   */
  op2 << imm;

  /* Rcl expression */
  expr << smt2lib::bvror(
            smt2lib::concat(cf.str(), op1.str()),
            op2.str()
          );

  /* Temporary extended expression */
  se1 = ap.createSE(inst, expr, "Temporary Extended Expression");

  /* Apply the taint */
  ap.assignmentSpreadTaintExprReg(se1, reg);

  /* Result expression */
  res << smt2lib::extract(regSize, expr.str());

  /* Create the symbolic element for the result */
  se2 = ap.createRegSE(inst, res, reg, regSize);

  /* Apply the taint */
  ap.aluSpreadTaintRegReg(se2, reg, reg);

  /* Add the symbolic flags element to the current inst */
  EflagsBuilder::cfRcl(inst, se1, ap, regSize, op2); /* Same as RCL */
  EflagsBuilder::ofRor(inst, se2, ap, regSize, op2); /* Same as ROR */
}


void RcrIRBuilder::regReg(AnalysisProcessor &ap, Inst &inst) const {
  SymbolicElement   *se1, *se2;
  std::stringstream expr, op1, op2, cf, res;
  uint64            reg1     = this->operands[0].getValue();
  uint32            regSize1 = this->operands[0].getSize();

  /* Create the SMT semantic */
  cf << ap.buildSymbolicFlagOperand(ID_CF);
  op1 << ap.buildSymbolicRegOperand(reg1, regSize1);
  /*
   * Note that SMT2-LIB doesn't support expression as rotate's value.
   * The op2 must be the concretization's value.
   */
  op2 << (ap.getRegisterValue(ID_RCX) & 0xff); /* 0xff -> There is only CL available */

  /* Rcl expression */
  expr << smt2lib::bvror(
            smt2lib::concat(cf.str(), op1.str()),
            op2.str()
          );

  /* Temporary extended expression */
  se1 = ap.createSE(inst, expr, "Temporary Extended Expression");

  /* Apply the taint */
  ap.assignmentSpreadTaintExprReg(se1, reg1);

  /* Result expression */
  res << smt2lib::extract(regSize1, expr.str());

  /* Create the symbolic element */
  se2 = ap.createRegSE(inst, res, reg1, regSize1);

  /* Apply the taint */
  ap.aluSpreadTaintRegReg(se2, reg1, reg1);

  /* Add the symbolic flags element to the current inst */
  EflagsBuilder::cfRcl(inst, se1, ap, regSize1, op2); /* Same as RCL */
  EflagsBuilder::ofRor(inst, se2, ap, regSize1, op2); /* Same as ROR */
}


void RcrIRBuilder::regMem(AnalysisProcessor &ap, Inst &inst) const {
  TwoOperandsTemplate::stop(this->disas);
}


void RcrIRBuilder::memImm(AnalysisProcessor &ap, Inst &inst) const {
  SymbolicElement   *se1, *se2;
  std::stringstream expr, op1, op2, cf, res;
  uint32            writeSize = this->operands[0].getSize();
  uint64            mem       = this->operands[0].getValue();
  uint64            imm       = this->operands[1].getValue();

  /* Create the SMT semantic */
  cf << ap.buildSymbolicFlagOperand(ID_CF);
  op1 << ap.buildSymbolicMemOperand(mem, writeSize);
  /*
   * Note that SMT2-LIB doesn't support expression as rotate's value.
   * The op2 must be the concretization's value.
   */
  op2 << imm;

  /* Rcl expression */
  expr << smt2lib::bvror(
            smt2lib::concat(cf.str(), op1.str()),
            op2.str()
          );

  /* Temporary extended expression */
  se1 = ap.createSE(inst, expr, "Temporary Extended Expression");

  /* Apply the taint */
  ap.assignmentSpreadTaintExprMem(se1, mem, writeSize);

  /* Result expression */
  res << smt2lib::extract(writeSize, expr.str());

  /* Create the symbolic element */
  se2 = ap.createMemSE(inst, res, mem, writeSize);

  /* Apply the taint */
  ap.aluSpreadTaintMemMem(se2, mem, mem, writeSize);

  /* Add the symbolic flags element to the current inst */
  EflagsBuilder::cfRcl(inst, se1, ap, writeSize, op2); /* Same as RCL */
  EflagsBuilder::ofRor(inst, se2, ap, writeSize, op2); /* Same as ROR */
}


void RcrIRBuilder::memReg(AnalysisProcessor &ap, Inst &inst) const {
  SymbolicElement   *se1, *se2;
  std::stringstream expr, op1, op2, cf, res;
  uint32            writeSize = this->operands[0].getSize();
  uint64            mem       = this->operands[0].getValue();

  /* Create the SMT semantic */
  cf << ap.buildSymbolicFlagOperand(ID_CF);
  op1 << ap.buildSymbolicMemOperand(mem, writeSize);
  /*
   * Note that SMT2-LIB doesn't support expression as rotate's value.
   * The op2 must be the concretization's value.
   */
  op2 << (ap.getRegisterValue(ID_RCX) & 0xff); /* 0xff -> There is only CL available */

  /* Rcl expression */
  expr << smt2lib::bvror(
            smt2lib::concat(cf.str(), op1.str()),
            op2.str()
          );

  /* Temporary extended expression */
  se1 = ap.createSE(inst, expr, "Temporary Extended Expression");

  /* Apply the taint */
  ap.assignmentSpreadTaintExprMem(se1, mem, writeSize);

  /* Result expression */
  res << smt2lib::extract(writeSize, expr.str());

  /* Create the symbolic element */
  se2 = ap.createMemSE(inst, res, mem, writeSize);

  /* Apply the taint */
  ap.aluSpreadTaintMemMem(se2, mem, mem, writeSize);

  /* Add the symbolic flags element to the current inst */
  EflagsBuilder::cfRcl(inst, se1, ap, writeSize, op2); /* Same as RCL */
  EflagsBuilder::ofRor(inst, se2, ap, writeSize, op2); /* Same as ROR */
}


Inst *RcrIRBuilder::process(AnalysisProcessor &ap) const {
  this->checkSetup();

  Inst *inst = new Inst(ap.getThreadID(), this->address, this->disas);

  try {
    this->templateMethod(ap, *inst, this->operands, "RCR");
    ap.incNumberOfExpressions(inst->numberOfElements()); /* Used for statistics */
    ControlFlow::rip(*inst, ap, this->nextAddress);
  }
  catch (std::exception &e) {
    delete inst;
    throw;
  }

  return inst;
}

