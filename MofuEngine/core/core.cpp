/* Created by Xiaodong Ning (ningxiaodong23333@gmail.com)
 *
 * Copyright (c) 2019 Xiaodong Ning. All rights reserved.
 */

#include "core/core.h"

namespace mofu {

Entity::Entity(std::string source_file)
    : source_file_(std::move(source_file)) {}

std::string Entity::GetSourceFile() const { return source_file_; }

VideoEntity::VideoEntity(std::string source_file, mofu::EntityDepth depth)
    : Entity(std::move(source_file)), depth_(depth), visible_(false),
      position_() {}

void VideoEntity::SetVisible(bool visible) { visible_ = visible; }

void VideoEntity::SetPosition(const Position &position) {
  position_.x = position.x;
  position_.y = position.y;
}

EntityDepth VideoEntity::GetDepth() const { return depth_; }

bool VideoEntity::GetVisible() const { return visible_; }

Position VideoEntity::GetPosition() const { return position_; }

AudioEntity::AudioEntity(std::string source_file)
    : Entity(std::move(source_file)) {}

World::World() : objects{}, music("") {}

} // namespace mofu
