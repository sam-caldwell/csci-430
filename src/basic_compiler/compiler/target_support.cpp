// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/TargetUtils.h"

bool isSupportedTargetTriple(const std::string& triple) {
    std::string t = triple;
    for (auto& c : t) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    // Arch: x86_64 or arm64 only
    const bool archOK = (t.find("x86_64") != std::string::npos) ||
                        (t.find("arm64") != std::string::npos);
    // OS: linux, darwin
    const bool osOK = (t.find("linux") != std::string::npos) ||
                      (t.find("darwin") != std::string::npos);
    return archOK && osOK;
}

