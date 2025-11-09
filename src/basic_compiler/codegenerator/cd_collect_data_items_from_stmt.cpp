// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/DataStmt.h"

namespace gwbasic {

/*
 * Function: cdCollectDataItemsFromStmt
 * Summary: Flatten a single DATA statement's items into internal tables.
 * Parameters:
 *  - dataStmt: Parsed DataStmt node (by const reference).
 * Returns:
 *  - void
 */
// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void CodeGenerator::cdCollectDataItemsFromStmt(const DataStmt& dataStmt) {
    for (const auto& [isString, text] : dataStmt.items) {
        const std::string& txt = text;
        if (!strLiteralId_.contains(txt)) {
            strLiteralId_[txt] = strCounter_++;
        }
        dataLiteralIds_.push_back(strLiteralId_[txt]);
        dataIsString_.push_back(isString ? 1U : 0U);
        dataNumValues_.push_back(isString ? 0.0 : (std::stod(txt)));
    }
}

} // namespace gwbasic
