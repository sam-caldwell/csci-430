// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_NODEKIND_H
#define BASIC_COMPILER_AST_NODEKIND_H

#include <cstdint>

namespace gwbasic {

/**
 * Enum: NodeKind
 *  - Lightweight identifier for AST node kinds enabling LLVM-style RTTI.
 */
enum class NodeKind : std::uint8_t {
    // Abstract/base markers
    AbstractExpr,
    AbstractStmt,
    // Expressions
    NumberExpr,
    StringExpr,
    VarExpr,
    UnaryExpr,
    BinaryExpr,
    CallExpr,
    // Statements
    AssignStmt,
    PrintStmt,
    GotoStmt,
    GosubStmt,
    ReturnStmt,
    IfStmt,
    IfBlockStmt,
    ElseStmt,
    EndIfStmt,
    InputStmt,
    ForStmt,
    NextStmt,
    EndStmt,
    StopStmt,
    SystemStmt,
    RandomizeStmt,
    WhileStmt,
    WendStmt,
    RunStmt,
    CommonStmt,
    ChainStmt,
    MergeStmt,
    OnGotoStmt,
    OnGosubStmt,
    OnErrorGotoStmt,
    ResumeStmt,
    ErrorStmt,
    DimStmt,
    ArrayAssignStmt,
    OpenStmt,
    CloseStmt,
    DataStmt,
    ReadStmt,
    RestoreStmt,
    WriteStmt,
    FileInputStmt,
    LineInputStmt,
    DefFnStmt,
    DefTypeStmt,
    DefSegStmt,
    BloadStmt,
    BsaveStmt,
    PokeStmt,
    CallAbsStmt,
    DefUsrStmt,
    ChdirStmt,
    ColorStmt,
    ScreenStmt,
    CircleStmt,
    ClearStmt,
    EraseStmt,
    SwapStmt,
    MidAssignStmt,
    ClsStmt,
    OptionBaseStmt,
    OptionPrintZonesStmt,
    UnsupportedStmt,
    LocateStmt,
    WidthStmt,
    // New filesystem / OS / env / console statements
    FilesStmt,
    MkdirStmt,
    RmdirStmt,
    KillStmt,
    NameStmt,
    ShellStmt,
    EnvironStmt,
    BeepStmt,
    // Program management / listing
    ListStmt,
    DeleteStmt,
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_NODEKIND_H
