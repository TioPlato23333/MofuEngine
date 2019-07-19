/* Created by Xiaodong Ning (ningxiaodong23333@gmail.com)
 *
 * Copyright (c) 2019 Xiaodong Ning. All rights reserved.
 */

#ifndef CORE_CORE_H_
#define CORE_CORE_H_

#include <cstdint>

namespace mofu {

enum EntityDepth : int8_t {
  kLayout0,
  kLayout1,
  kLayout2,
};

class Entity {
public:
  Entity();
  ~Entity() = default;

private:
  EntityDepth depth;
};

} // namespace mofu

#endif // CORE_CORE_H_
