// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"

namespace gwbasic {

/* Collect variables for READ targets (scalars or arrays). */
void CodeGenerator::csvHandleRead(const ReadStmt* rd) {
    for (const auto& t : rd->targets) {
        if (!t.indices.empty())
            for (const auto& ix : t.indices)
                collectExprVars(ix.get());
        else variables_.insert(t.name);
    }
}

} // namespace gwbasic

