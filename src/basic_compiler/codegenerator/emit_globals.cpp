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
    // SGR printf format for COLOR statement: "%c[%dm\0"; emit as bytes
    out << "@.fmt_sgr = private unnamed_addr constant [7 x i8] [i8 37, i8 99, i8 91, i8 37, i8 100, i8 109, i8 0]" << STR_LF
        // PC palette to ANSI SGR mapping (foreground)
        << "@.sgr_fg_tbl = private unnamed_addr constant [16 x i32] [i32 30, i32 34, i32 32, i32 36, i32 31, i32 35, i32 33, i32 37, i32 90, i32 94, i32 92, i32 96, i32 91, i32 95, i32 93, i32 97]" << STR_LF
        // PC palette to ANSI SGR mapping (background)
        << "@.sgr_bg_tbl = private unnamed_addr constant [16 x i32] [i32 40, i32 44, i32 42, i32 46, i32 41, i32 45, i32 43, i32 47, i32 100, i32 104, i32 102, i32 106, i32 101, i32 105, i32 103, i32 107]" << STR_LF << STR_LF;
    // Graphics environment variables and readiness flag
    out << "@.env_display = private unnamed_addr constant [8 x i8] c\"DISPLAY\\00\"" << STR_LF
        << "@.env_wayland = private unnamed_addr constant [15 x i8] c\"WAYLAND_DISPLAY\\00\"" << STR_LF
        << "@.env_enable_gfx = private unnamed_addr constant [18 x i8] c\"GWBASIC_ENABLE_GFX\\00\"" << STR_LF
        << "@.env_disable_gfx = private unnamed_addr constant [19 x i8] c\"GWBASIC_DISABLE_GFX\\00\"" << STR_LF
        << "@gwb_gfx_ready = global i1 false" << STR_LF << STR_LF;
    // Virtual screen state for SCREEN(row,col[,z])
    // - 80x25 character buffer, row-major, 0-based indices internally
    // - current cursor position used by PRINT mirroring logic
    out << "@gwb_screen = internal global [2000 x i8] zeroinitializer" << STR_LF
        << "@gwb_cur_row = global i32 0" << STR_LF
        << "@gwb_cur_col = global i32 0" << STR_LF
        // Shared formatting scratch buffer for mirroring printf output to screen
        << "@gwb_sbuf = internal global [256 x i8] zeroinitializer" << STR_LF << STR_LF;
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
