// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include <format>

namespace gwbasic {

std::string CodeGenerator::loadFileHandleAtFixed(std::ostringstream &out, const long long index) {
    std::string fptr = nextTemp();
    out << std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", fptr, index)
        << Symbols::LF;
    std::string fh = nextTemp();
    out << std::format("  {} = load ptr, ptr {}", fh, fptr) << Symbols::LF;
    return fh;
}

} // namespace gwbasic

