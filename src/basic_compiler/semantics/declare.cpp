// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/semantics/SemanticAnalyzer.h"
#include <sstream>

namespace gwbasic {

void SemanticAnalyzer::declare(const std::string& name) {
    // BASIC variables are global; record declarations in the global scope
    if (scopes_.empty()) scopes_.emplace_back();
    scopes_.front().insert(name);
    vars_.insert(name);
    std::ostringstream m; m << "VarDecl " << name; log(m.str());
}

} // namespace gwbasic
