// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: ast_optimizer_optimize_for_body.cpp
 * Purpose: Implement AstOptimizer::optimizeForBody to simplify FOR bodies.
 */
#include "basic_compiler/opt/AstOptimizer.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/ForStmt.h"
#include "basic_compiler/ast/AssignStmt.h"
#include "basic_compiler/ast/PrintStmt.h"

using namespace gwbasic;

void AstOptimizer::optimizeForBody(ForStmt &fs) {
    std::vector<std::unique_ptr<Stmt>> body;
    body.reserve(fs.body.size());
    for (auto& bs : fs.body) {
        if (const auto basg = dyn_cast<AssignStmt>(bs.get())) {
            basg->value = optExpr(std::move(basg->value));
            body.emplace_back(std::move(bs));
            continue;
        }
        if (const auto bpr = dyn_cast<PrintStmt>(bs.get())) {
            if (bpr->value) bpr->value = optExpr(std::move(bpr->value));
            for (auto& v : bpr->more) v = optExpr(std::move(v));
            body.emplace_back(std::move(bs));
            continue;
        }
        body.emplace_back(std::move(bs));
    }
    fs.body = std::move(body);
}

