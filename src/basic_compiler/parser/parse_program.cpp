// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/ForStmt.h"
#include "basic_compiler/ast/NextStmt.h"
#include "basic_compiler/ast/IfBlockStmt.h"
#include "basic_compiler/ast/WhileStmt.h"
#include "basic_compiler/ast/WendStmt.h"

namespace gwbasic {

Program Parser::parseProgram() {
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
        enum class Kind { ForK, IfK, WhileK } kind;
        ForStmt* f{nullptr};
        IfBlockStmt* ib{nullptr};
        WhileStmt* w{nullptr};
        bool ifInElse{false};
        static BlockEntry For(ForStmt* p) { BlockEntry b; b.kind = Kind::ForK; b.f = p; return b; }
        static BlockEntry If(IfBlockStmt* p) { BlockEntry b; b.kind = Kind::IfK; b.ib = p; return b; }
        static BlockEntry While(WhileStmt* p) { BlockEntry b; b.kind = Kind::WhileK; b.w = p; return b; }
    };
    std::vector<BlockEntry> stack;
    for (auto& line : prog.lines) {
        Line out; out.number = line.number;
        for (auto& st : line.statements) {
            // Handle structural markers regardless of context
            if (auto nx = dyn_cast<NextStmt>(st.get())) {
                // Find innermost FOR in the stack
                int idx = -1;
                for (int i = static_cast<int>(stack.size()) - 1; i >= 0; --i) {
                    if (stack[i].kind == BlockEntry::Kind::ForK) { idx = i; break; }
                }
                if (idx < 0) throw ParseError("NEXT without matching FOR");
                if (nx->var && !nx->var->empty()) {
                    if (stack[idx].f->var != *nx->var) throw ParseError("NEXT variable does not match open FOR variable");
                }
                // Pop that FOR entry
                (void)st;
                stack.erase(stack.begin() + idx);
                continue;
            }
            if (auto els = dyn_cast<ElseStmt>(st.get())) {
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
            if (auto ei = dyn_cast<EndIfStmt>(st.get())) {
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
            if (auto we = dyn_cast<WendStmt>(st.get())) {
                (void)we;
                int idx = -1;
                for (int i = static_cast<int>(stack.size()) - 1; i >= 0; --i) {
                    if (stack[i].kind == BlockEntry::Kind::WhileK) { idx = i; break; }
                }
                if (idx < 0) throw ParseError("WEND without matching WHILE");
                stack.erase(stack.begin() + idx);
                continue;
            }

            const bool inAnyBlock = !stack.empty();
            if (inAnyBlock) {
                // Append to the innermost open block's body
                auto& top = stack.back();
                if (top.kind == BlockEntry::Kind::ForK) {
                    if (isa<ForStmt>(st.get())) {
                        // Move into FOR body first
                        top.f->body.push_back(std::move(st));
                        auto* newF = dyn_cast<ForStmt>(top.f->body.back().get());
                        if (!newF->inlineNext) stack.push_back(BlockEntry::For(newF));
                    } else if (isa<IfBlockStmt>(st.get())) {
                        top.f->body.push_back(std::move(st));
                        auto* newI = dyn_cast<IfBlockStmt>(top.f->body.back().get());
                        stack.push_back(BlockEntry::If(newI));
                    } else if (isa<WhileStmt>(st.get())) {
                        top.f->body.push_back(std::move(st));
                        auto* newW = dyn_cast<WhileStmt>(top.f->body.back().get());
                        if (!newW->inlineWend) stack.push_back(BlockEntry::While(newW));
                    } else {
                        top.f->body.push_back(std::move(st));
                    }
                } else if (top.kind == BlockEntry::Kind::IfK) { // IfK
                    if (isa<ForStmt>(st.get())) {
                        if (!top.ifInElse) top.ib->thenBody.push_back(std::move(st));
                        else top.ib->elseBody.push_back(std::move(st));
                        auto* newF = dyn_cast<ForStmt>((top.ifInElse ? top.ib->elseBody.back().get() : top.ib->thenBody.back().get()));
                        if (!newF->inlineNext) stack.push_back(BlockEntry::For(newF));
                    } else if (isa<IfBlockStmt>(st.get())) {
                        if (!top.ifInElse) top.ib->thenBody.push_back(std::move(st));
                        else top.ib->elseBody.push_back(std::move(st));
                        auto* newI = dyn_cast<IfBlockStmt>((top.ifInElse ? top.ib->elseBody.back().get() : top.ib->thenBody.back().get()));
                        stack.push_back(BlockEntry::If(newI));
                    } else if (isa<WhileStmt>(st.get())) {
                        if (!top.ifInElse) top.ib->thenBody.push_back(std::move(st));
                        else top.ib->elseBody.push_back(std::move(st));
                        auto* newW = dyn_cast<WhileStmt>((top.ifInElse ? top.ib->elseBody.back().get() : top.ib->thenBody.back().get()));
                        if (!newW->inlineWend) stack.push_back(BlockEntry::While(newW));
                    } else {
                        if (!top.ifInElse) top.ib->thenBody.push_back(std::move(st));
                        else top.ib->elseBody.push_back(std::move(st));
                    }
                } else { // WhileK
                    if (isa<ForStmt>(st.get())) {
                        top.w->body.push_back(std::move(st));
                        auto* newF = dyn_cast<ForStmt>(top.w->body.back().get());
                        if (!newF->inlineNext) stack.push_back(BlockEntry::For(newF));
                    } else if (isa<IfBlockStmt>(st.get())) {
                        top.w->body.push_back(std::move(st));
                        auto* newI = dyn_cast<IfBlockStmt>(top.w->body.back().get());
                        stack.push_back(BlockEntry::If(newI));
                    } else if (isa<WhileStmt>(st.get())) {
                        top.w->body.push_back(std::move(st));
                        auto* newW = dyn_cast<WhileStmt>(top.w->body.back().get());
                        if (!newW->inlineWend) stack.push_back(BlockEntry::While(newW));
                    } else {
                        top.w->body.push_back(std::move(st));
                    }
                }
            } else {
                // Not inside a block: new top-level statement
                if (isa<ForStmt>(st.get())) {
                    out.statements.push_back(std::move(st));
                    auto* fsPtr = dyn_cast<ForStmt>(out.statements.back().get());
                    if (!fsPtr->inlineNext) stack.push_back(BlockEntry::For(fsPtr));
                } else if (isa<IfBlockStmt>(st.get())) {
                    out.statements.push_back(std::move(st));
                    auto* ibPtr = dyn_cast<IfBlockStmt>(out.statements.back().get());
                    stack.push_back(BlockEntry::If(ibPtr));
                } else if (isa<WhileStmt>(st.get())) {
                    out.statements.push_back(std::move(st));
                    auto* wbPtr = dyn_cast<WhileStmt>(out.statements.back().get());
                    if (!wbPtr->inlineWend) stack.push_back(BlockEntry::While(wbPtr));
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
