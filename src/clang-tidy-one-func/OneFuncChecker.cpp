// (c) 2025 Sam Caldwell. All Rights Reserved.
// Implementation: one-function-per-file checker for C/C++ sources.

#include "clang-tidy-one-func/OneFuncChecker.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <regex>
#include <sstream>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

namespace onefunc {

const std::array<const char*, 8> kExts = {
    ".c", ".cpp", ".cc", ".cxx", ".h", ".hpp", ".hh", ".hxx"
};

constexpr std::size_t kLineReserve = 256U;

struct FunctionDef {
    std::string name;
    int line = 1; // line where signature starts (1-based)
};

static inline bool isWordChar(char chr) {
    return (std::isalnum(static_cast<unsigned char>(chr)) != 0) || chr == '_';
}

static std::string trim(const std::string& str) {
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

static std::string stripLineComment(const std::string& line) {
    const size_t pos = line.find("//");
    return (pos == std::string::npos) ? line : line.substr(0, pos);
}

static std::string normalizeSpaces(const std::string& src) {
    // Replace all runs of whitespace with a single space, then trim.
    static const std::regex whitespacePattern("\\s+");
    std::string out = std::regex_replace(src, whitespacePattern, " ");
    if (!out.empty() && out.front() == ' ') {
        out.erase(out.begin());
    }
    if (!out.empty() && out.back() == ' ') {
        out.pop_back();
    }
    return out;
}

static bool isControlLike(const std::string& text) {
    static const std::vector<std::string> words = {
        "if", "for", "while", "switch", "catch", "return", "sizeof", "alignof", "decltype", "static_assert"
    };
    const std::string normalized = normalizeSpaces(text);
    const bool found = std::any_of(words.begin(), words.end(), [&](const std::string& word){
        return normalized.rfind(word + " ", 0) == 0 || normalized == word;
    });
    return found;
}

static bool containsControlAnywhere(const std::string& sig) {
    static const std::regex ctrlPattern(R"((^|[^A-Za-z0-9_])(if|for|while|switch|catch|else)\s*\()",
                                        std::regex::ECMAScript);
    return std::regex_search(sig, ctrlPattern);
}

static std::string extractFunctionName(const std::string& maybeSig) {
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

static int updateBraceDepth(int current, const std::string& raw) {
    int depth = current;
    for (const char chr : raw) {
        if (chr == '{') {
            ++depth;
        } else if (chr == '}') {
            --depth;
        }
    }
    return depth;
}

static bool isLikelySignatureLine(const std::string& trimmed, int braceDepth) {
    if (trimmed.empty()) {
        return false;
    }
    if (braceDepth > 1) {
        return false;
    }
    if (trimmed.find('(') == std::string::npos) {
        return false;
    }
    const size_t lbrace = trimmed.find('{');
    if (lbrace == std::string::npos) {
        return false; // require same-line '{'
    }
    if (isControlLike(trimmed) || containsControlAnywhere(trimmed)) {
        return false;
    }
    if (trimmed.find('"') != std::string::npos) {
        return false;
    }
    const size_t rparen = trimmed.rfind(')', lbrace);
    return rparen != std::string::npos;
}

static bool isForbiddenName(const std::string& name) {
    return name == "if" || name == "for" || name == "while" ||
           name == "switch" || name == "catch" || name == "else";
}

static std::vector<bool> computeBlockCommentMask(const std::string& content, std::size_t lineCount) {
    std::vector<bool> inBlock(lineCount, false);
    bool block = false;
    std::size_t lineIndex = 0;
    for (std::size_t i = 0; i < content.size(); ++i) {
        if (!block && i + 1 < content.size() && content[i] == '/' && content[i + 1] == '*') {
            block = true;
            inBlock[lineIndex] = true;
            ++i;
            continue;
        }
        if (block && i + 1 < content.size() && content[i] == '*' && content[i + 1] == '/') {
            block = false;
            inBlock[lineIndex] = true;
            ++i;
            continue;
        }
        if (block) {
            inBlock[lineIndex] = true;
        }
        if (content[i] == '\n' && lineIndex + 1 < inBlock.size()) {
            ++lineIndex;
        }
    }
    return inBlock;
}

static bool hasSourceExtension(const std::filesystem::path& path) {
    const std::string lowerExt = [&]() {
        auto ext = path.extension().string();
        std::string lower;
        lower.reserve(ext.size());
        for (const char chr : ext) {
            lower.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(chr))));
        }
        return lower;
    }();
    return std::any_of(kExts.begin(), kExts.end(), [&](const char* ext) {
        return lowerExt == ext;
    });
}

