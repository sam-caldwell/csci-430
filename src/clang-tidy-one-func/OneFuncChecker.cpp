// (c) 2025 Sam Caldwell. All Rights Reserved.
// Implementation: one-function-per-file checker for C/C++ sources.

#include "clang-tidy-one-func/OneFuncChecker.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstddef>
#include <fstream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

namespace onefunc {

namespace {

const std::array<const char*, 8> kExts = {
    ".c", ".cpp", ".cc", ".cxx", ".h", ".hpp", ".hh", ".hxx"
};

struct FunctionDef {
    std::string name;
    int line = 1; // line where signature starts (1-based)
};

inline bool isWordChar(char chr) {
    return (std::isalnum(static_cast<unsigned char>(chr)) != 0) || chr == '_';
}

std::string trim(const std::string& str) {
    size_t startIndex = 0;
    size_t endIndex = str.size();
    while (startIndex < endIndex && std::isspace(static_cast<unsigned char>(str[startIndex])) != 0) {
        ++startIndex;
    }
    while (endIndex > startIndex && std::isspace(static_cast<unsigned char>(str[endIndex - 1])) != 0) {
        --endIndex;
    }
    return str.substr(startIndex, endIndex - startIndex);
}

std::string stripLineComment(const std::string& line) {
    const size_t pos = line.find("//");
    return (pos == std::string::npos) ? line : line.substr(0, pos);
}

std::string normalizeSpaces(const std::string& src) {
    std::string out;
    out.reserve(src.size());
    bool inSpace = false;
    for (const char chr : src) {
        if (std::isspace(static_cast<unsigned char>(chr)) != 0) {
            if (inSpace) {
                continue;
            }
            out.push_back(' ');
            inSpace = true;
            continue;
        }
        inSpace = false;
        out.push_back(chr);
    }
    if (!out.empty() && out.front() == ' ') {
        out.erase(out.begin());
    }
    if (!out.empty() && out.back() == ' ') {
        out.pop_back();
    }
    return out;
}

bool isControlLike(const std::string& text) {
    static const std::vector<std::string> words = {
        "if", "for", "while", "switch", "catch", "return", "sizeof", "alignof", "decltype", "static_assert"
    };
    const std::string normalized = normalizeSpaces(text);
    const bool found = std::any_of(words.begin(), words.end(), [&](const std::string& word){
        return normalized.rfind(word + " ", 0) == 0 || normalized == word;
    });
    return found;
}

bool containsControlAnywhere(const std::string& sig) {
    static const std::regex ctrlPattern(R"((^|[^A-Za-z0-9_])(if|for|while|switch|catch|else)\s*\()",
                                        std::regex::ECMAScript);
    return std::regex_search(sig, ctrlPattern);
}

std::string extractFunctionName(const std::string& maybeSig) {
    std::string sig = normalizeSpaces(maybeSig);
    const size_t leftParenPos = sig.rfind('(');
    if (leftParenPos == std::string::npos) { return {}; }
    size_t endPos = leftParenPos;
    while (endPos > 0 && (std::isspace(static_cast<unsigned char>(sig[endPos - 1])) != 0)) {
        --endPos;
    }
    size_t nameStart = endPos;
    while (nameStart > 0 && (isWordChar(sig[nameStart - 1]) || sig[nameStart - 1] == ':' || sig[nameStart - 1] == '~' || sig[nameStart - 1] == '>' || sig[nameStart - 1] == '=')) {
        --nameStart;
    }
    std::string token = trim(sig.substr(nameStart, endPos - nameStart));
    if (token.rfind("operator", 0) == 0) { return token; }
    const size_t scopePos = token.rfind("::");
    if (scopePos != std::string::npos) { token = token.substr(scopePos + 2); }
    return token;
}

std::vector<FunctionDef> findFunctionDefs(const std::vector<std::string>& lines,
                                          const std::vector<bool>& inBlockComment) {
    // Mimic legacy script: count only signatures with '{' on the same line.
    std::vector<FunctionDef> out;
    const int lineCount = static_cast<int>(lines.size());
    int braceDepth = 0;
    for (int index = 0; index < lineCount; ++index) {
        if (index >= 0 && index < static_cast<int>(inBlockComment.size()) && inBlockComment[index]) continue;
        const std::string& raw = lines[index];
        for (const char chr : raw) {
            if (chr == '{') { ++braceDepth; }
            else if (chr == '}') { --braceDepth; }
        }
        std::string trimmed = trim(stripLineComment(raw));
        if (trimmed.empty()) continue;
        if (braceDepth > 1) continue;
        if (trimmed.find('(') == std::string::npos) { continue; }
        size_t lbrace = trimmed.find('{');
        if (lbrace == std::string::npos) { continue; } // require same-line brace
        if (isControlLike(trimmed) || containsControlAnywhere(trimmed)) { continue; }
        if (trimmed.find('"') != std::string::npos) { continue; }
        size_t rightParenPos = trimmed.rfind(')', lbrace);
        if (rightParenPos == std::string::npos) { continue; }
        std::string name = extractFunctionName(trimmed);
        if (name.empty()) { continue; }
        if (name == "if" || name == "for" || name == "while" || name == "switch" || name == "catch" || name == "else") { continue; }
        out.push_back(FunctionDef{std::move(name), index + 1});
    }
    return out;
}

} // namespace

bool OneFuncChecker::isSourceFile(const std::filesystem::path& pathIn) {
    auto ext = pathIn.extension().string();
    std::string lower;
    lower.reserve(ext.size());
    for (const char chr : ext) {
        lower.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(chr))));
    }
    for (const auto &e : kExts) {
        if (lower == e) return true;
    }
    return false;
}

