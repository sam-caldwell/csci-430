// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"

namespace gwbasic {

void Parser::logSyntax(const std::string& msg) {
    if (syntaxLogEnabled_ && syntaxLog_.is_open()) syntaxLog_ << msg << '\n';
}

} // namespace gwbasic

