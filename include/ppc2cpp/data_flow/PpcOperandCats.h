
#pragma once

#include <unordered_set>
#include <cstdint>

namespace ppc2cpp {
bool isImmediate(uint64_t operandFlags, int64_t operandValue);
bool isIgnore(uint64_t operandFlags);
}