void OneFuncChecker::addPath(const std::filesystem::path& p) {
    paths_.push_back(p);
}

std::vector<Issue> OneFuncChecker::run() {
    std::vector<Issue> all;
    for (const auto &p : paths_) {
        std::error_code ec;
        if (!std::filesystem::exists(p, ec)) continue;
        if (std::filesystem::is_directory(p, ec)) {
            for (auto it = std::filesystem::recursive_directory_iterator(p, ec);
                 it != std::filesystem::recursive_directory_iterator(); ++it) {
                if (ec) break;
                if (!it->is_regular_file()) continue;
                if (!isSourceFile(it->path())) continue;
                std::ifstream in(it->path());
                if (!in) continue;
                std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
                auto issues = checkContent(content, it->path());
                all.insert(all.end(), issues.begin(), issues.end());
            }
        } else if (std::filesystem::is_regular_file(p, ec) && isSourceFile(p)) {
            std::ifstream in(p);
            if (!in) continue;
            std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
            auto issues = checkContent(content, p);
            all.insert(all.end(), issues.begin(), issues.end());
        }
    }
    return all;
}

std::vector<Issue> OneFuncChecker::checkContent(const std::string& content,
                                                const std::filesystem::path& path) const {
    // Build line array
    std::vector<std::string> lines;
    lines.reserve(256);
    std::istringstream iss(content);
    std::string line;
    while (std::getline(iss, line)) lines.push_back(line);

    // Compute mask of block-comment lines
    std::vector<bool> inBlock(lines.size(), false);
    bool block = false; size_t li = 0;
    for (size_t i = 0; i < content.size(); ++i) {
        if (!block && i + 1 < content.size() && content[i] == '/' && content[i+1] == '*') { block = true; inBlock[li] = true; ++i; continue; }
        if (block && i + 1 < content.size() && content[i] == '*' && content[i+1] == '/') { block = false; inBlock[li] = true; ++i; continue; }
        if (block) inBlock[li] = true;
        if (content[i] == '\n') { if (li + 1 < inBlock.size()) ++li; }
    }

    auto defs = findFunctionDefs(lines, inBlock);
    std::vector<Issue> out;
    if (defs.size() > 1) {
        // Report starting from the second definition
        const int extraLine = defs[1].line;
        std::ostringstream msg;
        msg << "File contains " << defs.size() << " function/method definitions; expected at most 1";
        out.push_back(Issue{path.string(), extraLine, msg.str()});
    }
    return out;
}

} // namespace onefunc
