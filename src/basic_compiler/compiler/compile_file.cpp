// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Compiler.h"
#include <fstream>
#include <sstream>

namespace gwbasic {

std::string Compiler::compileFile(const std::string& path) {
    /*
     * Function: Compiler::compileFile
     * Inputs:
     *  - path: filesystem path to GW-BASIC source file
     * Outputs:
     *  - std::string: LLVM IR text of the compiled program
     * Theory of operation:
     *  - Reads file contents into memory, delegates to compileString(), which
     *    lexes, parses, and generates IR for the program.
     */
    std::ifstream in(path);
    if (!in) throw std::runtime_error(std::string("Unable to open input file: ").append(path));
    std::ostringstream buf;
    buf << in.rdbuf();
    return compileString(buf.str());
}

} // namespace gwbasic
