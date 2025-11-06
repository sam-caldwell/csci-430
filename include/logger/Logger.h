// File: include/logger/Logger.h
// Purpose: Lightweight stream-style logger with null-sink when disabled.

#pragma once

#include <fstream>
#include <ostream>
#include <streambuf>
#include <string>

namespace logger {

class Logger {
 public:
  Logger();
  ~Logger();

  // Open a log file path. If append is true, appends; otherwise truncates.
  // Returns true on success, false otherwise.
  bool open(const std::string& path, bool append = false);

  // Close the log file if open.
  void close();

  // Enable/disable logging. When disabled or not open, stream() and operator() return a null sink.
  void setEnabled(const bool enabled) { enabled_ = enabled; }
  bool enabled() const { return enabled_; }

  // Get the output stream (file when enabled and open; otherwise null sink).
  std::ostream& stream();
  std::ostream& stream() const;

  // Shorthand to chain: log() << "message" << '\n';
  std::ostream& operator()() { return stream(); }
  std::ostream& operator()() const { return stream(); }

 private:
  // Null sink stream buffer that discards all output
  struct NullBuffer final : public std::streambuf {
    int overflow(const int c) override { return traits_type::not_eof(c); }
  };

  // Null stream that uses NullBuffer
  class NullStream final : public std::ostream {
   public:
    NullStream() : std::ostream(&buf_) {}
   private:
    NullBuffer buf_{};
  };

  mutable std::ofstream ofs_{};
  bool enabled_{false};
  mutable NullStream null_{};
};

}  // namespace logger
