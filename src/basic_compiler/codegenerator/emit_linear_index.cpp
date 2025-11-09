// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include <format>
#include <sstream>
#include <vector>

namespace gwbasic {

std::string CodeGenerator::emitLinearIndex(std::ostringstream& out,
                                           const std::vector<std::string>& idxI64s,
                                           const std::vector<int>& dims) {
    // Compute strides on host
    std::vector<long long> extents;
    extents.reserve(dims.size());
    for (size_t di = 0; di < dims.size(); ++di) {
        long long e = static_cast<long long>(dims[di]) - optionBase_ + 1;
        if (e < 0) {
            e = 0;
        }
        extents.push_back(e);
    }
    std::vector<long long> strides(dims.size(), 1);
    for (int di = static_cast<int>(dims.size()) - 2; di >= 0; --di) {
        strides[di] = strides[di + 1] * extents[di + 1];
    }
    // Adjust each index by base
    std::vector<std::string> adjs;
    adjs.reserve(idxI64s.size());
    for (const auto& ii : idxI64s) {
        std::string a = nextTemp();
        out << std::format("  {} = sub i64 {}, {}", a, ii, optionBase_) << Symbols::LF;
        adjs.push_back(a);
    }
    // Multiply-accumulate
    std::string lin = nextTemp();
    out << std::format("  {} = mul i64 {}, {}", lin, adjs[0], strides[0]) << Symbols::LF;
    for (size_t di = 1; di < adjs.size(); ++di) {
        std::string t = nextTemp();
        out << std::format("  {} = mul i64 {}, {}", t, adjs[di], strides[di]) << Symbols::LF;
        std::string s2 = nextTemp();
        out << std::format("  {} = add i64 {}, {}", s2, lin, t) << Symbols::LF;
        lin = s2;
    }
    return lin;
}

} // namespace gwbasic

