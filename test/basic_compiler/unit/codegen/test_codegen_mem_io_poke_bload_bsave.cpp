// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
Test: CodeGenLineBlock.MemIO_POKE_BSAVE_BLOAD
Purpose: Exercise POKE (mem write), BSAVE (write from mem), and BLOAD (read to
         mem) codegen paths, which produce fopen/fread/fwrite/fclose and
         pointer arithmetic.
*/
TEST(CodeGenLineBlock, MemIO_POKE_BSAVE_BLOAD) {
    const char* src =
        "10 POKE 100, 65\n"
        "20 BSAVE \"OUT.BIN\", 0, 128\n"
        "30 BLOAD \"OUT.BIN\", 0\n"
        "40 END\n";
    std::string ir = Compiler::compileString(src);
    // POKE path: writes a byte to @gwb_mem via computed address
    ASSERT_NE(ir.find("@gwb_mem"), std::string::npos);
    ASSERT_NE(ir.find("store i8"), std::string::npos);
    // BSAVE/BLOAD paths use fopen/fread/fwrite/fclose and modes
    ASSERT_NE(ir.find("@fopen"), std::string::npos);
    ASSERT_NE(ir.find("@fclose"), std::string::npos);
    ASSERT_NE(ir.find("@fread"), std::string::npos);
    ASSERT_NE(ir.find("@fwrite"), std::string::npos);
}

