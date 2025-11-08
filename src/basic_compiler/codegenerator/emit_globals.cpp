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
    void CodeGenerator::emitGlobals(std::ostringstream &out) {
        // Printf format strings:
        //  - For the last item in a PRINT list, append a newline ("%f\n" / "%s\n").
        //  - For non-last items, do not append any extra spacing. Any spacing
        //    should come from the program's string literals, so tests can assert
        //    exact output (avoids unexpected double spaces).
        out << R"(@.fmt_num = private unnamed_addr constant [4 x i8] c"%f\0A\00")" << Symbols::LF
                << R"(@.fmt_str = private unnamed_addr constant [4 x i8] c"%s\0A\00")" << Symbols::LF
                << R"(@.fmt_num_sp = private unnamed_addr constant [4 x i8] c"%f\20\00")" << Symbols::LF
                // numeric non-last: add one space
                << R"(@.fmt_num_ns = private unnamed_addr constant [3 x i8] c"%f\00")" << Symbols::LF
                // numeric non-last: no suffix
                << R"(@.fmt_str_sp = private unnamed_addr constant [3 x i8] c"%s\00")" << Symbols::LF
                // string non-last: no extra space
                << R"(@.fmt_int = private unnamed_addr constant [5 x i8] c"%ld\0A\00")" << Symbols::LF
                << R"(@.fmt_int_sp = private unnamed_addr constant [5 x i8] c"%ld\20\00")" << Symbols::LF
                << R"(@.fmt_int_ns = private unnamed_addr constant [4 x i8] c"%ld\00")" << Symbols::LF
                // Padding helper for PRINT zones: printf("%.*s", n, spaces)
                << R"(@.fmt_pad = private unnamed_addr constant [5 x i8] c"%.*s\00")" << Symbols::LF
                << R"(@.spaces_14 = private unnamed_addr constant [15 x i8] c"              \00")" << Symbols::LF;
        // Dedicated empty string literal for runtime helpers (e.g., INKEY$ default)
        out << R"(@.str_empty = private unnamed_addr constant [1 x i8] c"\00")" << Symbols::LF;
        if (needsBreakMsg_) {
            out << R"(@.msg_break = private unnamed_addr constant [13 x i8] c"Break in %d\0A\00")" << Symbols::LF;
        }
        out << R"(@.fmt_in = private unnamed_addr constant [4 x i8] c"%lf\00")" << Symbols::LF
                // scanf format to read an entire line into a buffer, discarding the trailing newline
                << R"(@.fmt_line_in = private unnamed_addr constant [12 x i8] c"%255[^\0A]%*c\00")" << Symbols::LF
                // strftime formats for DATE$/TIME$
                << R"(@.fmt_date = private unnamed_addr constant [9 x i8] c"%m-%d-%y\00")" << Symbols::LF
                << R"(@.fmt_time = private unnamed_addr constant [9 x i8] c"%H:%M:%S\00")" << Symbols::LF
                << R"(@.mode_r = private unnamed_addr constant [2 x i8] c"r\00")" << Symbols::LF
                << R"(@.mode_w = private unnamed_addr constant [2 x i8] c"w\00")" << Symbols::LF
                << R"(@.mode_rb = private unnamed_addr constant [3 x i8] c"rb\00")" << Symbols::LF
                << R"(@.mode_wb = private unnamed_addr constant [3 x i8] c"wb\00")" << Symbols::LF
                << R"(@.call_msg = private unnamed_addr constant [8 x i8] c"CALLED\0A\00")" << Symbols::LF
                << R"(@.str_eq = private unnamed_addr constant [2 x i8] c"=\00")" << Symbols::LF
                << "@gwb_last_rnd = global float 0.0" << Symbols::LF; // RNG state (single precision) for RND(0)
        for (const auto &[fst, snd]: strLiteralId_) {
            const std::string &s = fst;
            const int id = snd;
            std::string esc = escapeForIR(s);
            const size_t N = s.size() + 1;
            out << globalStringName(id)
                    << " = private unnamed_addr constant [" << N << " x i8] c\""
                    << esc << R"(\00")" << Symbols::LF;
            // Log discovery of string literals by streaming directly
            log() << "emitGlobals: literal " << globalStringName(id) << " from StringExpr \"" << s << "\"" <<
                    Symbols::LF;
        }
        out << Symbols::LF;
        // SGR printf format for COLOR statement: "%c[%dm\0"; emit as bytes
        out << "@.fmt_sgr = private unnamed_addr constant "
                << "[7 x i8] [i8 37, i8 99, i8 91, i8 37, i8 100, i8 109, i8 0]" << Symbols::LF
                // PC palette to ANSI SGR mapping (foreground)
                << "@.sgr_fg_tbl = private unnamed_addr constant "
                << "[16 x i32] [i32 30, i32 34, i32 32, i32 36, i32 31, i32 35, i32 33, i32 37, i32 90, i32 94, "
                << "i32 92, i32 96, i32 91, i32 95, i32 93, i32 97]"
                << Symbols::LF
                // PC palette to ANSI SGR mapping (background)
                << "@.sgr_bg_tbl = private unnamed_addr constant [16 x i32] [i32 40, i32 44, i32 42, i32 46, i32 41, "
                << "i32 45, i32 43, i32 47, i32 100, i32 104, i32 102, i32 106, i32 101, i32 105, i32 103, i32 107]"
                << Symbols::LF << Symbols::LF;
        // Graphics environment variables and readiness flag
        out << R"(@.env_display = private unnamed_addr constant [8 x i8] c"DISPLAY\00")" << Symbols::LF
                << R"(@.env_wayland = private unnamed_addr constant [16 x i8] c"WAYLAND_DISPLAY\00")" << Symbols::LF
                << "@gwb_gfx_ready = global i1 false" << Symbols::LF << Symbols::LF;
        // Device names for WIDTH comparisons
        out << R"(@.str_scrn = private unnamed_addr constant [6 x i8] c"SCRN:\00")" << Symbols::LF
                << R"(@.str_lpt1 = private unnamed_addr constant [6 x i8] c"LPT1:\00")" << Symbols::LF;
        // Bell character for BEEP
        out << R"(@.bell = private unnamed_addr constant [2 x i8] c"\07\00")" << Symbols::LF;
        // Helper strings for FILES implementation
        out << R"(@.str_ls1sp = private unnamed_addr constant [7 x i8] c"ls -1 \00")" << Symbols::LF
                << R"(@.str_dot = private unnamed_addr constant [2 x i8] c".\00")" << Symbols::LF;
        // Virtual screen state for SCREEN(row,col[,z])
        // - 80x25 character buffer, row-major, 0-based indices internally
        // - current cursor position used by PRINT mirroring logic
        out << "@gwb_screen = internal global [2000 x i8] zeroinitializer" << Symbols::LF
                << "@gwb_cur_row = global i32 0" << Symbols::LF
                << "@gwb_cur_col = global i32 0" << Symbols::LF
                // Current screen width (columns) and printer width (columns)
                << "@gwb_screen_cols = global i32 80" << Symbols::LF
                << "@gwb_printer_cols = global i32 80" << Symbols::LF
                // Shared formatting scratch buffer for mirroring printf output to screen
                << "@gwb_sbuf = internal global [256 x i8] zeroinitializer" << Symbols::LF << Symbols::LF;
        // Error handling/trap globals
        out << "@gwb_err_trap_line = global i32 0" << Symbols::LF
                << "@gwb_err_code = global i32 0" << Symbols::LF
                << "@gwb_err_line = global i32 0" << Symbols::LF
                << "@gwb_resume_line = global i32 0" << Symbols::LF
                << "@gwb_resume_stmt = global i32 0" << Symbols::LF
                << "@gwb_in_handler = global i1 false" << Symbols::LF << Symbols::LF;
        // Emit DATA/READ backing store if present (array of pointers to literals),
        // a parallel kind and numeric value tables, and an index
        {
            // Always provide an index variable; table may be size 0
            out << "@gwb_data_idx = global i32 0" << Symbols::LF;
            const size_t N = dataLiteralIds_.size();
            out << "@gwb_data = internal constant [" << N << " x ptr] [";
            for (size_t i = 0; i < N; ++i) {
                if (i) out << ", ";
                out << "ptr " << globalStringName(dataLiteralIds_[i]);
            }
            out << "]" << Symbols::LF;
            // is-string marker table (i8 1 when original item was quoted string)
            out << "@gwb_data_isstr = internal constant [" << N << " x i8] [";
            for (size_t i = 0; i < N; ++i) {
                if (i) out << ", ";
                out << "i8 " << static_cast<int>(dataIsString_[i]);
            }
            out << "]" << Symbols::LF;
            // numeric value table (double) for numeric items; undefined for strings (0.0)
            out << "@gwb_data_num = internal constant [" << N << " x double] [";
            for (size_t i = 0; i < N; ++i) {
                if (i) out << ", ";
                // Emit as floating literal with exponent to satisfy LLVM parser (e.g., 0.000000e+00)
                out << "double " << std::format("{:.6e}", dataNumValues_[i]);
            }
            out << "]" << Symbols::LF << Symbols::LF;
        }

        // Emulated memory and current segment
        out << "@gwb_mem = internal global [1048576 x i8] zeroinitializer" << Symbols::LF
                << "@gwb_seg = global i32 0" << Symbols::LF << Symbols::LF;
    }
} // namespace gwbasic
