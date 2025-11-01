// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/semantics/SemanticAnalyzer.h"
#include <sstream>

namespace gwbasic {

bool SemanticAnalyzer::isDeclared(const std::string& name) const {
    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
        if (it->contains(name)) return true;
    }
    return false;
}

void SemanticAnalyzer::declare(const std::string& name) {
    scopes_.back().insert(name);
    vars_.insert(name);
    std::ostringstream m; m << "VarDecl " << name; log(m.str());
}

void SemanticAnalyzer::reference(const std::string& name, const SourcePos& pos) {
    if (!isDeclared(name)) {
        // BASIC: implicitly declare on first use
        declare(name);
        std::ostringstream m; m << "VarImplicitDecl " << name << " @ " << pos.line << ':' << pos.col; log(m.str());
    } else {
        std::ostringstream m; m << "VarRef " << name << " @ " << pos.line << ':' << pos.col; log(m.str());
    }
}

void SemanticAnalyzer::analyzeExpr(const Expr* e) {
    if (!e) return;
    if (auto v = dyn_cast<const VarExpr>(e)) {
        reference(v->name, v->pos);
        return;
    }
    if (auto b = dyn_cast<const BinaryExpr>(e)) {
        analyzeExpr(b->lhs.get());
        analyzeExpr(b->rhs.get());
        return;
    }
    if (auto u = dyn_cast<const UnaryExpr>(e)) {
        analyzeExpr(u->inner.get());
        return;
    }
    if (auto s = dyn_cast<const StringExpr>(e)) {
        strings_.insert(s->value);
        std::ostringstream m; m << "StringLiteral @ " << s->pos.line << ':' << s->pos.col; log(m.str());
        return;
    }
}

void SemanticAnalyzer::analyzeStmt(const Stmt* s) {
    if (auto p = dyn_cast<const PrintStmt>(s)) {
        // PRINT accepts either string or number
        analyzeExpr(p->value.get());
        return;
    }
    if (auto a = dyn_cast<const AssignStmt>(s)) {
        reference(a->name, a->pos);
        // Assignment must be numeric in this implementation
        if (typeOf(a->value.get()) == ValueType::String) {
            std::ostringstream m; m << "TypeError: cannot assign string to numeric var '" << a->name << "' @ " << a->pos.line << ':' << a->pos.col;
            log(m.str());
            throw SemanticError(m.str());
        }
        analyzeExpr(a->value.get());
        return;
    }
    if (auto i = dyn_cast<const IfStmt>(s)) {
        // IF condition must be numeric (result of comparison)
        if (typeOf(i->cond.get()) == ValueType::String) {
            std::ostringstream m; m << "TypeError: IF condition cannot be string @ " << i->pos.line << ':' << i->pos.col;
            log(m.str());
            throw SemanticError(m.str());
        }
        analyzeExpr(i->cond.get());
        // Validate target line exists
        if (!lines_.contains(i->targetLine)) {
            if (strictControlFlow_) {
                std::ostringstream err; err << "ControlFlowError: missing IF target line " << i->targetLine << " @ " << i->pos.line << ':' << i->pos.col;
                log(err.str());
                throw SemanticError(err.str());
            } else {
                std::ostringstream w; w << "Warning: IF missing target line " << i->targetLine << " @ " << i->pos.line << ':' << i->pos.col;
                log(w.str());
            }
        }
        std::ostringstream m; m << "IfThen target=" << i->targetLine << " @ " << i->pos.line << ':' << i->pos.col; log(m.str());
        return;
    }
    if (auto f = dyn_cast<const ForStmt>(s)) {
        // Induction var is a variable; in BASIC it's global, but we note loop scope
        reference(f->var, f->pos);
        std::ostringstream m; m << "For var=" << f->var << " @ " << f->pos.line << ':' << f->pos.col; log(m.str());
        // Type-check bounds and step are numeric
        if (typeOf(f->start.get()) == ValueType::String) {
            std::ostringstream err; err << "TypeError: FOR start must be numeric @ " << f->pos.line << ':' << f->pos.col;
            log(err.str());
            throw SemanticError(err.str());
        }
        if (typeOf(f->end.get()) == ValueType::String) {
            std::ostringstream err; err << "TypeError: FOR end must be numeric @ " << f->pos.line << ':' << f->pos.col;
            log(err.str());
            throw SemanticError(err.str());
        }
        if (f->step && typeOf(f->step.get()) == ValueType::String) {
            std::ostringstream err; err << "TypeError: FOR step must be numeric @ " << f->pos.line << ':' << f->pos.col;
            log(err.str());
            throw SemanticError(err.str());
        }
        analyzeExpr(f->start.get());
        analyzeExpr(f->end.get());
        // Always analyze step (may be null); analyzeExpr handles nulls.
        analyzeExpr(f->step.get());
        enterScope();
        for (const auto& st : f->body) analyzeStmt(st.get());
        exitScope();
        return;
    }
    if (auto in = dyn_cast<const InputStmt>(s)) {
        reference(in->name, in->pos);
        return;
    }
    if (auto g = dyn_cast<const GotoStmt>(s)) {
        std::ostringstream m; m << "Goto target=" << g->targetLine << " @ " << g->pos.line << ':' << g->pos.col; log(m.str());
        if (!lines_.contains(g->targetLine)) {
            if (strictControlFlow_) {
                std::ostringstream err; err << "ControlFlowError: missing GOTO target line " << g->targetLine << " @ " << g->pos.line << ':' << g->pos.col;
                log(err.str());
                throw SemanticError(err.str());
            } else {
                std::ostringstream w; w << "Warning: GOTO missing target line " << g->targetLine << " @ " << g->pos.line << ':' << g->pos.col;
                log(w.str());
            }
        }
        return;
    }
    if (auto gs = dyn_cast<const GosubStmt>(s)) {
        std::ostringstream m; m << "Gosub target=" << gs->targetLine << " @ " << gs->pos.line << ':' << gs->pos.col; log(m.str());
        if (!lines_.contains(gs->targetLine)) {
            if (strictControlFlow_) {
                std::ostringstream err; err << "ControlFlowError: missing GOSUB target line " << gs->targetLine << " @ " << gs->pos.line << ':' << gs->pos.col;
                log(err.str());
                throw SemanticError(err.str());
            } else {
                std::ostringstream w; w << "Warning: GOSUB missing target line " << gs->targetLine << " @ " << gs->pos.line << ':' << gs->pos.col;
                log(w.str());
            }
        }
        return;
    }
    if (dyn_cast<const ReturnStmt>(s)) {
        log("Return");
        return;
    }
    if (dyn_cast<const EndStmt>(s)) {
        log("End");
        return;
    }
}

