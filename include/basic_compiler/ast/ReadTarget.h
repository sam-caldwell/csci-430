// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <string>
#include <memory>
#include <vector>
#include "basic_compiler/ast/Expr.h"

namespace gwbasic {

/**
 * Type: ReadTarget
 * Purpose:
 *  - Identify a READ destination (scalar var or array element).
 * Inputs:
 *  - name: Variable or array name
 *  - index: Optional index expression when targeting an array element
 * Outputs:
 *  - Aggregate used within ReadStmt.targets
 */
struct ReadTarget {
    std::string name;                 // variable or array name
    std::vector<std::unique_ptr<Expr>> indices; // optional array indices (empty for scalar)
};

} // namespace gwbasic
