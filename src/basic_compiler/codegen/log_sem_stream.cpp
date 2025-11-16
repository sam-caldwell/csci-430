// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <ostream>
#include <streambuf>

namespace gwbasic {

/*
 * Function: logSem
 * Summary: Access the semantic analysis log stream.
 * Parameters:
 *  - (none)
 * Returns:
 *  - std::ostream&: Stream for semantic logging.
 */
std::ostream& CodeGenerator::logSem() {
    struct NullBuf : public std::streambuf { int overflow(int c) override { return traits_type::not_eof(c); } };
    static NullBuf nb;
    static std::ostream os(&nb);
    return os;
}

} // namespace gwbasic
