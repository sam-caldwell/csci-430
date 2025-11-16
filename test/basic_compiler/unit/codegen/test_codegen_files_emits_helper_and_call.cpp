// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;
/***
 * Test: CodeGenFILES.EmitsHelperAndCall
 * Purpose: Ensure FILES lowers to a call to @gwb_list_files and that
 * runtime helper and libc decls are present in IR.
 */
TEST(CodeGenFILES, EmitsHelperAndCall) {
    const char* src =
        "10 FILES\n"
        "20 END\n";
    const std::string ir = Compiler::compileString(src);
    // Decls used by helper
    ASSERT_NE(ir.find("declare ptr @popen(ptr, ptr)"), std::string::npos);
    ASSERT_NE(ir.find("declare i32 @pclose(ptr)"), std::string::npos);
    // Helper body
    ASSERT_NE(ir.find("define void @gwb_list_files(ptr %dev, ptr %pat)"), std::string::npos);
    // Call site
    ASSERT_NE(ir.find("call void @gwb_list_files(ptr null, ptr null)"), std::string::npos);
}

