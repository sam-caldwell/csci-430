// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/cli/TakeOptValues.h"
#include "basic_compiler/cli/TakeOptValue.h"

namespace gwbasic::cli {

bool takeOptValue(const std::string& arg,
                  std::initializer_list<const char*> names,
                  int& i,
                  const int argc,
                  char** argv,
                  std::optional<std::string>& out) {
    for (const auto name : names) {
        if (takeOptValue(arg, name, i, argc, argv, out)) return true;
    }
    return false;
}

} // namespace gwbasic::cli