void SemanticAnalyzer::analyzeLine(const Line& line) {
    currentLine_ = line.number;
    std::ostringstream m; m << "Line " << currentLine_; log(m.str());
    for (const auto& st : line.statements) analyzeStmt(st.get());
}

SemanticAnalyzer::Result SemanticAnalyzer::analyze(const Program& program) {
    // Reset state
    scopes_.clear(); scopes_.emplace_back();
    vars_.clear(); strings_.clear(); lines_.clear();
    currentLine_ = 0;
    // Record all line numbers up-front and check for duplicates (invalid IR otherwise)
    std::unordered_set<int> seen;
    for (const auto& line : program.lines) {
        if (!seen.insert(line.number).second) {
            std::ostringstream err; err << "ControlFlowError: duplicate line number " << line.number;
            log(err.str());
            throw SemanticError(err.str());
        }
        lines_.insert(line.number);
    }
    // Traverse
    for (const auto& line : program.lines) analyzeLine(line);
    // Build result
    Result r;
    r.variables = vars_;
    r.stringLiterals = strings_;
    r.lineNumbers = lines_;
    return r;
}

SemanticAnalyzer::ValueType SemanticAnalyzer::typeOf(const Expr* e) {
    if (!e) return ValueType::Number;
    if (dyn_cast<const NumberExpr>(e)) return ValueType::Number;
    if (dyn_cast<const StringExpr>(e)) return ValueType::String;
    if (auto v = dyn_cast<const VarExpr>(e)) {
        (void)v; // variables are numeric in this implementation
        return ValueType::Number;
    }
    if (auto u = dyn_cast<const UnaryExpr>(e)) {
        auto t = typeOf(u->inner.get());
        if (t == ValueType::String) {
            std::ostringstream m; m << "TypeError: unary '" << u->op << "' not applicable to string @ " << u->pos.line << ':' << u->pos.col;
            log(m.str());
            throw SemanticError(m.str());
        }
        return ValueType::Number;
    }
    if (auto b = dyn_cast<const BinaryExpr>(e)) {
        auto lt = typeOf(b->lhs.get());
        auto rt = typeOf(b->rhs.get());
        switch (b->op) {
            case BinaryOp::Add:
            case BinaryOp::Sub:
            case BinaryOp::Mul:
            case BinaryOp::Div:
                if (lt == ValueType::String || rt == ValueType::String) {
                    std::ostringstream m; m << "TypeError: arithmetic on string @ " << e->pos.line << ':' << e->pos.col;
                    log(m.str());
                    throw SemanticError(m.str());
                }
                return ValueType::Number;
            case BinaryOp::Eq:
            case BinaryOp::Ne:
            case BinaryOp::Lt:
            case BinaryOp::Le:
            case BinaryOp::Gt:
            case BinaryOp::Ge:
                if (lt == ValueType::String || rt == ValueType::String) {
                    if (lt != rt) {
                        std::ostringstream m; m << "TypeError: cannot compare string with number @ " << e->pos.line << ':' << e->pos.col;
                        log(m.str());
                        throw SemanticError(m.str());
                    }
                    return ValueType::Number; // string comparisons are allowed and yield number-like 0/1
                }
                return ValueType::Number;
            default:
                return ValueType::Number;
        }
    }
    return ValueType::Number;
}

} // namespace gwbasic
