// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"

namespace gwbasic {

/*
 * Function: csvHandleData
 * Summary: Collect literal ids from DATA items to seed the string table.
 * Parameters:
 *  - ds: Parsed DataStmt node.
 * Returns:
 *  - void
 */
void CodeGenerator::csvHandleData(const DataStmt* ds) {
    for (const auto& [isString, text] : ds->items) {
        (void)isString;
        if (!strLiteralId_.contains(text)) strLiteralId_[text] = strCounter_++;
        dataLiteralIds_.push_back(strLiteralId_[text]);
    }
    logSem() << "Data items=" << ds->items.size() << Symbols::LF;
}

} // namespace gwbasic
