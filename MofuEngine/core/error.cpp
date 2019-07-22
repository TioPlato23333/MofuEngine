/* Created by Xiaodong Ning (ningxiaodong23333@gmail.com)
 *
 * Copyright (c) 2019 Xiaodong Ning. All rights reserved.
 */

#include "core/error.h"

namespace mofu {

ErrorInfo::ErrorInfo() : type_(kOk), message_("") {}

ErrorInfo::ErrorInfo(ErrorType type, std::string message)
    : type_(type), message_(std::move(message)) {}

} // namespace mofu
