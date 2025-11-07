// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <gtest/gtest.h>
#include <sstream>
#include <string>

#include "../../helper/clang_path.h"

namespace e2e_helpers {

    struct Row { double x; double s; double c; double t; };
    /***
     * Parse a single data line of the form:
     * "<x> <sin> <cos> <tan>" (numbers with 6 decimals)
     * Returns true if four doubles were parsed.
     */
    inline bool parseRow(const std::string& line, Row& out) {
        if (std::istringstream iss(line); !(iss >> out.x >> out.s >> out.c >> out.t)) return false;
        return true;
    }

}
