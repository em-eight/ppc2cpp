
#include <fmt/format.h>

#include "ppc2cpp/control_flow/ControlFlowAnalysis.hpp"
#include "ppc2cpp/control_flow/ControlFlowGraph.hpp"
#include "ppc2cpp/common/endian.h"
#include "ppc2cpp/common/insn_properties.h"

#include "ppcdisasm/ppc-dis.hpp"
#include "ppcdisasm/ppc-operands.h"

using namespace ppc2cpp;

void ControlFlowAnalysis::functionCFA(Function& func) {
  func.cfg.init();
  functionCFA(func, 0);
}

void ControlFlowAnalysis::functionCFA(Function& func, uint32_t blockIdx) {
  uint32_t* funcPtr = (uint32_t*) programLoader->getBufferAtLocation(func.location).value();
  for (uint32_t pc = func.cfg.blocks[blockIdx].start; pc < func.length(); pc++) {
    const uint32_t insn = be32(*(funcPtr + pc));

    if (isBranchUncoditional(insn)) {
      int32_t branchVal = ppcdisasm::operand_value_powerpc(LI, insn, ppcdisasm::ppc_750cl_dialect);
      handleBranch(insn, blockIdx, func, pc, branchVal);

      if (isLink(insn)) { // control flow returns back after function call
        assert(pc+1 < func.length() && "Branch with link cannot be the last instruction of a function (need to blr at the end)");
        int32_t maybeNewBlock = func.cfg.tryCreateSuccessorInternal(blockIdx, pc, pc + 1);
        if (maybeNewBlock >= 0) functionCFA(func, maybeNewBlock);
        func.cfg.blocks[blockIdx].endsInCall = true;
      } else if (pc+1 == func.length() && func.cfg.hasExternalSuccessor(blockIdx)) { // tail call
        assert(isBranchConditionUnconditional(insn) && "Tail call with conditional branch? wtf");
        func.cfg.blocks[blockIdx].endsInCall = true;
        func.cfg.blocks[blockIdx].endsInTailCall = true;
        func.cfg.blocks[blockIdx].isExit = true;
      }
      break;
    } else if (isBranchConditional(insn)) {
      int32_t branchVal = ppcdisasm::operand_value_powerpc(BD, insn, ppcdisasm::ppc_750cl_dialect);
      handleBranch(insn, blockIdx, func, pc, branchVal);

      if (isLink(insn) || !isBranchConditionUnconditional(insn)) {
        assert(pc+1 < func.length() && "Branch with link cannot be the last instruction of a function (need to blr at the end)");
        int32_t maybeNewBlock = func.cfg.tryCreateSuccessorInternal(blockIdx, pc, pc + 1);
        if (maybeNewBlock >= 0) functionCFA(func, maybeNewBlock);
        if (isLink(insn)) func.cfg.blocks[blockIdx].endsInCall = true;
      } else if (!isLink(insn) && pc+1 == func.length() && func.cfg.hasExternalSuccessor(blockIdx)) { // cursed tail call
        assert(isBranchConditionUnconditional(insn) && "Tail call with conditional branch? wtf");
        func.cfg.blocks[blockIdx].endsInCall = true;
        func.cfg.blocks[blockIdx].endsInTailCall = true;
        func.cfg.blocks[blockIdx].isExit = true;
      }
      break;
    } else if (isBranchToLR(insn) || isBranchToCTR(insn)) {
      func.cfg.createSuccessorRuntime(blockIdx, pc);

      if (isLink(insn) || !isBranchConditionUnconditional(insn)) {
        assert(pc+1 < func.length() && "Branch with link cannot be the last instruction of a function (need to blr at the end)");
        int32_t maybeNewBlock = func.cfg.tryCreateSuccessorInternal(blockIdx, pc, pc + 1);
        if (maybeNewBlock >= 0) functionCFA(func, maybeNewBlock);
        if (isLink(insn)) func.cfg.blocks[blockIdx].endsInCall = true;
      } else if (isBranchToLR(insn) && !isLink(insn) && isBranchConditionUnconditional(insn)) { // return
        func.cfg.blocks[blockIdx].isExit = true;
      } else if (!isLink(insn) && isBranchToCTR(insn) && pc+1 == func.length() && func.cfg.hasExternalSuccessor(blockIdx)) { // curseder tail call
        assert(isBranchConditionUnconditional(insn) && "Tail call with conditional branch? wtf");
        func.cfg.blocks[blockIdx].endsInCall = true;
        func.cfg.blocks[blockIdx].endsInTailCall = true;
        func.cfg.blocks[blockIdx].isExit = true;
      }
      break;
    }
    // If the following instruction is contained in an existing internal basic block, end current block here and fall through to that
    int32_t maybeFollowingBlockIdx = func.cfg.findInternalBlockByPosition(pc + 1);
    if (pc+1 < func.length() && maybeFollowingBlockIdx >= 0) {
      func.cfg.createSuccessorFallthrough(blockIdx, pc, maybeFollowingBlockIdx);
      break;
    }
  }
}

void ControlFlowAnalysis::handleBranch(uint32_t insn, uint32_t blockIdx, Function& func, uint32_t pc, int32_t branchVal) {
  bool isBranchAbsolute = isAbsolute(insn);
  ProgramLocation targetLocation = programLoader->resolveTarget(func.location + 4*pc, branchVal, isBranchAbsolute).value();
  if (func.contains(targetLocation)) {
    uint32_t position = (targetLocation.section_offset - func.location.section_offset)/4;
    int32_t maybeNewBlock = func.cfg.tryCreateSuccessorInternal(blockIdx, pc, position);
    if (maybeNewBlock >= 0) functionCFA(func, maybeNewBlock);
  } else {
    func.cfg.createSuccessorExternal(blockIdx, pc);
  }
}