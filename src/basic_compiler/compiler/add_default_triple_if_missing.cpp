// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/compiler/Compiler.h"

#include <array>
#include <cstdio>
#include <sstream>
#include <string>

namespace gwbasic {

// NOLINTNEXTLINE(readability-function-size)
std::string Compiler::addDefaultTripleIfMissing(const std::string& ir_text) {
    if (ir_text.find("target triple =") != std::string::npos) {
        return ir_text;
    }
    std::string triple;
    const auto* const cmd = "clang -### -S -x ir - -o /dev/null 2>&1";
    if (FILE* pipe_file = popen(cmd, "r")) {
        std::array<char, 256> buf{}; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
        std::string out;
        while (const size_t read_count = fread(buf.data(), 1, buf.size(), pipe_file)) {
            out.append(buf.data(), read_count);
        }
        pclose(pipe_file);
        if (const auto pos = out.find("\"-triple\""); pos != std::string::npos) {
            if (const auto first_quote = out.find('"', pos + 9); first_quote != std::string::npos) {
                if (const auto second_quote = out.find('"', first_quote + 1);
                    second_quote != std::string::npos && second_quote > first_quote + 1) {
                    triple = out.substr(first_quote + 1, second_quote - (first_quote + 1));
                }
            }
        }
    }
    if (triple.empty()) {
        return ir_text;
    }
    std::ostringstream out;
    out << "target triple = \"" << triple << "\"\n\n" << ir_text;
    return out.str();
}

} // namespace gwbasic

