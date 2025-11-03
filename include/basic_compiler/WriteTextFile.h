// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

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

