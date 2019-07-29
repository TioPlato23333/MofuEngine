/* Created by Xiaodong Ning (ningxiaodong23333@gmail.com)
 *
 * Copyright (c) 2019 Xiaodong Ning. All rights reserved.
 */

#include "core/algo.h"

#include <random>

namespace mofu {

int GetRandomNumber(int low, int high) {
  std::random_device r;
  std::default_random_engine engine(r());
  std::uniform_int_distribution<int> uniform_dist(low, high);
  return uniform_dist(engine);
}

} // namespace mofu
