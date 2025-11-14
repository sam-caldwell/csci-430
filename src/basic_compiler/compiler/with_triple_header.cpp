// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/WithTripleHeader.h"

#include <sstream>
#include <string>

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
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
std::string withTripleHeader(const std::string& irText, const std::string& triple) {
    if (triple.empty()) { return irText; }
    if (irText.find("target triple =") != std::string::npos) { return irText; }
    std::ostringstream out;
    out << "target triple = \"" << triple << "\"\n\n" << irText;
    return out.str();
}
