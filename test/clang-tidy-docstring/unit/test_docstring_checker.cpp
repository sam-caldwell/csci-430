// (c) 2025 Sam Caldwell. All Rights Reserved.
#include <gtest/gtest.h>
#include "clang-tidy-docstring/DocstringChecker.h"

using namespace doccheck;

TEST(DocstringChecker, AcceptsWellFormedCFunction) {
    const char* src = R"C(
/*
Function: add
Parameters: a, b
Returns: sum
*/
int add(int a, int b) {
  return a + b;
}
)C";
    DocstringChecker c;
    auto issues = c.checkContent(src, "mem://c/add.c");
    EXPECT_TRUE(issues.empty()) << "Unexpected issues: " << issues.size();
}

TEST(DocstringChecker, RequiresDocstringAboveDefinition) {
    const char* src = R"C(
int mul(int a, int b) { return a*b; }
)C";
    DocstringChecker c;
    auto issues = c.checkContent(src, "mem://c/mul.c");
    ASSERT_FALSE(issues.empty());
    EXPECT_NE(issues[0].message.find("Missing docstring"), std::string::npos);
}

TEST(DocstringChecker, RequiresFieldsAndParamNames) {
    const char* src = R"CPP(
/**
Method: Thing::work
Parameters: a
Returns: none
*/
int Thing::work(int a, int b) {
  return a + b;
}
)CPP";
    DocstringChecker c;
    auto issues = c.checkContent(src, "mem://cpp/thing.cpp");
    // Missing mention of param b in doc
    bool hasParamB = false;
    for (auto &i : issues) if (i.message.find("parameter 'b'") != std::string::npos) hasParamB = true;
    EXPECT_TRUE(hasParamB);
}

