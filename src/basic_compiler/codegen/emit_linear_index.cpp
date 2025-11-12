// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include <algorithm>
#include <cstddef>
#include <format>
#include <sstream>
#include <string>
#include <vector>

namespace gwbasic {

/*
 * Function: emitLinearIndex
 * Summary: Compute linearized array index from multi-dimensional indices.
 * Parameters:
 *  - out: IR output stream to append to.
 *  - idxI64s: SSA names of each index (i64), in order.
 *  - dims: Declared upper bounds for each dimension.
 * Returns:
 *  - std::string: SSA register name holding the linear index (i64).
 */
std::string CodeGenerator::emitLinearIndex(std::ostringstream& out,
                                           const std::vector<std::string>& idxI64s,
                                           const std::vector<int>& dims) {
    // Compute strides on host
    std::vector<long long> extents;
    extents.reserve(dims.size());
    for (size_t dimIndex = 0; dimIndex < dims.size(); ++dimIndex) {
        long long extentVal = static_cast<long long>(dims[dimIndex]) - optionBase_ + 1;
        extentVal = std::max<long long>(extentVal, 0);
        extents.push_back(extentVal);
    }
    std::vector<long long> strides(dims.size(), 1);
    for (int di = static_cast<int>(dims.size()) - 2; di >= 0; --di) {
        strides[di] = strides[di + 1] * extents[di + 1];
    }
    // Adjust each index by base
    std::vector<std::string> adjusted;
    adjusted.reserve(idxI64s.size());
    for (const auto& idxReg : idxI64s) {
        std::string adj = nextTemp();
        out << std::format("  {} = sub i64 {}, {}", adj, idxReg, optionBase_) << Symbols::LF;
        adjusted.push_back(adj);
    }
    // Multiply-accumulate
    std::string linearIdx = nextTemp();
    out << std::format("  {} = mul i64 {}, {}", linearIdx, adjusted[0], strides[0]) << Symbols::LF;
    for (size_t dimIndex = 1; dimIndex < adjusted.size(); ++dimIndex) {
        std::string prod = nextTemp();
        out << std::format("  {} = mul i64 {}, {}", prod, adjusted[dimIndex], strides[dimIndex]) << Symbols::LF;
        std::string sum = nextTemp();
        out << std::format("  {} = add i64 {}, {}", sum, linearIdx, prod) << Symbols::LF;
        linearIdx = sum;
    }
    return linearIdx;
}

} // namespace gwbasic
