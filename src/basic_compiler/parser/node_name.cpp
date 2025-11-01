// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"

namespace gwbasic {

const char* Parser::nodeName(const Stmt* s) {
    return prettyName(s ? s->getKind() : NodeKind::AbstractStmt);
}

} // namespace gwbasic

