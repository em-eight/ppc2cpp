
#include "ppcdisasm/ppc-forms.h"
#include "ppcdisasm/ppc-operands.h"

#include "ppc2cpp/data_flow/PpcInstructionUD.hpp"

using namespace ppcdisasm;

namespace ppc2cpp {
// ---- Instruction set partitioning -----
// The following sets partition the supported PowerPC instruction set
// PowerPC fixed point unit instruction
const std::unordered_set<int32_t> fixedPointInsn {
 PPC_FORM_ADD ,
 PPC_FORM_ADD_ ,
 PPC_FORM_ADDC ,
 PPC_FORM_ADDC_ ,
 PPC_FORM_ADDCO ,
 PPC_FORM_ADDCO_ ,
 PPC_FORM_ADDE ,
 PPC_FORM_ADDE_ ,
 PPC_FORM_ADDEO ,
 PPC_FORM_ADDEO_ ,
 PPC_FORM_ADDI ,
 PPC_FORM_ADDIC ,
 PPC_FORM_ADDIC_ ,
 PPC_FORM_ADDIS ,
 PPC_FORM_ADDME ,
 PPC_FORM_ADDME_ ,
 PPC_FORM_ADDMEO ,
 PPC_FORM_ADDMEO_ ,
 PPC_FORM_ADDO ,
 PPC_FORM_ADDO_ ,
 PPC_FORM_ADDZE ,
 PPC_FORM_ADDZE_ ,
 PPC_FORM_ADDZEO ,
 PPC_FORM_ADDZEO_ ,
 PPC_FORM_AND ,
 PPC_FORM_AND_ ,
 PPC_FORM_ANDC ,
 PPC_FORM_ANDC_ ,
 PPC_FORM_ANDI_ ,
 PPC_FORM_ANDIS_, 

 PPC_FORM_DIVW, 
 PPC_FORM_DIVW_, 
 PPC_FORM_DIVWO, 
 PPC_FORM_DIVWO_, 
 PPC_FORM_DIVWU, 
 PPC_FORM_DIVWU_, 
 PPC_FORM_DIVWUO, 
 PPC_FORM_DIVWUO_, 

 PPC_FORM_MULHW, 
 PPC_FORM_MULHW_, 
 PPC_FORM_MULHWU, 
 PPC_FORM_MULHWU_, 
 PPC_FORM_MULLI, 
 PPC_FORM_MULLW, 
 PPC_FORM_MULLW_, 
 PPC_FORM_MULLWO, 
 PPC_FORM_MULLWO_, 

 PPC_FORM_NAND, 
 PPC_FORM_NAND_, 
 PPC_FORM_NEG, 
 PPC_FORM_NEG_, 
 PPC_FORM_NEGO, 
 PPC_FORM_NEGO_, 
 PPC_FORM_NOR, 
 PPC_FORM_NOR_, 

 PPC_FORM_OR, 
 PPC_FORM_OR_, 
 PPC_FORM_ORC, 
 PPC_FORM_ORC_, 
 PPC_FORM_ORI, 
 PPC_FORM_ORIS, 

 PPC_FORM_RLWIMI, 
 PPC_FORM_RLWIMI_, 
 PPC_FORM_RLWINM, 
 PPC_FORM_RLWINM_, 
 PPC_FORM_RLWNM, 
 PPC_FORM_RLWNM_, 

 PPC_FORM_SLW, 
 PPC_FORM_SLW_, 
 PPC_FORM_SRAW, 
 PPC_FORM_SRAW_, 
 PPC_FORM_SRAWI, 
 PPC_FORM_SRAWI_, 
 PPC_FORM_SRW, 
 PPC_FORM_SRW_, 

 PPC_FORM_SUBF, 
 PPC_FORM_SUBF_, 
 PPC_FORM_SUBFC, 
 PPC_FORM_SUBFC_, 
 PPC_FORM_SUBFCO, 
 PPC_FORM_SUBFCO_, 
 PPC_FORM_SUBFE, 
 PPC_FORM_SUBFE_, 
 PPC_FORM_SUBFEO, 
 PPC_FORM_SUBFEO_, 
 PPC_FORM_SUBFIC, 
 PPC_FORM_SUBFME, 
 PPC_FORM_SUBFME_, 
 PPC_FORM_SUBFMEO, 
 PPC_FORM_SUBFMEO_, 
 PPC_FORM_SUBFO, 
 PPC_FORM_SUBFO_, 
 PPC_FORM_SUBFZE, 
 PPC_FORM_SUBFZE_, 
 PPC_FORM_SUBFZEO, 
 PPC_FORM_SUBFZEO_, 

 PPC_FORM_XOR, 
 PPC_FORM_XOR_, 
 PPC_FORM_XORI, 
 PPC_FORM_XORIS, 
};

// PowerPC floating point unit instruction
const std::unordered_set<int32_t> floatInsn {
 PPC_FORM_FABS_, 
 PPC_FORM_FABS, 
 PPC_FORM_FADD_, 
 PPC_FORM_FADD, 
 PPC_FORM_FADDS_, 
 PPC_FORM_FADDS, 
 PPC_FORM_FCMPO, 
 PPC_FORM_FCMPU, 
 PPC_FORM_FCTIW_, 
 PPC_FORM_FCTIW, 
 PPC_FORM_FCTIWZ_, 
 PPC_FORM_FCTIWZ, 
 PPC_FORM_FDIV_, 
 PPC_FORM_FDIV, 
 PPC_FORM_FDIVS_, 
 PPC_FORM_FDIVS, 
 PPC_FORM_FMADD_, 
 PPC_FORM_FMADD, 
 PPC_FORM_FMADDS_, 
 PPC_FORM_FMADDS, 
 PPC_FORM_FMR_, 
 PPC_FORM_FMR, 
 PPC_FORM_FMSUB_, 
 PPC_FORM_FMSUB, 
 PPC_FORM_FMSUBS_, 
 PPC_FORM_FMSUBS, 
 PPC_FORM_FMUL_, 
 PPC_FORM_FMUL, 
 PPC_FORM_FMULS_, 
 PPC_FORM_FMULS, 
 PPC_FORM_FNABS_, 
 PPC_FORM_FNABS, 
 PPC_FORM_FNEG_, 
 PPC_FORM_FNEG, 
 PPC_FORM_FNMADD_, 
 PPC_FORM_FNMADD, 
 PPC_FORM_FNMADDS_, 
 PPC_FORM_FNMADDS, 
 PPC_FORM_FNMSUB_, 
 PPC_FORM_FNMSUB, 
 PPC_FORM_FNMSUBS_, 
 PPC_FORM_FNMSUBS, 
 PPC_FORM_FRES_, 
 PPC_FORM_FRES, 
 PPC_FORM_FRSP_, 
 PPC_FORM_FRSP, 
 PPC_FORM_FRSQRTE_, 
 PPC_FORM_FRSQRTE, 
 PPC_FORM_FSEL_, 
 PPC_FORM_FSEL, 
 PPC_FORM_FSQRT_, 
 PPC_FORM_FSQRT, 
 PPC_FORM_FSQRTS_, 
 PPC_FORM_FSQRTS, 
 PPC_FORM_FSUB_, 
 PPC_FORM_FSUB, 
 PPC_FORM_FSUBS_, 
 PPC_FORM_FSUBS, 
};

const std::unordered_set<int32_t> loadInsn {
 PPC_FORM_LBZ, 
 PPC_FORM_LBZU, 
 PPC_FORM_LBZUX, 
 PPC_FORM_LBZX, 
 PPC_FORM_LFD, 
 PPC_FORM_LFDU, 
 PPC_FORM_LFDUX, 
 PPC_FORM_LFDX, 
 PPC_FORM_LFS, 
 PPC_FORM_LFSU, 
 PPC_FORM_LFSUX, 
 PPC_FORM_LFSX, 
 PPC_FORM_LHA, 
 PPC_FORM_LHAU, 
 PPC_FORM_LHAUX, 
 PPC_FORM_LHAX, 
 PPC_FORM_LHBRX, 
 PPC_FORM_LHZ, 
 PPC_FORM_LHZU, 
 PPC_FORM_LHZUX, 
 PPC_FORM_LHZX, 
//#define PPC_FORM_LMW -1207959552
 PPC_FORM_LSWI, 
 PPC_FORM_LSWX, 
//#define PPC_FORM_LWARX 2080374824
 PPC_FORM_LWBRX, 
 PPC_FORM_LWZ, 
 PPC_FORM_LWZU, 
 PPC_FORM_LWZUX, 
 PPC_FORM_LWZX, 
};

const std::unordered_set<int32_t> storeInsn {
 PPC_FORM_STB, 
 PPC_FORM_STBU, 
 PPC_FORM_STBUX, 
 PPC_FORM_STBX, 
 PPC_FORM_STFD, 
 PPC_FORM_STFDU, 
 PPC_FORM_STFDUX, 
 PPC_FORM_STFDX, 
 PPC_FORM_STFIWX, 
 PPC_FORM_STFS, 
 PPC_FORM_STFSU, 
 PPC_FORM_STFSUX, 
 PPC_FORM_STFSX, 
 PPC_FORM_STH, 
 PPC_FORM_STHBRX, 
 PPC_FORM_STHU, 
 PPC_FORM_STHUX, 
 PPC_FORM_STHX, 
//#define PPC_FORM_STMW -1140850688
 PPC_FORM_STSWI, 
 PPC_FORM_STSWX, 
 PPC_FORM_STW, 
 PPC_FORM_STWBRX, 
//#define PPC_FORM_STWCX_ 2080375085
 PPC_FORM_STWU, 
 PPC_FORM_STWUX, 
 PPC_FORM_STWX, 
};
// ---- End of instruction set partitioning -----

// Other sets here (setsXER, setsFPSCR, setsCR etc)

std::unordered_set<int32_t> nonStoreInsn;
std::unordered_set<int32_t> supportedInsn;
void initInstructionUD() {
  // all supported instructions minus store instructions. The first operand is input while the rest are outputs.
  // The only exception is load with update instructions, where RAL is also an output.
  nonStoreInsn.insert(fixedPointInsn.begin(), fixedPointInsn.end());
  nonStoreInsn.insert(floatInsn.begin(), floatInsn.end());
  nonStoreInsn.insert(loadInsn.begin(), loadInsn.end());
  
  supportedInsn.insert(nonStoreInsn.begin(), nonStoreInsn.end());
  supportedInsn.insert(storeInsn.begin(), storeInsn.end());
}


InstructionUD instructionUD(uint32_t insn, const struct powerpc_opcode* opcode, ppc_cpu_t dialect) {
  if (!supportedInsn.contains(opcode->opcode)) {
    throw std::runtime_error("Unsupported instruction " + std::string(opcode->name));
  }

  InstructionUD insnUD;

  const ppc_opindex_t *opindex;
  const struct powerpc_operand *operand;

  int registerInputCount = 0;
  for (opindex = opcode->operands; *opindex != 0; opindex++) {
    uint32_t operand_idx = *opindex;
    operand = powerpc_operands + operand_idx;
    
    int64_t value = operand_value_powerpc (operand, insn, dialect);

    if ((operand->flags & supported_register_types) != 0) { // register input
      if (nonStoreInsn.contains(opcode->opcode) && registerInputCount == 0) {
        insnUD.outputs.push_back(operand_idx);
      } else {
        insnUD.inputs.push_back(operand_idx);
      }
      if (operand_idx == RAL || operand_idx == RAS) {
        insnUD.outputs.push_back(operand_idx);
      }
      // TODO: check setting of CR, XER and FPSCR (maybe PS1 too)
      registerInputCount++;
    } else if (isImmediate(operand->flags, value)) { // immediate input
      insnUD.inputs.push_back(operand_idx);
    } else {
      throw std::runtime_error("unsupported operand, instruction " + std::string(opcode->name) + " operand index " + std::to_string(operand_idx));
    }
  }

  return insnUD;
}
} // ns