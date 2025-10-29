// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <sstream>

namespace gwbasic {

void CodeGenerator::emitGlobals(std::ostringstream& out) {
    /*
     * Function: CodeGenerator::emitGlobals
     * Inputs:
     *  - out: IR output stream
     * Outputs:
     *  - void
     * Theory of operation:
     *  - Emits format strings and all discovered string literals as constant
     *    global arrays with unnamed_addr for efficient addressing.
     */
    out << "@.fmt_num = private unnamed_addr constant [4 x i8] c\"%f\\0A\\00\"\n";
    out << "@.fmt_str = private unnamed_addr constant [4 x i8] c\"%s\\0A\\00\"\n";
    out << "@.fmt_in = private unnamed_addr constant [4 x i8] c\"%lf\\00\"\n";
    for (const auto&[fst, snd] : strLiteralId_) {
        const std::string& s = fst;
        const int id = snd;
        std::string esc = escapeForIR(s);
        const size_t N = s.size() + 1;
        out << globalStringName(id) << " = private unnamed_addr constant [" << N << " x i8] c\"" << esc << "\\00\"\n";
        {
            std::string msg;
            msg.reserve(32 + s.size());
            msg = "emitGlobals: literal @";
            msg += globalStringName(id);
            msg += " from StringExpr \"";
            msg += s;
            msg += "\"";
            log(msg);
        }
    }
    out << "\n";
}

} // namespace gwbasic
