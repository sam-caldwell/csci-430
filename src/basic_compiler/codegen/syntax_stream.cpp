// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <ostream>
#include <streambuf>

namespace gwbasic {

/*
 * Function: syntax
 * Summary: Access the syntax log stream used for debug tracing.
 * Parameters:
 *  - (none)
 * Returns:
 *  - std::ostream&: Stream for syntax logging.
 */
std::ostream& CodeGenerator::syntax() {
    struct NullBuf : public std::streambuf { int overflow(int c) override { return traits_type::not_eof(c); } };
    static NullBuf nb;
    static std::ostream os(&nb);
    return os;
}

} // namespace gwbasic
