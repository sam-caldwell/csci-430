// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"

namespace gwbasic {

void Parser::setSyntaxLogPath(const std::string& path) {
    if (syntaxLog_.is_open()) syntaxLog_.close();
    syntaxLog_.open(path, std::ios::out | std::ios::trunc);
    syntaxLogEnabled_ = syntaxLog_.is_open();
}

} // namespace gwbasic

