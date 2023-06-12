
#pragma once

#include <unordered_set>
#include <cstdint>

namespace ppc2cpp {
extern const std::unordered_set<int32_t> imm_operands;
bool isImmediate(uint64_t operandFlags, int64_t operandValue);

extern const std::unordered_set<int32_t> input_operands;
extern const std::unordered_set<int32_t> output_operands;
}