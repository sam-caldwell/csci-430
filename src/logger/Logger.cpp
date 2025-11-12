// File: src/logger/Logger.cpp
// Purpose: Implementation of logger::Logger

#include "logger/Logger.h"

#include <filesystem>
#include <ios>
#include <ostream>
#include <string>
#include <system_error>

namespace logger {

Logger::Logger() = default;

Logger::~Logger() noexcept { close(); }

bool Logger::open(const std::string& path, bool append) {
  // Ensure parent directory exists if possible
  {
    const std::filesystem::path pathObj(path);
    if (pathObj.has_parent_path()) {
      std::error_code errorCode;
      std::filesystem::create_directories(pathObj.parent_path(), errorCode);
    }
  }

  std::ios_base::openmode mode = std::ios::out;
  if (append) {
    mode |= std::ios::app;
  } else {
    mode |= std::ios::trunc;
  }

  ofs_.close();
  ofs_.clear();
  ofs_.open(path, mode);
  return ofs_.is_open() && ofs_.good();
}

void Logger::close() noexcept {
  try {
    if (ofs_.is_open()) {
      ofs_.flush();
      ofs_.close();
    }
  } catch (...) {
    enabled_ = false; // suppress exceptions during close
  }
}

std::ostream& Logger::stream() {
  if (enabled_ && ofs_.is_open()) {
    return ofs_;
  }
  return null_;
}

std::ostream& Logger::stream() const {
  if (enabled_ && ofs_.is_open()) {
    return ofs_;
  }
  return null_;
}

}  // namespace logger
