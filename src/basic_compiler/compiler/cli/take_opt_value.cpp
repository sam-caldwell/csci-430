// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/cli/TakeOptValue.h"

namespace gwbasic::cli {

bool takeOptValue(const std::string& arg,
                  const char* name,
                  int& i,
                  int argc,
                  char** argv,
                  std::optional<std::string>& out) {
    if (arg == name) {
        if (i + 1 < argc) {
            out = argv[++i];
            return true;
        }
        // Missing value; do not consume. Let caller handle error/reporting.
    }
    return false;
}

} // namespace gwbasic::cli

