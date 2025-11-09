// (c) 2025 Sam Caldwell. All Rights Reserved.
// Implementation of a lightweight docstring checker for C/C++ sources.

#include "clang-tidy-docstring/DocstringChecker.h"

#include <regex>
#include <fstream>
#include <sstream>

namespace doccheck {

namespace {

static const std::vector<std::string> kExts = {
    ".c", ".cpp", ".cc", ".cxx", ".h", ".hpp", ".hh", ".hxx"
};

struct FunctionDef {
    std::string name;
    std::vector<std::string> params; // parameter names
    int line = 1; // line of the signature start (1-based)
    int docLine = -1; // starting line of doc block if found
    std::string doc; // extracted doc block text
};

static inline bool isWordChar(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
}

static std::string trim(const std::string& s) {
    size_t i = 0, j = s.size();
    while (i < j && std::isspace(static_cast<unsigned char>(s[i]))) ++i;
    while (j > i && std::isspace(static_cast<unsigned char>(s[j-1]))) --j;
    return s.substr(i, j - i);
}

static std::string stripLineComment(const std::string& s) {
    // Remove // comments (naive, ignores // within strings)
    size_t p = s.find("//");
    return (p == std::string::npos) ? s : s.substr(0, p);
}

static std::string removeBlockComments(const std::string& in) {
    std::string out;
    out.reserve(in.size());
    bool inBlock = false;
    for (size_t i = 0; i < in.size(); ++i) {
        if (!inBlock && i + 1 < in.size() && in[i] == '/' && in[i+1] == '*') {
            inBlock = true; ++i; continue;
        }
        if (inBlock && i + 1 < in.size() && in[i] == '*' && in[i+1] == '/') {
            inBlock = false; ++i; continue;
        }
        if (!inBlock) out.push_back(in[i]);
    }
    return out;
}

static std::string normalizeSpaces(std::string s) {
    // Collapse consecutive whitespace to single spaces
    std::string out; out.reserve(s.size());
    bool inSpace = false;
    for (char c : s) {
        if (std::isspace(static_cast<unsigned char>(c))) {
            if (!inSpace) { out.push_back(' '); inSpace = true; }
        } else {
            inSpace = false; out.push_back(c);
        }
    }
    // trim
    if (!out.empty() && out.front() == ' ') out.erase(out.begin());
    if (!out.empty() && out.back() == ' ') out.pop_back();
    return out;
}

// Attempt to extract function name from a signature string.
// Strategy: find last '('; scan backward to previous non-space sequence;
// take token after last '::' and before '(' as name.
static std::string extractFunctionName(const std::string& maybeSig) {
    auto sig = normalizeSpaces(maybeSig);
    size_t lp = sig.rfind('(');
    if (lp == std::string::npos) return {};
    // skip any spaces before '('
    size_t i = lp;
    while (i > 0 && std::isspace(static_cast<unsigned char>(sig[i-1]))) --i;
    // now move left over identifier chars, ':', '~', '>' (for operators), and operator keyword
    size_t j = i;
    while (j > 0 && (isWordChar(sig[j-1]) || sig[j-1] == ':' || sig[j-1] == '~' || sig[j-1] == '>' || sig[j-1] == '=')) {
        --j;
    }
    std::string token = sig.substr(j, i - j);
    token = trim(token);
    // Handle operators like operator<<, operator new, etc.
    if (token.rfind("operator", 0) == 0) return token; // accept as-is
    // strip qualifiers like Class::
    size_t pos = token.rfind("::");
    if (pos != std::string::npos) token = token.substr(pos + 2);
    return token;
}

// Parse parameter names from inside parentheses of a signature string.
static std::vector<std::string> extractParamNames(const std::string& sig) {
    std::vector<std::string> out;
    size_t lp = sig.rfind('(');
    size_t rp = sig.find(')', lp == std::string::npos ? 0 : lp);
    if (lp == std::string::npos || rp == std::string::npos || rp < lp) return out;
    std::string inner = sig.substr(lp + 1, rp - lp - 1);
    std::string s = inner;
    // remove comments to avoid commas within comments
    s = removeBlockComments(s);
    // simple split by commas but honor template/paren/nesting
    std::vector<std::string> parts;
    int angle = 0, paren = 0, brace = 0;
    std::string cur;
    for (size_t i = 0; i < s.size(); ++i) {
        char c = s[i];
        if (c == '<') angle++;
        else if (c == '>') angle = std::max(0, angle-1);
        else if (c == '(') paren++;
        else if (c == ')') paren = std::max(0, paren-1);
        else if (c == '{') brace++;
        else if (c == '}') brace = std::max(0, brace-1);
        if (c == ',' && angle == 0 && paren == 0 && brace == 0) {
            parts.push_back(cur); cur.clear();
        } else {
            cur.push_back(c);
        }
    }
    if (!cur.empty()) parts.push_back(cur);

    for (auto &p : parts) {
        std::string seg = trim(stripLineComment(p));
        if (seg.empty()) continue;
        // handle ellipsis (varargs)
        if (seg == "...") { out.emplace_back("..."); continue; }
        // remove default value assignment
        size_t eq = seg.find('=');
        if (eq != std::string::npos) seg = trim(seg.substr(0, eq));
        // remove qualifiers at end
        static const std::vector<std::string> trailing = {"const", "volatile", "noexcept"};
        bool changed = true;
        while (changed) {
            changed = false; std::string t = trim(seg);
            for (auto &kw : trailing) {
                size_t n = t.size(), k = kw.size();
                if (n >= k && t.compare(n-k, k, kw) == 0 && (n==k || std::isspace(static_cast<unsigned char>(t[n-k-1])))) {
                    seg = trim(t.substr(0, n-k)); changed = true; break;
                }
            }
        }
        // now attempt to capture the last identifier as the name
        std::smatch m;
        std::regex re("([A-Za-z_][A-Za-z_0-9]*)\\s*(?:\\[[^\\]]*\\])?\\s*$");
        if (std::regex_search(seg, m, re)) {
            out.emplace_back(m[1]);
        } else {
            // could be unnamed parameter (e.g., void) or function pointer; skip
        }
    }
    // Special-case: single 'void' means no parameters
    std::string lowered; lowered.reserve(inner.size());
    for (char c : inner) lowered.push_back(std::tolower(static_cast<unsigned char>(c)));
    if (trim(lowered) == "void") out.clear();
    return out;
}

static bool isControlLike(const std::string& s) {
    static const std::vector<std::string> words = {
        "if", "for", "while", "switch", "catch", "return", "sizeof", "alignof", "decltype", "static_assert"
    };
    std::string t = normalizeSpaces(s);
    // check word at start of string
    for (const auto &w : words) {
        if (t.rfind(w + " ", 0) == 0 || t == w) return true;
    }
    return false;
}

// Extract nearest docstring immediately above 'startLine' (0-based index into lines).
static std::pair<int, std::string> extractDocAbove(const std::vector<std::string>& lines, int startLine) {
    // Prefer C-style block comments that terminate at or above the previous line.
    int i = startLine - 1;
    if (i < 0) return {-1, {}};

    // Skip over blank lines with whitespace only? Policy: require immediate adjacency or only comment lines between.
    // We'll stop if we encounter a non-empty non-comment line.
    // Try '///' group first.
    std::string accum;
    int first = -1;

    // Look for contiguous ///
    int j = i;
    while (j >= 0) {
        std::string t = trim(lines[j]);
        if (t.rfind("///", 0) == 0) {
            first = j; accum = t.substr(3) + "\n" + accum; --j; continue;
        }
        if (t.empty()) { --j; continue; }
        break;
    }
    if (first != -1) {
        return {first+1, accum}; // return 1-based line
    }

    // Search for /* ... */ ending not too far above (scan up to ~40 lines)
    int end = -1;
    for (int k = i; k >= std::max(0, i - 40); --k) {
        std::string t = lines[k];
        if (t.find("*/") != std::string::npos) { end = k; break; }
        if (!trim(t).empty() && trim(t).rfind("//", 0) != 0) {
            // hit non-comment content; keep scanning to find a closer block end
        }
    }
    if (end == -1) return {-1, {}};

    int begin = -1;
    for (int k = end; k >= std::max(0, end - 200); --k) {
        auto pos = lines[k].find("/*");
        if (pos != std::string::npos) { begin = k; break; }
    }
    if (begin == -1) return {-1, {}};

    // Ensure adjacency: no intervening code lines between end-of-comment and function start
    for (int k = end + 1; k < i; ++k) {
        std::string t = trim(lines[k]);
        if (t.empty()) continue;
        if (t.rfind("//", 0) == 0 || t.rfind("///", 0) == 0) continue; // allow line comments
        // any other content means not immediately above
        return {-1, {}};
    }

    std::ostringstream oss;
    for (int k = begin; k <= end; ++k) oss << lines[k] << '\n';
    return {begin + 1, oss.str()};
}

static bool containsWord(const std::string& hay, const std::string& needle) {
    if (needle.empty()) return false;
    // Escape regex meta chars in needle
    std::string escaped = std::regex_replace(needle,
                                             std::regex(R"([\^\$\.\|\?\*\+\(\)\[\]\{\}])"),
                                             R"(\$&)"
    );
    std::string pattern = std::string("(^|[^A-Za-z0-9_])") + escaped + std::string("([^A-Za-z0-9_]|$)");
    std::regex re(pattern);
    return std::regex_search(hay, re);
}

// Helper: detect presence of control statements anywhere in a candidate signature.
static bool containsControlAnywhere(const std::string& sig) {
    // Match control statements like if/for/while/switch/catch/else if regardless of spacing
    static const std::regex re(R"((^|[^A-Za-z0-9_])(if|for|while|switch|catch|else)\s*\()",
                               std::regex::ECMAScript);
    return std::regex_search(sig, re);
}

// Scan for function/method definitions in non-comment code lines.
static std::vector<FunctionDef> findFunctionDefs(const std::vector<std::string>& lines,
                                                 const std::vector<bool>& inBlockComment) {
    std::vector<FunctionDef> out;
    const int N = static_cast<int>(lines.size());
    int braceDepth = 0;
    for (int i = 0; i < N; ++i) {
        if (i >= 0 && i < static_cast<int>(inBlockComment.size()) && inBlockComment[i]) continue; // skip block comments
        std::string line = stripLineComment(lines[i]);
        std::string t = trim(line);
        // Track brace depth on non-comment content (approximate)
        int opens = 0, closes = 0;
        for (char c : line) { if (c == '{') ++opens; else if (c == '}') ++closes; }

        if (t.empty()) { braceDepth += opens - closes; continue; }
        // Only consider potential function definitions at top-level or namespace scope
        if (braceDepth > 1) { braceDepth += opens - closes; continue; }
        // potential signature if contains '(' and not ';' at end of signature block
        if (t.find('(') == std::string::npos) continue;
        if (isControlLike(t)) continue;

        // accumulate up to reach an opening '{' or end with '{' on same/next line
        std::string sig = t;
        int sigStart = i;
        int k = i;
        bool hasLBrace = (t.find('{') != std::string::npos);
        while (!hasLBrace && k + 1 < N) {
            if (k + 1 < static_cast<int>(inBlockComment.size()) && inBlockComment[k+1]) { ++k; continue; }
            std::string next = stripLineComment(lines[k+1]);
            std::string nt = trim(next);
            if (nt.empty()) { ++k; continue; }
            sig += ' '; sig += nt;
            ++k;
            if (nt.find('{') != std::string::npos) { hasLBrace = true; break; }
            if (nt.back() == ';') break; // declaration only
            if (nt.find("->") != std::string::npos) {
                // potentially trailing return type; keep going
            }
        }
        if (!hasLBrace) { i = k; braceDepth += opens - closes; continue; }
        // Accept even if semicolons exist later on the same line (e.g., one-line bodies)
        // Basic heuristic to exclude initializer lists, lambdas
        if (sig.find("=") != std::string::npos && sig.find("[]") != std::string::npos) { i = k; continue; }
        if (containsControlAnywhere(sig)) { i = k; continue; }

        // Heuristic: skip any candidate containing string literals which can fake signatures
        if (sig.find('"') != std::string::npos) { i = k; continue; }

        // Heuristic: require that the character sequence right before the '{'
        // looks like the end of a function signature ')', optionally followed by
        // qualifiers (const/noexcept) or trailing return type "-> T".
        auto looksLikeFuncSig = [](const std::string& s) {
            std::string x = normalizeSpaces(s);
            size_t lbrace = x.find('{');
            if (lbrace == std::string::npos) return false;
            // Skip lambdas or captures: presence of [] before '{'
            size_t capL = x.find('[');
            size_t capR = x.find(']');
            if (capL != std::string::npos && capR != std::string::npos && capL < lbrace && capR < lbrace) return false;
            // Skip assignments/declarations with '=' preceding '{' (e.g., auto f = [](){ ... })
            size_t eq = x.find('=');
            if (eq != std::string::npos && eq < lbrace) return false;
            std::string before = trim(x.substr(0, lbrace));
            size_t rp = before.rfind(')');
            if (rp == std::string::npos) return false;
            std::string tail = trim(before.substr(rp + 1));
            if (tail.empty()) return true;
            // allow sequences like "const", "noexcept", "const noexcept"
            if (tail == "const" || tail == "noexcept" || tail == "const noexcept" || tail == "noexcept const") return true;
            // or trailing return type using '->'
            if (tail.rfind("-> ", 0) == 0) return true;
            return false;
        };
        if (!looksLikeFuncSig(sig)) { i = k; braceDepth += opens - closes; continue; }

        std::string name = extractFunctionName(sig);
        if (name.empty()) { i = k; continue; }
        // Extra guard to avoid treating control statements as functions
        if (name == "if" || name == "for" || name == "while" || name == "switch" || name == "catch" || name == "else") { i = k; continue; }

        FunctionDef f; f.name = name; f.params = extractParamNames(sig); f.line = sigStart + 1;
        auto [docL, doc] = extractDocAbove(lines, sigStart);
        f.docLine = docL; f.doc = doc;
        out.emplace_back(std::move(f));
        i = k;
        // Update brace depth for the line we ended on
        int opensEnd = 0, closesEnd = 0;
        for (char c : lines[i]) { if (c == '{') ++opensEnd; else if (c == '}') ++closesEnd; }
        braceDepth += opensEnd - closesEnd;
    }
    return out;
}

static std::vector<Issue> validateFunction(const FunctionDef& f, const std::filesystem::path& path) {
    std::vector<Issue> issues;
    if (f.docLine < 0 || f.doc.empty()) {
        issues.push_back({path.string(), f.line, "Missing docstring immediately above function: " + f.name});
        return issues;
    }
    std::string low = DocstringChecker::toLower(f.doc);

    bool hasFunctionTag = (low.find("function:") != std::string::npos) || (low.find("method:") != std::string::npos);
    bool hasParamsTag = (low.find("parameters:") != std::string::npos)
                        || (low.find("inputs:") != std::string::npos)
                        || (low.find("input:") != std::string::npos)
                        || (low.find("args:") != std::string::npos)
                        || (low.find("arguments:") != std::string::npos);
    bool hasReturnsTag = (low.find("returns:") != std::string::npos) || (low.find("outputs:") != std::string::npos) || (low.find("output:") != std::string::npos);
    if (!hasFunctionTag) {
        issues.push_back({path.string(), f.docLine, "Docstring missing 'Function:' or 'Method:' field"});
    }
    if (!hasParamsTag) {
        issues.push_back({path.string(), f.docLine, "Docstring missing 'Parameters:' field"});
    }
    if (!hasReturnsTag) {
        issues.push_back({path.string(), f.docLine, "Docstring missing 'Returns:' (or 'Outputs:') field"});
    }
    // Function name must appear in the docstring
    if (!containsWord(DocstringChecker::toLower(f.doc), DocstringChecker::toLower(f.name))) {
        issues.push_back({path.string(), f.docLine, "Docstring must include function/method name '" + f.name + "'"});
    }
    // (Relaxed) Do not require every parameter name to appear.
    // The presence of a Parameters/Inputs section is considered sufficient.
    return issues;
}

} // namespace

bool DocstringChecker::isSourceFile(const std::filesystem::path& p) {
    auto ext = p.extension().string();
    std::string lower;
    lower.reserve(ext.size());
    for (char c : ext) lower.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    for (const auto &e : kExts) if (lower == e) return true;
    return false;
}

std::string DocstringChecker::toLower(std::string s) {
    for (char &c : s) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return s;
}

void DocstringChecker::addPath(const std::filesystem::path& p) {
    paths_.push_back(p);
}

std::vector<Issue> DocstringChecker::run() {
    std::vector<Issue> all;
    for (const auto &p : paths_) {
        std::error_code ec;
        if (std::filesystem::exists(p, ec)) {
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
                if (in) {
                    std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
                    auto issues = checkContent(content, p);
                    all.insert(all.end(), issues.begin(), issues.end());
                }
            }
        }
    }
    return all;
}

