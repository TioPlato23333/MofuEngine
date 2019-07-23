/* Created by Xiaodong Ning (ningxiaodong23333@gmail.com)
 *
 * Copyright (c) 2019 Xiaodong Ning. All rights reserved.
 */

#ifndef CORE_ERROR_H_
#define CORE_ERROR_H_

#include <string>

namespace mofu {

class ErrorInfo {
public:
  enum ErrorType : uint8_t {
    kOk,
    kDecoderError,
    kEof,
  };

public:
  ErrorInfo();
  ErrorInfo(ErrorType type, std::string message);
  ~ErrorInfo() = default;

  ErrorType GetType();

private:
  ErrorType type_;
  std::string message_;
};

} // namespace mofu

#endif // CORE_ERROR_H_
