// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/WithTripleHeader.h"
#include <sstream>

/*
 * Function: withTripleHeader
 * Inputs:
 *  - ir: LLVM IR text to inspect
 *  - triple: Target triple string to inject if missing
 * Outputs:
 *  - std::string: IR with a leading target triple line when absent
 * Theory of operation:
 *  - If 'triple' is non-empty and the IR does not already contain a
 *    'target triple =' header, prepend one, otherwise return IR unchanged.
 */
std::string withTripleHeader(const std::string& ir, const std::string& triple) {
    if (triple.empty()) return ir;
    if (ir.find("target triple =") != std::string::npos) return ir;
    std::ostringstream out;
    out << "target triple = \"" << triple << "\"\n\n" << ir;
    return out.str();
}
