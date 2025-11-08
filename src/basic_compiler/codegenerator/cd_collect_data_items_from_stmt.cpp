// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/DataStmt.h"

namespace gwbasic {

/*
 * Function: CodeGenerator::cdCollectDataItemsFromStmt
 * Purpose: Flatten a single DATA statement's items into tables.
 */
void CodeGenerator::cdCollectDataItemsFromStmt(const DataStmt& ds) {
    for (const auto& it : ds.items) {
        const std::string& txt = it.text;
        if (!strLiteralId_.contains(txt)) strLiteralId_[txt] = strCounter_++;
        dataLiteralIds_.push_back(strLiteralId_[txt]);
        dataIsString_.push_back(it.isString ? 1u : 0u);
        dataNumValues_.push_back(it.isString ? 0.0 : (std::stod(txt)));
    }
}

} // namespace gwbasic

