// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Compiler.h"
#include <fstream>
#include <sstream>

namespace gwbasic {

std::string Compiler::compileFileWithLog(const std::string& path, const std::string& logPath) {
    /*
     * Function: Compiler::compileFileWithLog
     * Inputs:
     *  - path: Filesystem path to a GW-BASIC source file
     *  - logPath: Path for the code generation log output
     * Outputs:
     *  - std::string: LLVM IR text for the compiled program
     * Theory of operation:
     *  - Loads the file contents into memory and delegates to
     *    compileStringWithLog() so the same lex/parse/codegen pipeline and
     *    logging behavior are used for both file and string inputs.
     */
    std::ifstream in(path);
    if (!in) throw std::runtime_error(std::string("Unable to open input file: ").append(path));
    std::ostringstream buf;
    buf << in.rdbuf();
    return compileStringWithLog(buf.str(), logPath);
}

} // namespace gwbasic
