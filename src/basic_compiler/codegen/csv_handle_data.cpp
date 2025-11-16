// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/DataStmt.h"

namespace gwbasic {

/*
 * Function: csvHandleData
 * Summary: Collect literal ids from DATA items to seed the string table.
 * Parameters:
 *  - dataStmt: Parsed DataStmt node.
 * Returns:
 *  - void
 */
void CodeGenerator::csvHandleData(const DataStmt* dataStmt) {
    for (const auto& [isString, text] : dataStmt->items) {
        (void)isString;
        if (!strLiteralId_.contains(text)) { strLiteralId_[text] = strCounter_++; }
        dataLiteralIds_.push_back(strLiteralId_[text]);
    }
    
}

} // namespace gwbasic
