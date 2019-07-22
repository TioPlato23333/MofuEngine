/* Created by Xiaodong Ning (ningxiaodong23333@gmail.com)
 *
 * Copyright (c) 2019 Xiaodong Ning. All rights reserved.
 */

#ifndef CORE_LOG_H_
#define CORE_LOG_H_

#include <cstdint>
#include <memory>

namespace mofu {

class Logger {
public:
  enum LogLevel : uint8_t {
    kDebug = 1u,
    kInfo = (1u << 1u),
    kWarning = (1u << 2u),
    kError = (1u << 3u),
    kAll = 0xff,
  };

public:
  Logger();
  ~Logger() = default;

  static std::shared_ptr<Logger> GetInstance();
  void SetLevel(uint8_t level);
  uint8_t GetLevel() const;
  void PrintLog(LogLevel severity, const char *format, ...);

private:
  uint8_t level_;
  static std::shared_ptr<Logger> instance_;
};

#define LOG_SET_LEVEL(level) mofu::Logger::GetInstance()->SetLevel(level)
#define LOG_GET_LEVEL mofu::Logger::GetInstance()->GetLevel()
#define LOGD(...)                                                              \
  mofu::Logger::GetInstance()->PrintLog(mofu::Logger::kDebug, __VA_ARGS__)
#define LOGI(...)                                                              \
  mofu::Logger::GetInstance()->PrintLog(mofu::Logger::kInfo, __VA_ARGS__)
#define LOGW(...)                                                              \
  mofu::Logger::GetInstance()->PrintLog(mofu::Logger::kWarning, __VA_ARGS__)
#define LOGE(...)                                                              \
  mofu::Logger::GetInstance()->PrintLog(mofu::Logger::kError, __VA_ARGS__)

} // namespace mofu

#endif // CORE_LOG_H_
