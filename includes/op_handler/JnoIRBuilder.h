#ifndef JNOIRBUILDER_H
#define JNOIRBUILDER_H

#include "BaseIRBuilder.h"
#include "EflagsBuilder.h"
#include "Inst.h"
#include "OneOperandTemplate.h"


class JnoIRBuilder: public BaseIRBuilder, public OneOperandTemplate {

  public:
    JnoIRBuilder(uint64 address, const std::string &disassembly);

    // From BaseIRBuilder
    virtual Inst *process(AnalysisProcessor &ap) const;

    // From OneOperandTemplate
    virtual void none(AnalysisProcessor &ap, Inst &inst) const;

    virtual void reg(AnalysisProcessor &ap, Inst &inst) const;

    virtual void imm(AnalysisProcessor &ap, Inst &inst) const;

    virtual void mem(AnalysisProcessor &ap, Inst &inst) const;
};

#endif // JNOIRBUILDER_H
