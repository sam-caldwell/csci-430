// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <ranges>

namespace gwbasic {

/*
 * Function: lookupBinding
 * Summary: Lookup a temporary binding for a given variable name.
 * Parameters:
 *  - name: Variable identifier to lookup.
 *  - out: Output string to receive the bound SSA name.
 * Returns:
 *  - bool: True if a binding was found and set in out.
 */
bool CodeGenerator::lookupBinding(const std::string& name, std::string& out) const {
    for (const auto& it : std::ranges::reverse_view(bindingStack_)) {
        auto f = it.find(name);
        if (f != it.end()) {
            out = f->second;
            return true;
        }
    }
    return false;
}

} // namespace gwbasic
