// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include "basic_compiler/ast/NodeKind.h"

namespace gwbasic {

// NodeKindTraits: Pretty names for logging and UI
template <NodeKind K>
struct NodeKindTraits;

template <> struct NodeKindTraits<NodeKind::AbstractExpr> { static constexpr auto pretty = "Expr"; };
template <> struct NodeKindTraits<NodeKind::AbstractStmt> { static constexpr auto pretty = "Stmt"; };

template <> struct NodeKindTraits<NodeKind::NumberExpr> { static constexpr auto pretty = "NumberExpr"; };
template <> struct NodeKindTraits<NodeKind::StringExpr> { static constexpr auto pretty = "StringExpr"; };
template <> struct NodeKindTraits<NodeKind::VarExpr>    { static constexpr auto pretty = "VarExpr"; };
template <> struct NodeKindTraits<NodeKind::UnaryExpr>  { static constexpr auto pretty = "UnaryExpr"; };
template <> struct NodeKindTraits<NodeKind::BinaryExpr> { static constexpr auto pretty = "BinaryExpr"; };
template <> struct NodeKindTraits<NodeKind::CallExpr>   { static constexpr auto pretty = "CallExpr"; };

template <> struct NodeKindTraits<NodeKind::AssignStmt>  { static constexpr auto pretty = "AssignStmt"; };
template <> struct NodeKindTraits<NodeKind::PrintStmt>   { static constexpr auto pretty = "PrintStmt"; };
template <> struct NodeKindTraits<NodeKind::GotoStmt>    { static constexpr auto pretty = "GotoStmt"; };
template <> struct NodeKindTraits<NodeKind::GosubStmt>   { static constexpr auto pretty = "GosubStmt"; };
template <> struct NodeKindTraits<NodeKind::ReturnStmt>  { static constexpr auto pretty = "ReturnStmt"; };
template <> struct NodeKindTraits<NodeKind::IfStmt>      { static constexpr auto pretty = "IfStmt"; };
template <> struct NodeKindTraits<NodeKind::IfBlockStmt> { static constexpr auto pretty = "IfBlockStmt"; };
template <> struct NodeKindTraits<NodeKind::ElseStmt>    { static constexpr auto pretty = "ElseStmt"; };
template <> struct NodeKindTraits<NodeKind::EndIfStmt>   { static constexpr auto pretty = "EndIfStmt"; };
template <> struct NodeKindTraits<NodeKind::InputStmt>   { static constexpr auto pretty = "InputStmt"; };
template <> struct NodeKindTraits<NodeKind::ForStmt>     { static constexpr auto pretty = "ForStmt"; };
template <> struct NodeKindTraits<NodeKind::NextStmt>    { static constexpr auto pretty = "NextStmt"; };
template <> struct NodeKindTraits<NodeKind::EndStmt>     { static constexpr auto pretty = "EndStmt"; };
template <> struct NodeKindTraits<NodeKind::StopStmt>    { static constexpr auto pretty = "StopStmt"; };
template <> struct NodeKindTraits<NodeKind::SystemStmt>  { static constexpr auto pretty = "SystemStmt"; };
template <> struct NodeKindTraits<NodeKind::RandomizeStmt> { static constexpr auto pretty = "RandomizeStmt"; };
template <> struct NodeKindTraits<NodeKind::WhileStmt>    { static constexpr auto pretty = "WhileStmt"; };
template <> struct NodeKindTraits<NodeKind::WendStmt>     { static constexpr auto pretty = "WendStmt"; };
template <> struct NodeKindTraits<NodeKind::RunStmt>      { static constexpr auto pretty = "RunStmt"; };
template <> struct NodeKindTraits<NodeKind::CommonStmt>   { static constexpr auto pretty = "CommonStmt"; };
template <> struct NodeKindTraits<NodeKind::ChainStmt>    { static constexpr auto pretty = "ChainStmt"; };
template <> struct NodeKindTraits<NodeKind::MergeStmt>    { static constexpr auto pretty = "MergeStmt"; };
template <> struct NodeKindTraits<NodeKind::DimStmt>      { static constexpr auto pretty = "DimStmt"; };
template <> struct NodeKindTraits<NodeKind::ArrayAssignStmt> { static constexpr auto pretty = "ArrayAssignStmt"; };
template <> struct NodeKindTraits<NodeKind::OpenStmt>       { static constexpr auto pretty = "OpenStmt"; };
template <> struct NodeKindTraits<NodeKind::CloseStmt>      { static constexpr auto pretty = "CloseStmt"; };
template <> struct NodeKindTraits<NodeKind::DataStmt>       { static constexpr auto pretty = "DataStmt"; };
template <> struct NodeKindTraits<NodeKind::ReadStmt>       { static constexpr auto pretty = "ReadStmt"; };
template <> struct NodeKindTraits<NodeKind::RestoreStmt>    { static constexpr auto pretty = "RestoreStmt"; };
template <> struct NodeKindTraits<NodeKind::WriteStmt>      { static constexpr auto pretty = "WriteStmt"; };
template <> struct NodeKindTraits<NodeKind::FileInputStmt>  { static constexpr auto pretty = "FileInputStmt"; };
template <> struct NodeKindTraits<NodeKind::LineInputStmt>  { static constexpr auto pretty = "LineInputStmt"; };
template <> struct NodeKindTraits<NodeKind::DefFnStmt>      { static constexpr auto pretty = "DefFnStmt"; };
template <> struct NodeKindTraits<NodeKind::DefTypeStmt>    { static constexpr auto pretty = "DefTypeStmt"; };
template <> struct NodeKindTraits<NodeKind::DefSegStmt>     { static constexpr auto pretty = "DefSegStmt"; };
template <> struct NodeKindTraits<NodeKind::BloadStmt>      { static constexpr auto pretty = "BloadStmt"; };
template <> struct NodeKindTraits<NodeKind::BsaveStmt>      { static constexpr auto pretty = "BsaveStmt"; };
template <> struct NodeKindTraits<NodeKind::PokeStmt>       { static constexpr auto pretty = "PokeStmt"; };
template <> struct NodeKindTraits<NodeKind::CallAbsStmt>    { static constexpr auto pretty = "CallAbsStmt"; };
template <> struct NodeKindTraits<NodeKind::DefUsrStmt>     { static constexpr auto pretty = "DefUsrStmt"; };
template <> struct NodeKindTraits<NodeKind::ChdirStmt>      { static constexpr auto pretty = "ChdirStmt"; };
template <> struct NodeKindTraits<NodeKind::ColorStmt>      { static constexpr auto pretty = "ColorStmt"; };
template <> struct NodeKindTraits<NodeKind::ScreenStmt>     { static constexpr auto pretty = "ScreenStmt"; };
template <> struct NodeKindTraits<NodeKind::CircleStmt>     { static constexpr auto pretty = "CircleStmt"; };
template <> struct NodeKindTraits<NodeKind::ClearStmt>      { static constexpr auto pretty = "ClearStmt"; };
template <> struct NodeKindTraits<NodeKind::MidAssignStmt>  { static constexpr auto pretty = "MidAssignStmt"; };
template <> struct NodeKindTraits<NodeKind::OnGotoStmt>     { static constexpr auto pretty = "OnGotoStmt"; };
template <> struct NodeKindTraits<NodeKind::OnGosubStmt>    { static constexpr auto pretty = "OnGosubStmt"; };
template <> struct NodeKindTraits<NodeKind::OnErrorGotoStmt> { static constexpr auto pretty = "OnErrorGotoStmt"; };
template <> struct NodeKindTraits<NodeKind::ResumeStmt>      { static constexpr auto pretty = "ResumeStmt"; };
template <> struct NodeKindTraits<NodeKind::ErrorStmt>       { static constexpr auto pretty = "ErrorStmt"; };

// Map a runtime NodeKind to a human-readable pretty name
inline auto prettyName(const NodeKind k) {
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
        case NodeKind::StopStmt: return NodeKindTraits<NodeKind::StopStmt>::pretty;
        case NodeKind::SystemStmt: return NodeKindTraits<NodeKind::SystemStmt>::pretty;
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
        case NodeKind::ColorStmt: return NodeKindTraits<NodeKind::ColorStmt>::pretty;
        case NodeKind::ScreenStmt: return NodeKindTraits<NodeKind::ScreenStmt>::pretty;
        case NodeKind::CircleStmt: return NodeKindTraits<NodeKind::CircleStmt>::pretty;
        case NodeKind::ClearStmt: return NodeKindTraits<NodeKind::ClearStmt>::pretty;
        case NodeKind::MidAssignStmt: return NodeKindTraits<NodeKind::MidAssignStmt>::pretty;
        case NodeKind::OnGotoStmt: return NodeKindTraits<NodeKind::OnGotoStmt>::pretty;
        case NodeKind::OnGosubStmt: return NodeKindTraits<NodeKind::OnGosubStmt>::pretty;
        case NodeKind::OnErrorGotoStmt: return NodeKindTraits<NodeKind::OnErrorGotoStmt>::pretty;
        case NodeKind::ResumeStmt: return NodeKindTraits<NodeKind::ResumeStmt>::pretty;
        case NodeKind::ErrorStmt: return NodeKindTraits<NodeKind::ErrorStmt>::pretty;
    }
    return "Node";
}

// Example node-specific defaults via traits
struct ForStmtTraits { static constexpr double defaultStep = 1.0; };

} // namespace gwbasic
