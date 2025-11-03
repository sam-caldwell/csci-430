// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/WriteTextFile.h"
#include <fstream>

/*
 * Function: writeTextFile
 * Inputs:
 *  - path: Destination file path
 *  - content: Text to write to the file
 * Outputs:
 *  - void (writes or truncates file contents)
 * Theory of operation:
 *  - Opens an ofstream for 'path' and streams 'content' into it.
 */
void writeTextFile(const std::filesystem::path &path, const std::string &content) {
    std::ofstream out(path);
    out << content;
}