std::vector<Issue> DocstringChecker::checkContent(const std::string& content, const std::filesystem::path& path) const {
    std::vector<Issue> out;
    // split content into lines
    std::vector<std::string> lines;
    lines.reserve(256);
    std::istringstream iss(content);
    std::string line;
    while (std::getline(iss, line)) lines.push_back(line);

    // Compute a simple per-line mask for being inside a block comment
    std::vector<bool> inBlock(lines.size(), false);
    bool block = false;
    size_t lineIdx = 0;
    for (size_t i = 0; i < content.size(); ++i) {
        if (!block && i + 1 < content.size() && content[i] == '/' && content[i+1] == '*') { block = true; inBlock[lineIdx] = true; ++i; continue; }
        if (block && i + 1 < content.size() && content[i] == '*' && content[i+1] == '/') { block = false; inBlock[lineIdx] = true; ++i; continue; }
        if (block) { inBlock[lineIdx] = true; }
        if (content[i] == '\n') { if (lineIdx + 1 < inBlock.size()) ++lineIdx; }
    }

    auto defs = findFunctionDefs(lines, inBlock);
    for (const auto &f : defs) {
        auto v = validateFunction(f, path);
        out.insert(out.end(), v.begin(), v.end());
    }
    return out;
}

} // namespace doccheck
