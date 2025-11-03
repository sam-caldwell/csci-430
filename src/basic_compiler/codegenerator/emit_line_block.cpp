// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/RTTI.h"
#include <sstream>

namespace gwbasic {
    void CodeGenerator::emitLineBlock(std::ostringstream &out, const Line &line, int lineIndex, int lastIndex) {
        /*
         * Function: CodeGenerator::emitLineBlock
         * Inputs:
         *  - out: IR stream
         *  - line: AST line to emit
         *  - lineIndex/lastIndex: indices to compute fall-through label
         * Outputs:
         *  - void
         * Theory of operation:
         *  - Emits a basic block label for the line, then iterates statements,
         *    generating IR for assignments, PRINT, GOTO, GOSUB/RETURN, IF, INPUT,
         *    and inline FOR loops. Terminates with a branch to the next line or
         *    %exit on END/RETURN/GOTO.
         */
        currentLine_ = line.number;
        out << lineLabelName(line.number) << ":\n";
        {
            std::ostringstream m;
            m << "begin line " << currentLine_;
            log(m.str());
        }
        int localContCounter = 0;
        auto nextLabel = (lineIndex < lastIndex) ? lineLabelName(lineNumbers_[lineIndex + 1]) : std::string("exit");
        bool terminated = false;
        for (size_t i = 0; i < line.statements.size(); ++i) {
            const auto &st = line.statements[i];
            if (auto asg = dyn_cast<AssignStmt>(st.get())) {
                std::string val = emitExpr(out, asg->value.get(), "");
                std::string ir = "  store double ";
                ir += val;
                ir += ", ptr ";
                ir += varAllocaName_[asg->name];
                out << ir << "\n";
                std::ostringstream m;
                m << "line " << currentLine_ << ' ' << nodeName(st.get()) << " -> " << ir;
                log(m.str());
            } else if (auto pr = dyn_cast<PrintStmt>(st.get())) {
                std::vector<const Expr *> items;
                if (pr->value) items.push_back(pr->value.get());
                for (const auto &v: pr->more) items.push_back(v.get());
                for (size_t pi = 0; pi < items.size(); ++pi) {
                    const bool last = (pi + 1 == items.size());
                    const Expr *v = items[pi];
                    if (isa<StringExpr>(v)) {
                        int id = strLiteralId_[dyn_cast<StringExpr>(v)->value];
                        std::string sptr = nextTemp();
                        {
                            std::string ir1 = "  ";
                            ir1 += sptr;
                            ir1 += " = getelementptr inbounds i8, ptr ";
                            ir1 += globalStringName(id);
                            ir1 += ", i64 0";
                            out << ir1 << "\n";
                            {
                                std::ostringstream m;
                                m << "line " << currentLine_ << " PrintStmt -> " << ir1;
                                log(m.str());
                            }
                        }
                        std::string fmt = nextTemp();
                        {
                            std::string ir2 = "  ";
                            ir2 += fmt;
                            ir2 += " = getelementptr inbounds i8, ptr ";
                            ir2 += (last ? "@.fmt_str" : "@.fmt_str_sp");
                            ir2 += ", i64 0";
                            out << ir2 << "\n";
                            {
                                std::ostringstream m;
                                m << "line " << currentLine_ << " PrintStmt -> " << ir2;
                                log(m.str());
                            }
                        }
                        {
                            std::string ir3 = "  call i32 (ptr, ...) @printf(ptr ";
                            ir3 += fmt;
                            ir3 += ", ptr ";
                            ir3 += sptr;
                            ir3 += ")";
                            out << ir3 << "\n";
                            {
                                std::ostringstream m;
                                m << "line " << currentLine_ << " PrintStmt -> " << ir3;
                                log(m.str());
                            }
                        }
                    } else {
                        auto val = emitExpr(out, v, "");
                        std::string fmt = nextTemp();
                        {
                            std::string ir1 = "  ";
                            ir1 += fmt;
                            ir1 += " = getelementptr inbounds i8, ptr ";
                            ir1 += (last ? "@.fmt_num" : "@.fmt_num_sp");
                            ir1 += ", i64 0";
                            out << ir1 << "\n";
                            {
                                std::ostringstream m;
                                m << "line " << currentLine_ << " PrintStmt -> " << ir1;
                                log(m.str());
                            }
                        }
                        {
                            std::string ir2 = "  call i32 (ptr, ...) @printf(ptr ";
                            ir2 += fmt;
                            ir2 += ", double ";
                            ir2 += val;
                            ir2 += ")";
                            out << ir2 << "\n";
                            {
                                std::ostringstream m;
                                m << "line " << currentLine_ << " PrintStmt -> " << ir2;
                                log(m.str());
                            }
                        }
                    }
                }
            } else if (auto gt = dyn_cast<GotoStmt>(st.get())) {
                std::string ir = "  br label %";
                ir += lineLabelName(gt->targetLine);
                out << ir << "\n";
                {
                    std::ostringstream m;
                    m << "line " << currentLine_ << " GotoStmt -> " << ir;
                    log(m.str());
                }
                terminated = true;
                break;
            } else if (auto gs = dyn_cast<GosubStmt>(st.get())) {
                std::string contLbl = lineLabelName(line.number);
                contLbl += "_gosub_cont";
                contLbl += std::to_string(++localContCounter);
                std::string entryLbl = lineLabelName(line.number);
                entryLbl += "_gosub_entry";
                entryLbl += std::to_string(localContCounter);
                out << "  br label %" << entryLbl << "\n";
                emitSubroutineInline(out, gs->targetLine, entryLbl, contLbl);
                out << contLbl << ":\n";
            } else if (auto is = dyn_cast<IfStmt>(st.get())) {
                auto be = dyn_cast<BinaryExpr>(is->cond.get());
                if (!be || (be->op != BinaryOp::Eq && be->op != BinaryOp::Ne && be->op != BinaryOp::Lt && be->op !=
                            BinaryOp::Le && be->op != BinaryOp::Gt && be->op != BinaryOp::Ge)) {
                    throw CodeGenError("IF condition must be a comparison");
                }
                std::string cond = emitComparison(out, be);
                std::string contLbl = "line";
                contLbl += std::to_string(line.number);
                contLbl += "_cont";
                contLbl += std::to_string(++localContCounter);
                std::string ir = "  br i1 ";
                ir += cond;
                ir += ", label %";
                ir += lineLabelName(is->targetLine);
                ir += ", label %";
                ir += contLbl;
                out << ir << "\n";
                {
                    std::ostringstream m;
                    m << "line " << currentLine_ << " IfStmt -> " << ir;
                    log(m.str());
                }
                out << contLbl << ":\n";
            } else if (auto ib = dyn_cast<IfBlockStmt>(st.get())) {
                emitIfBlock(out, ib, lineLabelName(line.number), localContCounter);
            } else if (auto ws = dyn_cast<WhileStmt>(st.get())) {
                emitWhile(out, ws, lineLabelName(line.number), localContCounter);
            } else if (isa<EndStmt>(st.get())) {
                std::string ir = "  br label %exit";
                out << ir << "\n";
                {
                    std::ostringstream m;
                    m << "line " << currentLine_ << " EndStmt -> " << ir;
                    log(m.str());
                }
                terminated = true;
                break;
            } else if (auto ins = dyn_cast<InputStmt>(st.get())) {
                ensureVarAllocated(out, ins->name);
                std::string fmt = nextTemp();
                std::string ir1 = "  ";
                ir1 += fmt;
                ir1 += " = getelementptr inbounds i8, ptr @.fmt_in, i64 0";
                out << ir1 << "\n";
                {
                    std::ostringstream m;
                    m << "line " << currentLine_ << " InputStmt -> " << ir1;
                    log(m.str());
                }
                std::string ir2 = "  call i32 (ptr, ...) @scanf(ptr ";
                ir2 += fmt;
                ir2 += ", ptr ";
                ir2 += varAllocaName_[ins->name];
                ir2 += ")";
                out << ir2 << "\n";
                {
                    std::ostringstream m;
                    m << "line " << currentLine_ << " InputStmt -> " << ir2;
                    log(m.str());
                }
            } else if (auto fs = dyn_cast<ForStmt>(st.get())) {
                emitFor(out, fs, lineLabelName(line.number), localContCounter);
            } else if (auto rz = dyn_cast<RandomizeStmt>(st.get())) {
                // RANDOMIZE [expr]
                if (rz->seed) {
                    auto val = emitExpr(out, rz->seed.get(), "");
                    std::string si = nextTemp();
                    {
                        std::string ir = "  ";
                        ir += si;
                        ir += " = fptosi double ";
                        ir += val;
                        ir += " to i64";
                        out << ir << "\n";
                        {
                            std::ostringstream m;
                            m << "line " << currentLine_ << " Randomize fptosi -> " << ir;
                            log(m.str());
                        }
                    }
                    {
                        std::string ir = "  call void @srand48(i64 ";
                        ir += si;
                        ir += ")";
                        out << ir << "\n";
                        {
                            std::ostringstream m;
                            m << "line " << currentLine_ << " Randomize srand48 -> " << ir;
                            log(m.str());
                        }
                    }
                } else {
                    std::string t = nextTemp();
                    {
                        std::string ir = "  ";
                        ir += t;
                        ir += " = call i64 @time(ptr null)";
                        out << ir << "\n";
                        {
                            std::ostringstream m;
                            m << "line " << currentLine_ << " Randomize time -> " << ir;
                            log(m.str());
                        }
                    }
                    {
                        std::string ir = "  call void @srand48(i64 ";
                        ir += t;
                        ir += ")";
                        out << ir << "\n";
                        {
                            std::ostringstream m;
                            m << "line " << currentLine_ << " Randomize srand48(time) -> " << ir;
                            log(m.str());
                        }
                    }
                }
            } else if (isa<ReturnStmt>(st.get())) {
                std::string ir = "  br label %exit";
                out << ir << "\n";
                {
                    std::ostringstream m;
                    m << "line " << currentLine_ << " ReturnStmt -> " << ir;
                    log(m.str());
                }
                terminated = true;
                break;
            } else if (auto cs = dyn_cast<CommonStmt>(st.get())) {
                // COMMON has no direct codegen effect in this compiler; treat as no-op.
                std::ostringstream m;
                m << "line " << currentLine_ << " CommonStmt (no-op)";
                log(m.str());
            } else if (auto rn = dyn_cast<RunStmt>(st.get())) {
                // Reset all variables to 0.0 and branch to first or specified line
                for (const auto &v: variables_) {
                    auto it = varAllocaName_.find(v);
                    if (it == varAllocaName_.end()) continue;
                    std::string ir = "  store double 0.0, ptr ";
                    ir += it->second;
                    out << ir << "\n";
                    {
                        std::ostringstream m;
                        m << "line " << currentLine_ << " RunStmt reset -> " << ir;
                        log(m.str());
                    }
                }
                int dest = rn->targetLine.has_value()
                               ? *rn->targetLine
                               : (lineNumbers_.empty() ? line.number : lineNumbers_.front());
                std::string ir = "  br label %";
                ir += lineLabelName(dest);
                out << ir << "\n";
                {
                    std::ostringstream m;
                    m << "line " << currentLine_ << " RunStmt branch -> " << ir;
                    log(m.str());
                }
                terminated = true;
                break;
            } else if (auto ch = dyn_cast<ChainStmt>(st.get())) {
                // CHAIN: reset non-preserved variables and branch to target/first line
                if (!ch->all) {
                    for (const auto &v: variables_) {
                        if (commonVariables_.contains(v)) continue; // preserve COMMON
                        auto it = varAllocaName_.find(v);
                        if (it == varAllocaName_.end()) continue;
                        std::string ir = "  store double 0.0, ptr ";
                        ir += it->second;
                        out << ir << "\n";
                        {
                            std::ostringstream m;
                            m << "line " << currentLine_ << " ChainStmt reset -> " << ir;
                            log(m.str());
                        }
                    }
                }
                int dest = ch->targetLine.has_value()
                               ? *ch->targetLine
                               : (lineNumbers_.empty() ? line.number : lineNumbers_.front());
                std::string ir = "  br label %";
                ir += lineLabelName(dest);
                out << ir << "\n";
                {
                    std::ostringstream m;
                    m << "line " << currentLine_ << " ChainStmt branch -> " << ir;
                    log(m.str());
                }
                terminated = true;
                break;
            } else if (dyn_cast<MergeStmt>(st.get())) {
                std::ostringstream m;
                m << "line " << currentLine_ << " MergeStmt (no-op)";
                log(m.str());
            } else {
                throw CodeGenError("Unsupported statement encountered");
            }
        }
        if (!terminated) {
            std::string ir = "  br label %";
            ir += nextLabel;
            out << ir << "\n";
            {
                std::ostringstream m;
                m << "line " << currentLine_ << " fallthrough -> " << ir;
                log(m.str());
            }
        }
    }
} // namespace gwbasic
