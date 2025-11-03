// File: src/logger/Logger.cpp
// Purpose: Implementation of logger::Logger

#include "logger/Logger.h"

#include <filesystem>

namespace logger {

Logger::Logger() = default;

Logger::~Logger() { close(); }

bool Logger::open(const std::string& path, bool append) {
  // Ensure parent directory exists if possible
  try {
    std::filesystem::path p(path);
    if (p.has_parent_path()) {
      std::filesystem::create_directories(p.parent_path());
    }
  } catch (...) {
    // Best-effort; ignore directory creation failures, let ofstream handle
  }

  std::ios_base::openmode mode = std::ios::out;
  if (append) mode |= std::ios::app;
  else mode |= std::ios::trunc;

  ofs_.close();
  ofs_.clear();
  ofs_.open(path, mode);
  return ofs_.is_open() && ofs_.good();
}

void Logger::close() {
  if (ofs_.is_open()) {
    ofs_.flush();
    ofs_.close();
  }
}

std::ostream& Logger::stream() {
  if (enabled_ && ofs_.is_open()) return ofs_;
  return null_;
}

}  // namespace logger