static std::vector<FunctionDef> findFunctionDefs(const std::vector<std::string>& lines,
                                          const std::vector<bool>& inBlockComment) {
    std::vector<FunctionDef> out;
    const int lineCount = static_cast<int>(lines.size());
    int braceDepth = 0;
    for (int index = 0; index < lineCount; ++index) {
        if (index >= 0 && index < static_cast<int>(inBlockComment.size()) && inBlockComment[index]) {
            continue;
        }
        const std::string& raw = lines[index];
        braceDepth = updateBraceDepth(braceDepth, raw);
        const std::string trimmed = trim(stripLineComment(raw));
        if (!isLikelySignatureLine(trimmed, braceDepth)) {
            continue;
        }
        std::string name = extractFunctionName(trimmed);
        if (name.empty() || isForbiddenName(name)) {
            continue;
        }
        out.push_back(FunctionDef{std::move(name), index + 1});
    }
    return out;
}

// end of internal helpers

bool OneFuncChecker::isSourceFile(const std::filesystem::path& pathIn) {
    auto ext = pathIn.extension().string();
    std::string lower;
    lower.reserve(ext.size());
    for (const char chr : ext) {
        lower.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(chr))));
    }
    return std::any_of(kExts.begin(), kExts.end(), [&](const char* ext) {
        return lower == ext;
    });
}

void OneFuncChecker::addPath(const std::filesystem::path& path) {
    paths_.push_back(path);
}

// Helpers to collect issues from filesystem inputs. Kept internal to reduce
// complexity of the public API implementations.
static void collectFromFile(const std::filesystem::path& filePath, std::vector<Issue>& out) {
    std::ifstream input(filePath);
    if (!input) {
        return;
    }
    const std::string content((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
    auto issues = OneFuncChecker::checkContent(content, filePath);
    out.insert(out.end(), issues.begin(), issues.end());
}

static void collectFromDirectory(const std::filesystem::path& root, std::vector<Issue>& out) {
    std::error_code errorCode;
    for (auto entryIt = std::filesystem::recursive_directory_iterator(root, errorCode);
         entryIt != std::filesystem::recursive_directory_iterator(); ++entryIt) {
        if (errorCode) {
            break;
        }
        if (!entryIt->is_regular_file()) {
            continue;
        }
        if (!hasSourceExtension(entryIt->path())) {
            continue;
        }
        collectFromFile(entryIt->path(), out);
    }
}

std::vector<Issue> OneFuncChecker::run() {
    std::vector<Issue> all;
    for (const auto &path : paths_) {
        std::error_code errorCode;
        if (!std::filesystem::exists(path, errorCode)) {
            continue;
        }
        if (std::filesystem::is_directory(path, errorCode)) {
            collectFromDirectory(path, all);
        } else if (std::filesystem::is_regular_file(path, errorCode) && hasSourceExtension(path)) {
            collectFromFile(path, all);
        }
    }
    return all;
}

std::vector<Issue> OneFuncChecker::checkContent(const std::string& content,
                                                const std::filesystem::path& path) {
    // Build line array
    std::vector<std::string> lines;
    lines.reserve(kLineReserve);
    std::istringstream iss(content);
    std::string line;
    while (std::getline(iss, line)) {
        lines.push_back(line);
    }

    // Compute mask of block-comment lines
    const std::vector<bool> inBlock = computeBlockCommentMask(content, lines.size());

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
