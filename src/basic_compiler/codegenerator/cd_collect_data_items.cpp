// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/DataStmt.h"

namespace gwbasic {

/*
 * Function: CodeGenerator::cdCollectDataItems
 * Purpose: Flatten DATA items into tables and assign literal ids.
 */
void CodeGenerator::cdCollectDataItems(const std::vector<int>& lines) {
    dataLiteralIds_.clear(); dataIsString_.clear(); dataNumValues_.clear();
    for (int ln : lines) {
        const auto* lptr = lineMap_[ln]; if (!lptr) continue;
        for (const auto& st : lptr->statements) {
            if (const auto ds = dyn_cast<const DataStmt>(st.get())) {
                for (const auto& it : ds->items) {
                    const std::string& txt = it.text;
                    if (!strLiteralId_.contains(txt)) strLiteralId_[txt] = strCounter_++;
                    dataLiteralIds_.push_back(strLiteralId_[txt]);
                    dataIsString_.push_back(it.isString ? 1u : 0u);
                    dataNumValues_.push_back(it.isString ? 0.0 : (std::stod(txt)));
                }
            }
        }
    }
}

} // namespace gwbasic

