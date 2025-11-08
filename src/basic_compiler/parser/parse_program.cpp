// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/ForStmt.h"
#include "basic_compiler/ast/NextStmt.h"
#include "basic_compiler/ast/IfBlockStmt.h"
#include "basic_compiler/ast/ElseStmt.h"
#include "basic_compiler/ast/EndIfStmt.h"
#include "basic_compiler/ast/WhileStmt.h"
#include "basic_compiler/ast/WendStmt.h"

namespace gwbasic {

/*
 * Function: Parser::parseProgram
 * Inputs:
 *  - none (consumes internal token stream)
 * Outputs:
 *  - Program: AST containing ordered lines with statements
 * Theory of operation:
 *  - Skips leading blank lines; repeatedly parses a numbered line until
 *    EndOfFile, producing the program AST.
 */
// NOLINTNEXTLINE(readability-function-size,readability-function-cognitive-complexity)
Program Parser::parseProgram() {
    Program prog;
    while (!atEnd()) {
        while (match(TokenType::NewLine)) {}
        if (atEnd()) break;
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
        static BlockEntry For(ForStmt* p) { BlockEntry b; b.kind = Kind::ForK; b.f = p; return b; }
        static BlockEntry If(IfBlockStmt* p) { BlockEntry b; b.kind = Kind::IfK; b.ib = p; return b; }
        static BlockEntry While(WhileStmt* p) { BlockEntry b; b.kind = Kind::WhileK; b.w = p; return b; }
    };
    std::vector<BlockEntry> stack;
    for (auto&[number, statements] : prog.lines) {
        Line out; out.number = number;
        for (auto& st : statements) {
            // Handle structural markers regardless of context
            if (const auto nx = dyn_cast<NextStmt>(st.get())) {
                // Handle NEXT with optional var-list: NEXT v1[,v2...]
                // Empty list => close exactly one innermost FOR.
                auto popOne = [&]() {
                    int idx = -1;
                    for (int i = static_cast<int>(stack.size()) - 1; i >= 0; --i) {
                        if (stack[i].kind == BlockEntry::Kind::ForK) { idx = i; break; }
                    }
                    if (idx < 0) throw ParseError("NEXT without matching FOR");
                    // Pop that FOR entry
                    stack.erase(stack.begin() + idx);
                };
                if (nx->vars.empty()) {
                    popOne();
                    continue;
                }
                for (const auto& vname : nx->vars) {
                    // Find innermost FOR and enforce variable match
                    int idx = -1;
                    for (int i = static_cast<int>(stack.size()) - 1; i >= 0; --i) {
                        if (stack[i].kind == BlockEntry::Kind::ForK) { idx = i; break; }
                    }
                    if (idx < 0) throw ParseError("NEXT without matching FOR");
                    if (stack[idx].f->var != vname) throw ParseError("NEXT variable does not match open FOR variable");
                    stack.erase(stack.begin() + idx);
                }
                continue;
            }
            if (const auto els = dyn_cast<ElseStmt>(st.get())) {
                (void)els;
                // Toggle else for innermost IF
                int idx = -1;
                for (int i = static_cast<int>(stack.size()) - 1; i >= 0; --i) {
                    if (stack[i].kind == BlockEntry::Kind::IfK) { idx = i; break; }
                }
                if (idx < 0) throw ParseError("ELSE without matching IF");
                if (stack[idx].ifInElse) throw ParseError("Multiple ELSE in IF block");
                stack[idx].ifInElse = true;
                continue;
            }
            if (const auto ei = dyn_cast<EndIfStmt>(st.get())) {
                (void)ei;
                // Close innermost IF
                int idx = -1;
                for (int i = static_cast<int>(stack.size()) - 1; i >= 0; --i) {
                    if (stack[i].kind == BlockEntry::Kind::IfK) { idx = i; break; }
                }
                if (idx < 0) throw ParseError("END IF without matching IF");
                stack.erase(stack.begin() + idx);
                continue;
            }
            if (const auto we = dyn_cast<WendStmt>(st.get())) {
                (void)we;
                int idx = -1;
                for (int i = static_cast<int>(stack.size()) - 1; i >= 0; --i) {
                    if (stack[i].kind == BlockEntry::Kind::WhileK) { idx = i; break; }
                }
                if (idx < 0) throw ParseError("WEND without matching WHILE");
                stack.erase(stack.begin() + idx);
                continue;
            }

            if (const bool inAnyBlock = !stack.empty()) {
                // Append to the innermost open block's body
                if (const auto&[kind, f, ib, w, ifInElse] = stack.back(); kind == BlockEntry::Kind::ForK) {
                    if (isa<ForStmt>(st.get())) {
                        // Move into FOR body first
                        f->body.push_back(std::move(st));
                        if (auto* newF = dyn_cast<ForStmt>(f->body.back().get()); !newF->inlineNext) stack.push_back(BlockEntry::For(newF));
                    } else if (isa<IfBlockStmt>(st.get())) {
                        f->body.push_back(std::move(st));
                        auto* newI = dyn_cast<IfBlockStmt>(f->body.back().get());
                        if (!newI->inlineEnd) stack.push_back(BlockEntry::If(newI));
                    } else if (isa<WhileStmt>(st.get())) {
                        f->body.push_back(std::move(st));
                        if (auto* newW = dyn_cast<WhileStmt>(f->body.back().get()); !newW->inlineWend) stack.push_back(BlockEntry::While(newW));
                    } else {
                        f->body.push_back(std::move(st));
                    }
                } else if (kind == BlockEntry::Kind::IfK) { // IfK
                    if (isa<ForStmt>(st.get())) {
                        if (!ifInElse) ib->thenBody.push_back(std::move(st));
                        else ib->elseBody.push_back(std::move(st));
                        if (auto* newF = dyn_cast<ForStmt>((ifInElse ? ib->elseBody.back().get() : ib->thenBody.back().get())); !newF->inlineNext) stack.push_back(BlockEntry::For(newF));
                    } else if (isa<IfBlockStmt>(st.get())) {
                        if (!ifInElse) ib->thenBody.push_back(std::move(st));
                        else ib->elseBody.push_back(std::move(st));
                        auto* newI = dyn_cast<IfBlockStmt>((ifInElse ? ib->elseBody.back().get() : ib->thenBody.back().get()));
                        if (!newI->inlineEnd) stack.push_back(BlockEntry::If(newI));
                    } else if (isa<WhileStmt>(st.get())) {
                        if (!ifInElse) ib->thenBody.push_back(std::move(st));
                        else ib->elseBody.push_back(std::move(st));
                        if (auto* newW = dyn_cast<WhileStmt>((ifInElse ? ib->elseBody.back().get() : ib->thenBody.back().get())); !newW->inlineWend) stack.push_back(BlockEntry::While(newW));
                    } else {
                        if (!ifInElse) ib->thenBody.push_back(std::move(st));
                        else ib->elseBody.push_back(std::move(st));
                    }
                } else { // WhileK
                    if (isa<ForStmt>(st.get())) {
                        w->body.push_back(std::move(st));
                        if (auto* newF = dyn_cast<ForStmt>(w->body.back().get()); !newF->inlineNext) stack.push_back(BlockEntry::For(newF));
                    } else if (isa<IfBlockStmt>(st.get())) {
                        w->body.push_back(std::move(st));
                        auto* newI = dyn_cast<IfBlockStmt>(w->body.back().get());
                        if (!newI->inlineEnd) stack.push_back(BlockEntry::If(newI));
                    } else if (isa<WhileStmt>(st.get())) {
                        w->body.push_back(std::move(st));
                        if (auto* newW = dyn_cast<WhileStmt>(w->body.back().get()); !newW->inlineWend) stack.push_back(BlockEntry::While(newW));
                    } else {
                        w->body.push_back(std::move(st));
                    }
                }
            } else {
                // Not inside a block: new top-level statement
                if (isa<ForStmt>(st.get())) {
                    out.statements.push_back(std::move(st));
                    if (auto* fsPtr = dyn_cast<ForStmt>(out.statements.back().get()); !fsPtr->inlineNext) stack.push_back(BlockEntry::For(fsPtr));
                } else if (isa<IfBlockStmt>(st.get())) {
                    out.statements.push_back(std::move(st));
                    auto* ibPtr = dyn_cast<IfBlockStmt>(out.statements.back().get());
                    if (!ibPtr->inlineEnd) stack.push_back(BlockEntry::If(ibPtr));
                } else if (isa<WhileStmt>(st.get())) {
                    out.statements.push_back(std::move(st));
                    if (auto* wbPtr = dyn_cast<WhileStmt>(out.statements.back().get()); !wbPtr->inlineWend) stack.push_back(BlockEntry::While(wbPtr));
                } else if (isa<ElseStmt>(st.get())) {
                    throw ParseError("ELSE without matching IF");
                } else if (isa<EndIfStmt>(st.get())) {
                    throw ParseError("END IF without matching IF");
                } else if (isa<NextStmt>(st.get())) {
                    throw ParseError("NEXT without matching FOR");
                } else if (isa<WendStmt>(st.get())) {
                    throw ParseError("WEND without matching WHILE");
                } else {
                    out.statements.push_back(std::move(st));
                }
            }
        }
        if (!out.statements.empty()) {
            // Retain any top-level statements (including the FOR opener line)
            folded.lines.push_back(std::move(out));
        }
    }
    // Ensure all blocks closed
    for (const auto& b : stack) {
        if (b.kind == BlockEntry::Kind::ForK) throw ParseError("FOR without matching NEXT");
        if (b.kind == BlockEntry::Kind::IfK) throw ParseError("IF without matching END IF");
        if (b.kind == BlockEntry::Kind::WhileK) throw ParseError("WHILE without matching WEND");
    }
    return folded;
}

} // namespace gwbasic
