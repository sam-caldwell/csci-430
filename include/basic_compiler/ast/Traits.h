// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include "basic_compiler/ast/NodeKind.h"

namespace gwbasic {

// NodeKindTraits: Pretty names for logging and UI
template <NodeKind K>
struct NodeKindTraits;

template <> struct NodeKindTraits<NodeKind::AbstractExpr> { static constexpr const char* pretty = "Expr"; };
template <> struct NodeKindTraits<NodeKind::AbstractStmt> { static constexpr const char* pretty = "Stmt"; };

template <> struct NodeKindTraits<NodeKind::NumberExpr> { static constexpr const char* pretty = "NumberExpr"; };
template <> struct NodeKindTraits<NodeKind::StringExpr> { static constexpr const char* pretty = "StringExpr"; };
template <> struct NodeKindTraits<NodeKind::VarExpr>    { static constexpr const char* pretty = "VarExpr"; };
template <> struct NodeKindTraits<NodeKind::UnaryExpr>  { static constexpr const char* pretty = "UnaryExpr"; };
template <> struct NodeKindTraits<NodeKind::BinaryExpr> { static constexpr const char* pretty = "BinaryExpr"; };
template <> struct NodeKindTraits<NodeKind::CallExpr>   { static constexpr const char* pretty = "CallExpr"; };

template <> struct NodeKindTraits<NodeKind::AssignStmt>  { static constexpr const char* pretty = "AssignStmt"; };
template <> struct NodeKindTraits<NodeKind::PrintStmt>   { static constexpr const char* pretty = "PrintStmt"; };
template <> struct NodeKindTraits<NodeKind::GotoStmt>    { static constexpr const char* pretty = "GotoStmt"; };
template <> struct NodeKindTraits<NodeKind::GosubStmt>   { static constexpr const char* pretty = "GosubStmt"; };
template <> struct NodeKindTraits<NodeKind::ReturnStmt>  { static constexpr const char* pretty = "ReturnStmt"; };
template <> struct NodeKindTraits<NodeKind::IfStmt>      { static constexpr const char* pretty = "IfStmt"; };
template <> struct NodeKindTraits<NodeKind::IfBlockStmt> { static constexpr const char* pretty = "IfBlockStmt"; };
template <> struct NodeKindTraits<NodeKind::ElseStmt>    { static constexpr const char* pretty = "ElseStmt"; };
template <> struct NodeKindTraits<NodeKind::EndIfStmt>   { static constexpr const char* pretty = "EndIfStmt"; };
template <> struct NodeKindTraits<NodeKind::InputStmt>   { static constexpr const char* pretty = "InputStmt"; };
template <> struct NodeKindTraits<NodeKind::ForStmt>     { static constexpr const char* pretty = "ForStmt"; };
template <> struct NodeKindTraits<NodeKind::NextStmt>    { static constexpr const char* pretty = "NextStmt"; };
template <> struct NodeKindTraits<NodeKind::EndStmt>     { static constexpr const char* pretty = "EndStmt"; };
template <> struct NodeKindTraits<NodeKind::RandomizeStmt> { static constexpr const char* pretty = "RandomizeStmt"; };
template <> struct NodeKindTraits<NodeKind::WhileStmt>    { static constexpr const char* pretty = "WhileStmt"; };
template <> struct NodeKindTraits<NodeKind::WendStmt>     { static constexpr const char* pretty = "WendStmt"; };
template <> struct NodeKindTraits<NodeKind::RunStmt>      { static constexpr const char* pretty = "RunStmt"; };
template <> struct NodeKindTraits<NodeKind::CommonStmt>   { static constexpr const char* pretty = "CommonStmt"; };
template <> struct NodeKindTraits<NodeKind::ChainStmt>    { static constexpr const char* pretty = "ChainStmt"; };
template <> struct NodeKindTraits<NodeKind::MergeStmt>    { static constexpr const char* pretty = "MergeStmt"; };
template <> struct NodeKindTraits<NodeKind::DimStmt>      { static constexpr const char* pretty = "DimStmt"; };
template <> struct NodeKindTraits<NodeKind::ArrayAssignStmt> { static constexpr const char* pretty = "ArrayAssignStmt"; };
template <> struct NodeKindTraits<NodeKind::OpenStmt>       { static constexpr const char* pretty = "OpenStmt"; };
template <> struct NodeKindTraits<NodeKind::CloseStmt>      { static constexpr const char* pretty = "CloseStmt"; };
template <> struct NodeKindTraits<NodeKind::DataStmt>       { static constexpr const char* pretty = "DataStmt"; };
template <> struct NodeKindTraits<NodeKind::ReadStmt>       { static constexpr const char* pretty = "ReadStmt"; };
template <> struct NodeKindTraits<NodeKind::RestoreStmt>    { static constexpr const char* pretty = "RestoreStmt"; };
template <> struct NodeKindTraits<NodeKind::WriteStmt>      { static constexpr const char* pretty = "WriteStmt"; };
template <> struct NodeKindTraits<NodeKind::FileInputStmt>  { static constexpr const char* pretty = "FileInputStmt"; };
template <> struct NodeKindTraits<NodeKind::LineInputStmt>  { static constexpr const char* pretty = "LineInputStmt"; };
template <> struct NodeKindTraits<NodeKind::DefFnStmt>      { static constexpr const char* pretty = "DefFnStmt"; };
template <> struct NodeKindTraits<NodeKind::DefTypeStmt>    { static constexpr const char* pretty = "DefTypeStmt"; };
template <> struct NodeKindTraits<NodeKind::DefSegStmt>     { static constexpr const char* pretty = "DefSegStmt"; };
template <> struct NodeKindTraits<NodeKind::BloadStmt>      { static constexpr const char* pretty = "BloadStmt"; };
template <> struct NodeKindTraits<NodeKind::BsaveStmt>      { static constexpr const char* pretty = "BsaveStmt"; };
template <> struct NodeKindTraits<NodeKind::PokeStmt>       { static constexpr const char* pretty = "PokeStmt"; };
template <> struct NodeKindTraits<NodeKind::CallAbsStmt>    { static constexpr const char* pretty = "CallAbsStmt"; };
template <> struct NodeKindTraits<NodeKind::DefUsrStmt>     { static constexpr const char* pretty = "DefUsrStmt"; };
template <> struct NodeKindTraits<NodeKind::ChdirStmt>      { static constexpr const char* pretty = "ChdirStmt"; };

// Map a runtime NodeKind to a human-readable pretty name
inline const char* prettyName(NodeKind k) {
    switch (k) {
        case NodeKind::AbstractExpr: return NodeKindTraits<NodeKind::AbstractExpr>::pretty;
        case NodeKind::AbstractStmt: return NodeKindTraits<NodeKind::AbstractStmt>::pretty;
        case NodeKind::NumberExpr: return NodeKindTraits<NodeKind::NumberExpr>::pretty;
        case NodeKind::StringExpr: return NodeKindTraits<NodeKind::StringExpr>::pretty;
        case NodeKind::VarExpr: return NodeKindTraits<NodeKind::VarExpr>::pretty;
        case NodeKind::UnaryExpr: return NodeKindTraits<NodeKind::UnaryExpr>::pretty;
        case NodeKind::BinaryExpr: return NodeKindTraits<NodeKind::BinaryExpr>::pretty;
        case NodeKind::CallExpr: return NodeKindTraits<NodeKind::CallExpr>::pretty;
        case NodeKind::AssignStmt: return NodeKindTraits<NodeKind::AssignStmt>::pretty;
        case NodeKind::PrintStmt: return NodeKindTraits<NodeKind::PrintStmt>::pretty;
        case NodeKind::GotoStmt: return NodeKindTraits<NodeKind::GotoStmt>::pretty;
        case NodeKind::GosubStmt: return NodeKindTraits<NodeKind::GosubStmt>::pretty;
        case NodeKind::ReturnStmt: return NodeKindTraits<NodeKind::ReturnStmt>::pretty;
        case NodeKind::IfStmt: return NodeKindTraits<NodeKind::IfStmt>::pretty;
        case NodeKind::IfBlockStmt: return NodeKindTraits<NodeKind::IfBlockStmt>::pretty;
        case NodeKind::ElseStmt: return NodeKindTraits<NodeKind::ElseStmt>::pretty;
        case NodeKind::EndIfStmt: return NodeKindTraits<NodeKind::EndIfStmt>::pretty;
        case NodeKind::InputStmt: return NodeKindTraits<NodeKind::InputStmt>::pretty;
        case NodeKind::ForStmt: return NodeKindTraits<NodeKind::ForStmt>::pretty;
        case NodeKind::NextStmt: return NodeKindTraits<NodeKind::NextStmt>::pretty;
        case NodeKind::EndStmt: return NodeKindTraits<NodeKind::EndStmt>::pretty;
        case NodeKind::RandomizeStmt: return NodeKindTraits<NodeKind::RandomizeStmt>::pretty;
        case NodeKind::WhileStmt: return NodeKindTraits<NodeKind::WhileStmt>::pretty;
        case NodeKind::WendStmt: return NodeKindTraits<NodeKind::WendStmt>::pretty;
        case NodeKind::RunStmt: return NodeKindTraits<NodeKind::RunStmt>::pretty;
        case NodeKind::CommonStmt: return NodeKindTraits<NodeKind::CommonStmt>::pretty;
        case NodeKind::ChainStmt: return NodeKindTraits<NodeKind::ChainStmt>::pretty;
        case NodeKind::MergeStmt: return NodeKindTraits<NodeKind::MergeStmt>::pretty;
        case NodeKind::DimStmt: return NodeKindTraits<NodeKind::DimStmt>::pretty;
        case NodeKind::ArrayAssignStmt: return NodeKindTraits<NodeKind::ArrayAssignStmt>::pretty;
        case NodeKind::OpenStmt: return NodeKindTraits<NodeKind::OpenStmt>::pretty;
        case NodeKind::CloseStmt: return NodeKindTraits<NodeKind::CloseStmt>::pretty;
        case NodeKind::DataStmt: return NodeKindTraits<NodeKind::DataStmt>::pretty;
        case NodeKind::ReadStmt: return NodeKindTraits<NodeKind::ReadStmt>::pretty;
        case NodeKind::RestoreStmt: return NodeKindTraits<NodeKind::RestoreStmt>::pretty;
        case NodeKind::WriteStmt: return NodeKindTraits<NodeKind::WriteStmt>::pretty;
        case NodeKind::FileInputStmt: return NodeKindTraits<NodeKind::FileInputStmt>::pretty;
        case NodeKind::LineInputStmt: return NodeKindTraits<NodeKind::LineInputStmt>::pretty;
        case NodeKind::DefFnStmt: return NodeKindTraits<NodeKind::DefFnStmt>::pretty;
        case NodeKind::DefTypeStmt: return NodeKindTraits<NodeKind::DefTypeStmt>::pretty;
        case NodeKind::DefSegStmt: return NodeKindTraits<NodeKind::DefSegStmt>::pretty;
        case NodeKind::BloadStmt: return NodeKindTraits<NodeKind::BloadStmt>::pretty;
        case NodeKind::BsaveStmt: return NodeKindTraits<NodeKind::BsaveStmt>::pretty;
        case NodeKind::PokeStmt: return NodeKindTraits<NodeKind::PokeStmt>::pretty;
        case NodeKind::CallAbsStmt: return NodeKindTraits<NodeKind::CallAbsStmt>::pretty;
        case NodeKind::DefUsrStmt: return NodeKindTraits<NodeKind::DefUsrStmt>::pretty;
        case NodeKind::ChdirStmt: return NodeKindTraits<NodeKind::ChdirStmt>::pretty;
    }
    return "Node";
}

// Example node-specific defaults via traits
struct ForStmtTraits { static constexpr double defaultStep = 1.0; };

} // namespace gwbasic
