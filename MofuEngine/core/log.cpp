/* Created by Xiaodong Ning (ningxiaodong23333@gmail.com)
 *
 * Copyright (c) 2019 Xiaodong Ning. All rights reserved.
 */

#include "core/log.h"

#include <iomanip>
#include <iostream>
#include <sstream>

namespace {

constexpr int kMaxMessageLength = 256;

} // namespace

namespace mofu {

std::shared_ptr<Logger> Logger::instance_ = nullptr;

Logger::Logger() : level_(kAll) {}

std::shared_ptr<Logger> Logger::GetInstance() {
  if (!instance_) {
    instance_ = std::make_shared<Logger>();
  }
  return instance_;
}

void Logger::SetLevel(uint8_t level) { level_ = level; }

uint8_t Logger::GetLevel() const { return level_; }

void Logger::PrintLog(LogLevel severity, const char *format, ...) {
  if (!(severity & level_)) {
    return;
  }
  std::stringstream str_stream;
  auto date = std::chrono::system_clock::now();
  auto date_time = std::chrono::system_clock::to_time_t(date);
  auto date_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                     date.time_since_epoch()) %
                 1000;
  str_stream << std::put_time(std::localtime(&date_time), "%F %T") << '.'
             << std::setfill('0') << std::setw(3) << date_ms.count() << " ";
  switch (severity) {
  case LogLevel::kDebug:
    str_stream << "[DEBUG] ";
    break;
  case LogLevel::kInfo:
    str_stream << "[INFO] ";
    break;
  case LogLevel::kWarning:
    str_stream << "[WARNING] ";
    break;
  case LogLevel::kError:
    str_stream << "[ERROR] ";
    break;
  default:
    str_stream << "[OTHER] ";
    break;
  }
  char message[kMaxMessageLength] = {};
  va_list arg_list;
  va_start(arg_list, format);
  vsnprintf(message, kMaxMessageLength - 1, format, arg_list);
  va_end(arg_list);
  str_stream << message;
  std::cout << str_stream.str() << std::endl;
}

} // namespace mofu
