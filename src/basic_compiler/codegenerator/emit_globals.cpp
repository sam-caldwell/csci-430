// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <sstream>

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
    out << "@.fmt_num = private unnamed_addr constant [4 x i8] c\"%f\\0A\\00\"" << STR_LF
        << "@.fmt_str = private unnamed_addr constant [4 x i8] c\"%s\\0A\\00\"" << STR_LF
        << "@.fmt_num_sp = private unnamed_addr constant [4 x i8] c\"%f\\20\\00\"" << STR_LF // "%f "
        << "@.fmt_str_sp = private unnamed_addr constant [4 x i8] c\"%s\\20\\00\"" << STR_LF // "%s "
        << "@.fmt_in = private unnamed_addr constant [4 x i8] c\"%lf\\00\"" << STR_LF
        << "@.mode_r = private unnamed_addr constant [2 x i8] c\"r\\00\"" << STR_LF
        << "@.mode_w = private unnamed_addr constant [2 x i8] c\"w\\00\"" << STR_LF
        << "@.mode_rb = private unnamed_addr constant [3 x i8] c\"rb\\00\"" << STR_LF
        << "@.mode_wb = private unnamed_addr constant [3 x i8] c\"wb\\00\"" << STR_LF
        << "@.call_msg = private unnamed_addr constant [8 x i8] c\"CALLED\\0A\\00\"" << STR_LF
        << "@gwb_last_rnd = global double 0.0" << STR_LF; // RNG state: last random value for RND(0)
    for (const auto&[fst, snd] : strLiteralId_) {
        const std::string& s = fst;
        const int id = snd;
        std::string esc = escapeForIR(s);
        const size_t N = s.size() + 1;
        out << globalStringName(id)
            << " = private unnamed_addr constant [" << N << " x i8] c\""
            << esc << "\\00\"" << STR_LF;
        // Log discovery of string literals by streaming directly
        log() << "emitGlobals: literal " << globalStringName(id) << " from StringExpr \"" << s << "\"" << CH_LF;
    }
    out << STR_LF;
    // Emit DATA/READ backing store if present (array of pointers to literals) and an index
    {
        // Always provide an index variable; table may be size 0
        out << "@gwb_data_idx = global i32 0" << STR_LF;
        const size_t N = dataLiteralIds_.size();
        out << "@gwb_data = internal constant [" << N << " x ptr] [";
        for (size_t i = 0; i < N; ++i) {
            if (i) out << ", ";
            out << "ptr " << globalStringName(dataLiteralIds_[i]);
        }
        out << "]" << STR_LF << STR_LF;
    }

    // Emulated memory and current segment
    out << "@gwb_mem = internal global [1048576 x i8] zeroinitializer" << STR_LF
        << "@gwb_seg = global i32 0" << STR_LF << STR_LF;
}

} // namespace gwbasic
