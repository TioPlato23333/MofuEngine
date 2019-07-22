/* Created by Xiaodong Ning (ningxiaodong23333@gmail.com)
 *
 * Copyright (c) 2019 Xiaodong Ning. All rights reserved.
 */

#include "core/core.h"
#include "core/log.h"

namespace mofu {

Entity::Entity() : source_file_("") {}

void Entity::SetSourceFile(const std::string &source_file) {
  source_file_ = source_file;
}

std::string Entity::GetSourceFile() const { return source_file_; }

VideoEntity::VideoEntity() : depth_(kLayout0), visible_(false), position_() {}

void VideoEntity::SetDepth(VideoEntity::Depth depth) { depth_ = depth; }

void VideoEntity::SetVisible(bool visible) { visible_ = visible; }

void VideoEntity::SetPosition(const Position &position) {
  position_.x = position.x;
  position_.y = position.y;
}

VideoEntity::Depth VideoEntity::GetDepth() const { return depth_; }

bool VideoEntity::GetVisible() const { return visible_; }

VideoEntity::Position VideoEntity::GetPosition() const { return position_; }

World::World() : objects_{}, music_(nullptr) {}

void World::AddObject(VideoEntityPtr object) {
  objects_.push_back(std::move(object));
}

void World::SetObjects(const std::vector<VideoEntityPtr> &objects) {
  objects_ = objects;
}

void World::SetMusic(AudioEntityPtr music) { music_ = std::move(music); }

VideoEntityPtr World::GetObject(int i) const {
  if (objects_.empty() || i < 0 || i >= objects_.size()) {
    LOGE("World object out of index.");
    return nullptr;
  }
  return objects_[i];
}

AudioEntityPtr World::GetMusic() const { return music_; }

} // namespace mofu
