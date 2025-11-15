// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/Line.h"
#include "basic_compiler/Symbols.h"
#include <format>
#include <sstream>

namespace gwbasic {

void CodeGenerator::emitLineErrorHandlerSkip(std::ostringstream &out,
                                             const Line &line,
                                             int &localCounter) {
    const auto itSkip = handlerSkipAfter_.find(line.number);
    if (itSkip == handlerSkipAfter_.end()) {
        return;
    }
    const std::string tl = nextTemp();
    out << std::format("  {} = load i32, ptr @gwb_err_trap_line", tl) << Symbols::LF;
    const std::string isThis = nextTemp();
    out << std::format("  {} = icmp eq i32 {}, {}", isThis, tl, line.number) << Symbols::LF;
    const std::string contLbl = std::format("{}_hdlr_cont_{}", lineLabelName(line.number), ++localCounter);
    const std::string chkLbl = std::format("{}_hdlr_chk_{}", lineLabelName(line.number), localCounter);
    out << std::format("  br i1 {}, label %{}, label %{}", isThis, chkLbl, contLbl) << Symbols::LF;
    out << chkLbl << ":" << Symbols::LF;
    const std::string ih = nextTemp();
    out << std::format("  {} = load i1, ptr @gwb_in_handler", ih) << Symbols::LF;
    const std::string notIH = nextTemp();
    out << std::format("  {} = icmp eq i1 {}, false", notIH, ih) << Symbols::LF;
    const std::string skipLbl = std::format("{}_hdlr_skip_{}", lineLabelName(line.number), localCounter);
    out << std::format("  br i1 {}, label %{}, label %{}", notIH, skipLbl, contLbl) << Symbols::LF;
    out << skipLbl << ":" << Symbols::LF;
    if (itSkip->second >= 0) {
        out << std::format("  br label %{}", lineLabelName(itSkip->second)) << Symbols::LF;
    } else {
        out << "  br label %exit" << Symbols::LF;
    }
    out << contLbl << ":" << Symbols::LF;
}

} // namespace gwbasic

