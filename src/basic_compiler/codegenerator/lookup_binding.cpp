// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <ranges>

namespace gwbasic {

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

