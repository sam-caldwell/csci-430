// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/ElseStmt.h"
#include "basic_compiler/ast/EndIfStmt.h"
#include "basic_compiler/ast/ForStmt.h"
#include "basic_compiler/ast/IfBlockStmt.h"
#include "basic_compiler/ast/Line.h"
#include "basic_compiler/ast/NextStmt.h"
#include "basic_compiler/ast/Program.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/WendStmt.h"
#include "basic_compiler/ast/WhileStmt.h"
#include "basic_compiler/parser/ParseError.h"
#include "basic_compiler/token/TokenType.h"
#include <cstddef>
#include <utility>
#include <vector>

namespace gwbasic {

/*
 * Function: Parser::parseProgram
 * Summary:
 *  Parse the entire token stream into a Program AST and fold blocks.
 * Parameters:
 *  - none
 * Returns:
 *  - Program: Ordered lines with statements; block markers folded
 */
// NOLINTNEXTLINE(readability-function-size,readability-function-cognitive-complexity)
Program Parser::parseProgram() {
    Program prog;
    while (!atEnd()) {
        while (match(TokenType::NewLine)) {}
        if (atEnd()) {
            break;
        }
        prog.lines.push_back(parseLine());
    }

    // Restructure multi-line blocks:
    //  - FOR ... NEXT
    //  - IF ... THEN [ ... ELSE ... ] END IF
    //  - WHILE ... WEND
    // by folding intervening statements into the nearest open block body,
    // eliminating structural marker statements (NEXT/ELSE/END IF).
    Program folded;
    struct BlockEntry {
        enum class Kind { ForK, IfK, WhileK } kind{Kind::ForK};
        ForStmt* f{nullptr};
        IfBlockStmt* ib{nullptr};
        WhileStmt* w{nullptr};
        bool ifInElse{false};
        /// Function: BlockEntry::For
        /// Summary: Create a FOR block entry for folding.
        /// Parameters: p (ForStmt*): Open FOR statement pointer.
        /// Returns: BlockEntry: Initialized as Kind::ForK with pointer set.
        static BlockEntry For(ForStmt* ptr) { BlockEntry entry; entry.kind = Kind::ForK; entry.f = ptr; return entry; }
        /// Function: BlockEntry::If
        /// Summary: Create an IF block entry for folding.
        /// Parameters: p (IfBlockStmt*): Open IF block statement pointer.
        /// Returns: BlockEntry: Initialized as Kind::IfK with pointer set.
        static BlockEntry If(IfBlockStmt* ptr) { BlockEntry entry; entry.kind = Kind::IfK; entry.ib = ptr; return entry; }
        /// Function: BlockEntry::While
        /// Summary: Create a WHILE block entry for folding.
        /// Parameters: p (WhileStmt*): Open WHILE statement pointer.
        /// Returns: BlockEntry: Initialized as Kind::WhileK with pointer set.
        static BlockEntry While(WhileStmt* ptr) { BlockEntry entry; entry.kind = Kind::WhileK; entry.w = ptr; return entry; }
    };
    std::vector<BlockEntry> stack;
    // Helper: find the last index of a given block kind in the stack
    auto findLastOfKind = [&](BlockEntry::Kind kind) -> int {
        for (int i = static_cast<int>(stack.size()) - 1; i >= 0; --i) {
            if (stack[static_cast<size_t>(i)].kind == kind) {
                return i; // return avoids inner-loop break usage
            }
        }
        return -1;
    };
    for (auto&[number, statements] : prog.lines) {
        Line out; out.number = number;
        for (auto& stmtNode : statements) {
            // Handle structural markers regardless of context
            if (const auto* const nextStmt = dyn_cast<NextStmt>(stmtNode.get())) {
                // Handle NEXT with optional var-list: NEXT v1[,v2...]
                // Empty list => close exactly one innermost FOR.
                auto popOne = [&]() {
                    const int idx = findLastOfKind(BlockEntry::Kind::ForK);
                    if (idx < 0) {
                        throw ParseError("NEXT without matching FOR");
                    }
                    // Pop that FOR entry
                    stack.erase(stack.begin() + idx);
                };
                if (nextStmt->vars.empty()) {
                    popOne();
                    continue;
                }
                for (const auto& vname : nextStmt->vars) {
                    // Find innermost FOR and enforce variable match
                    const int idx = findLastOfKind(BlockEntry::Kind::ForK);
                    if (idx < 0) {
                        throw ParseError("NEXT without matching FOR");
                    }
                    if (stack[idx].f->var != vname) {
                        throw ParseError("NEXT variable does not match open FOR variable");
                    }
                    stack.erase(stack.begin() + idx);
                }
                continue;
            }
            if (const auto* const elseStmt = dyn_cast<ElseStmt>(stmtNode.get())) {
                (void)elseStmt;
                // Toggle else for innermost IF
                const int idx = findLastOfKind(BlockEntry::Kind::IfK);
                if (idx < 0) {
                    throw ParseError("ELSE without matching IF");
                }
                if (stack[idx].ifInElse) {
                    throw ParseError("Multiple ELSE in IF block");
                }
                stack[idx].ifInElse = true;
                continue;
            }
            if (const auto* const endIfStmt = dyn_cast<EndIfStmt>(stmtNode.get())) {
                (void)endIfStmt;
                // Close innermost IF
                const int idx = findLastOfKind(BlockEntry::Kind::IfK);
                if (idx < 0) {
                    throw ParseError("END IF without matching IF");
                }
                stack.erase(stack.begin() + idx);
                continue;
            }
            if (const auto* const wendStmt = dyn_cast<WendStmt>(stmtNode.get())) {
                (void)wendStmt;
                const int idx = findLastOfKind(BlockEntry::Kind::WhileK);
                if (idx < 0) {
                    throw ParseError("WEND without matching WHILE");
                }
                stack.erase(stack.begin() + idx);
                continue;
            }

            if (const bool inAnyBlock = !stack.empty()) {
                // Append to the innermost open block's body
                if (const auto&[kind, f, ib, w, ifInElse] = stack.back(); kind == BlockEntry::Kind::ForK) {
                    if (isa<ForStmt>(stmtNode.get())) {
                        // Move into FOR body first
                        f->body.push_back(std::move(stmtNode));
                        if (auto* newF = dyn_cast<ForStmt>(f->body.back().get()); !newF->inlineNext) {
                            stack.push_back(BlockEntry::For(newF));
                        }
                    } else if (isa<IfBlockStmt>(stmtNode.get())) {
                        f->body.push_back(std::move(stmtNode));
                        auto* newI = dyn_cast<IfBlockStmt>(f->body.back().get());
                        if (!newI->inlineEnd) {
                            stack.push_back(BlockEntry::If(newI));
                        }
                    } else if (isa<WhileStmt>(stmtNode.get())) {
                        f->body.push_back(std::move(stmtNode));
                        if (auto* newW = dyn_cast<WhileStmt>(f->body.back().get()); !newW->inlineWend) {
                            stack.push_back(BlockEntry::While(newW));
                        }
                    } else {
                        f->body.push_back(std::move(stmtNode));
                    }
                } else if (kind == BlockEntry::Kind::IfK) { // IfK
                    if (isa<ForStmt>(stmtNode.get())) {
                        if (!ifInElse) {
                            ib->thenBody.push_back(std::move(stmtNode));
                        } else {
                            ib->elseBody.push_back(std::move(stmtNode));
                        }
                        if (auto* newF = dyn_cast<ForStmt>((ifInElse ? ib->elseBody.back().get() : ib->thenBody.back().get())); !newF->inlineNext) {
                            stack.push_back(BlockEntry::For(newF));
                        }
                    } else if (isa<IfBlockStmt>(stmtNode.get())) {
                        if (!ifInElse) {
                            ib->thenBody.push_back(std::move(stmtNode));
                        } else {
                            ib->elseBody.push_back(std::move(stmtNode));
                        }
                        auto* newI = dyn_cast<IfBlockStmt>((ifInElse ? ib->elseBody.back().get() : ib->thenBody.back().get()));
                        if (!newI->inlineEnd) {
                            stack.push_back(BlockEntry::If(newI));
                        }
                    } else if (isa<WhileStmt>(stmtNode.get())) {
                        if (!ifInElse) {
                            ib->thenBody.push_back(std::move(stmtNode));
                        } else {
                            ib->elseBody.push_back(std::move(stmtNode));
                        }
                        if (auto* newW = dyn_cast<WhileStmt>((ifInElse ? ib->elseBody.back().get() : ib->thenBody.back().get())); !newW->inlineWend) {
                            stack.push_back(BlockEntry::While(newW));
                        }
                    } else {
                        if (!ifInElse) {
                            ib->thenBody.push_back(std::move(stmtNode));
                        } else {
                            ib->elseBody.push_back(std::move(stmtNode));
                        }
                    }
                } else { // WhileK
                    if (isa<ForStmt>(stmtNode.get())) {
                        w->body.push_back(std::move(stmtNode));
                        if (auto* newF = dyn_cast<ForStmt>(w->body.back().get()); !newF->inlineNext) {
                            stack.push_back(BlockEntry::For(newF));
                        }
                    } else if (isa<IfBlockStmt>(stmtNode.get())) {
                        w->body.push_back(std::move(stmtNode));
                        auto* newI = dyn_cast<IfBlockStmt>(w->body.back().get());
                        if (!newI->inlineEnd) {
                            stack.push_back(BlockEntry::If(newI));
                        }
                    } else if (isa<WhileStmt>(stmtNode.get())) {
                        w->body.push_back(std::move(stmtNode));
                        if (auto* newW = dyn_cast<WhileStmt>(w->body.back().get()); !newW->inlineWend) {
                            stack.push_back(BlockEntry::While(newW));
                        }
                    } else {
                        w->body.push_back(std::move(stmtNode));
                    }
                }
            } else {
                // Not inside a block: new top-level statement
                if (isa<ForStmt>(stmtNode.get())) {
                    out.statements.push_back(std::move(stmtNode));
                    if (auto* fsPtr = dyn_cast<ForStmt>(out.statements.back().get()); !fsPtr->inlineNext) {
                        stack.push_back(BlockEntry::For(fsPtr));
                    }
                } else if (isa<IfBlockStmt>(stmtNode.get())) {
                    out.statements.push_back(std::move(stmtNode));
                    auto* ibPtr = dyn_cast<IfBlockStmt>(out.statements.back().get());
                    if (!ibPtr->inlineEnd) {
                        stack.push_back(BlockEntry::If(ibPtr));
                    }
                } else if (isa<WhileStmt>(stmtNode.get())) {
                    out.statements.push_back(std::move(stmtNode));
                    if (auto* wbPtr = dyn_cast<WhileStmt>(out.statements.back().get()); !wbPtr->inlineWend) {
                        stack.push_back(BlockEntry::While(wbPtr));
                    }
                } else if (isa<ElseStmt>(stmtNode.get())) {
                    throw ParseError("ELSE without matching IF");
                } else if (isa<EndIfStmt>(stmtNode.get())) {
                    throw ParseError("END IF without matching IF");
                } else if (isa<NextStmt>(stmtNode.get())) {
                    throw ParseError("NEXT without matching FOR");
                } else if (isa<WendStmt>(stmtNode.get())) {
                    throw ParseError("WEND without matching WHILE");
                } else {
                    out.statements.push_back(std::move(stmtNode));
                }
            }
        }
        if (!out.statements.empty()) {
            // Retain any top-level statements (including the FOR opener line)
            folded.lines.push_back(std::move(out));
        }
    }
    // Ensure all blocks closed
    for (const auto& block : stack) {
        if (block.kind == BlockEntry::Kind::ForK) {
            throw ParseError("FOR without matching NEXT");
        }
        if (block.kind == BlockEntry::Kind::IfK) {
            throw ParseError("IF without matching END IF");
        }
        if (block.kind == BlockEntry::Kind::WhileK) {
            throw ParseError("WHILE without matching WEND");
        }
    }
    return folded;
}

} // namespace gwbasic
