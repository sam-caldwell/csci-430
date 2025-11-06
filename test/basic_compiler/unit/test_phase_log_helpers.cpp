// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include "basic_compiler/compiler/PhaseLogHelpers.h"

using namespace gwbasic::phase_log_helpers;
using gwbasic::Program;
using gwbasic::Line;

static std::filesystem::path mktempdir(const char* name) {
    auto d = std::filesystem::temp_directory_path() / name;
    std::filesystem::create_directories(d);
    return d;
}

TEST(PhaseLogHelpers, ResolveAndCanonical) {
    auto d = mktempdir("gwb_phase_paths");
    auto parent = d / "p"; std::filesystem::create_directories(parent);
    auto baseFile = parent / "file.bas"; std::ofstream(baseFile.string()).put('\n');
    auto child = parent / "child.bas"; std::ofstream(child.string()).put('\n');
    auto out = resolvePath(baseFile.string(), "child.bas");
    ASSERT_EQ(out, canonicalPath(child.string()));
}

TEST(PhaseLogHelpers, ReplaceOrAppendLine) {
    Program p{}; Line l1{10,{}}; Line l2{10,{}}; Line l3{20,{}};
    replaceOrAppendLine(p, std::move(l1), false);
    ASSERT_EQ(p.lines.size(), 1u);
    replaceOrAppendLine(p, std::move(l2), true); // replaces line 10
    ASSERT_EQ(p.lines.size(), 1u);
    replaceOrAppendLine(p, std::move(l3), false);
    ASSERT_EQ(p.lines.size(), 2u);
}

TEST(PhaseLogHelpers, ParseFileNoLogs_HappySad) {
    auto d = mktempdir("gwb_phase_parse");
    auto f = d / "one.bas";
    std::ofstream ofs(f.string()); ofs << "10 PRINT 1\n20 END\n"; ofs.close();
    auto prog = parseFileNoLogs(f.string());
    ASSERT_FALSE(prog.lines.empty());
    // Sad path: missing file
    ASSERT_THROW(parseFileNoLogs((d/"missing.bas").string()), std::runtime_error);
}

TEST(PhaseLogHelpers, TokenizeRootWithLogs) {
    auto d = mktempdir("gwb_phase_root");
    auto f = d / "root.bas"; std::ofstream(f.string()) << "10 END\n";
    auto lex = (d/"lex.log").string(); auto syn=(d/"syn.log").string();
    std::string canon; int minLine=0; auto prog = tokenizeRootWithLogs(f.string(), lex, syn, canon, minLine);
    ASSERT_FALSE(prog.lines.empty());
    ASSERT_EQ(minLine, 10);
}

TEST(PhaseLogHelpers, AssignBaseAndRenumber) {
    std::unordered_map<std::string, std::pair<int,int>> imported;
    auto a = canonicalPath("/tmp/a.bas");
    auto b = canonicalPath("/tmp/b.bas");
    imported[a] = {0, 10};
    int baseSame = assignBase(a, a, imported);
    int baseOther = assignBase(b, a, imported);
    ASSERT_EQ(baseSame, 0);
    ASSERT_GT(baseOther, 0);
    Program p{}; p.lines.push_back(Line{100,{}});
    int minImp=0; renumberProgram(p, baseOther, minImp);
    ASSERT_GT(p.lines.front().number, 100);
}

TEST(PhaseLogHelpers, GenerateIRWithLogs) {
    // Minimal program to exercise semantics+codegen
    Program p{}; p.lines.push_back(Line{10,{}});
    auto ir = generateIRWithLogs(p, "", "");
    ASSERT_FALSE(ir.empty());
}

