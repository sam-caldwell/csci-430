// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <filesystem>

namespace gwbasic::phase_log_helpers {


static std::filesystem::path mktempdir_plh_parse(const char* name) {
    auto d = std::filesystem::temp_directory_path() / name;
    std::filesystem::create_directories(d);
    return d;
}

} //end namespace
