// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

#include "basic_compiler/Compiler.h"
#include "../../helper/clang_path.h"
#include "../../helper/run_command.h"
#include "../../helper/tool_exists.h"
#include "../../helper/source_root.h"
#include "../../helper/parse_row.h"

using namespace gwbasic;
using namespace e2e_helpers;

/***
 * Test: E2E.TrigDemo_MathAccuracy
 * Purpose: Compile demos/trig.bas end-to-end and verify printed trig values
 *          for selected angles against math library within a small epsilon.
 */
TEST(E2E, TrigDemo_MathAccuracy) {
    if (!toolExists(CLANG_PATH)) { GTEST_SKIP() << "clang not found"; }

    // Compile program to IR
    const std::string srcPath = sourceRoot() + "/demos/trig.bas";
    std::string ir = Compiler::compileFile(srcPath.c_str());

    // Materialize a binary
    std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_trig";
    std::filesystem::create_directories(tmp);
    auto ll = tmp / "program.ll"; auto bin = tmp / "program.out";
    { std::ofstream f(ll); f << ir; }
    std::ostringstream cmd; cmd << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    cmd << " -lm";
#endif
    ASSERT_EQ(std::system(cmd.str().c_str()), 0);

    // Run and capture output
    std::string out = runCommand(std::string("\"") + bin.string() + "\"");
    ASSERT_FALSE(out.empty());

    // Split lines
    std::istringstream iss(out);
    std::string line;
    std::vector<Row> rows;
    while (std::getline(iss, line)) {
        // Skip header line(s)
        if (line.find("SIN(X)") != std::string::npos) continue;
        Row r{};
        if (parseRow(line, r)) rows.push_back(r);
    }
    ASSERT_GE(rows.size(), 361u) << "Expected 0..360 rows";

    // Index rows by integer degree for quick lookup
    std::vector<Row> byDeg(361);
    for (const auto& r : rows) {
        int deg = static_cast<int>(std::llround(r.x));
        if (deg >= 0 && deg <= 360) byDeg[deg] = r;
    }

    const double PI = std::acos(-1.0);
    auto deg2rad = [PI](double d) { return d * PI / 180.0; };
    auto almost = [](double a, double b, double eps=1e-5) { return std::fabs(a - b) <= eps; };

    // Validate selected degrees; avoid tan at 90/270 where undefined.
    const int samples[] = {0, 30, 45, 60, 120, 135, 150, 180, 360};
    for (int d : samples) {
        const Row& r = byDeg[d];
        const double rad = deg2rad(d);
        const double es = std::sin(rad);
        const double ec = std::cos(rad);
        const double et = std::tan(rad);
        ASSERT_TRUE(almost(r.s, es)) << "sin(" << d << ") expected=" << es << " got=" << r.s;
        ASSERT_TRUE(almost(r.c, ec)) << "cos(" << d << ") expected=" << ec << " got=" << r.c;
        // Only check tan when |cos| not ~0 to avoid INF
        if (std::fabs(ec) > 1e-12) {
            ASSERT_TRUE(almost(r.t, et)) << "tan(" << d << ") expected=" << et << " got=" << r.t;
        }
    }
}
