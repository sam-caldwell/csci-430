// NOLINTBEGIN(llvm-header-guard)
#ifndef BASIC_COMPILER_WRITETEXTFILE_H
#define BASIC_COMPILER_WRITETEXTFILE_H
// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <filesystem>
#include <string>

/**
 * Function: writeTextFile
 * Purpose:
 *  - Write string contents to a file, truncating any existing content.
 * Inputs:
 *  - path: Destination file path.
 *  - content: Text to write.
 * Outputs:
 *  - void (throws on I/O failures via fstream exceptions if enabled).
 */
void writeTextFile(const std::filesystem::path &path, const std::string &content);

#endif // BASIC_COMPILER_WRITETEXTFILE_H
// NOLINTEND(llvm-header-guard)
