// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/semantics/SemanticAnalyzer.h"
#include <sstream>

namespace gwbasic {

void SemanticAnalyzer::reference(const std::string& name, const SourcePos& pos) {
    if (!isDeclared(name)) {
        declare(name);
        std::ostringstream m; m << "VarImplicitDecl " << name << " @ " << pos.line << ':' << pos.col; log(m.str());
    } else {
        std::ostringstream m; m << "VarRef " << name << " @ " << pos.line << ':' << pos.col; log(m.str());
    }
}

} // namespace gwbasic

