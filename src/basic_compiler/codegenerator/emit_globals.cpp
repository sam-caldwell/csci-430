// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <sstream>
#include <format>

namespace gwbasic {

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
void CodeGenerator::emitGlobals(std::ostringstream& out) {
    out << "@.fmt_num = private unnamed_addr constant [4 x i8] c\"%f\\0A\\00\"\n"
        << "@.fmt_str = private unnamed_addr constant [4 x i8] c\"%s\\0A\\00\"\n"
        << "@.fmt_num_sp = private unnamed_addr constant [4 x i8] c\"%f\\20\\00\"\n" // "%f "
        << "@.fmt_str_sp = private unnamed_addr constant [4 x i8] c\"%s\\20\\00\"\n" // "%s "
        << "@.fmt_in = private unnamed_addr constant [4 x i8] c\"%lf\\00\"\n"
        << "@.mode_r = private unnamed_addr constant [2 x i8] c\"r\\00\"\n"
        << "@.mode_w = private unnamed_addr constant [2 x i8] c\"w\\00\"\n"
        << "@gwb_last_rnd = global double 0.0\n"; // RNG state: last random value for RND(0)
    for (const auto&[fst, snd] : strLiteralId_) {
        const std::string& s = fst;
        const int id = snd;
        std::string esc = escapeForIR(s);
        const size_t N = s.size() + 1;
        out << globalStringName(id)
            << " = private unnamed_addr constant [" << N << " x i8] c\""
            << esc << "\\00\"\n";
        {
            std::string msg;
            msg.reserve(32 + s.size());
            msg = std::format(R"(emitGlobals: literal @{} from StringExpr "{}")", globalStringName(id), s);
            log(msg);
        }
    }
    out << "\n";
}

} // namespace gwbasic
