/* Created by Xiaodong Ning (ningxiaodong23333@gmail.com)
 *
 * Copyright (c) 2019 Xiaodong Ning. All rights reserved.
 */

#include "core/core.h"
#include "core/log.h"

namespace mofu {

Entity::Entity(std::string id) : id_(std::move(id)), source_file_("") {}

void Entity::SetSourceFile(const std::string &source_file) {
  source_file_ = source_file;
}

std::string Entity::GetSourceFile() const { return source_file_; }

std::string Entity::GetId() const { return id_; }

VideoEntity::VideoEntity(std::string id)
    : Entity(std::move(id)), depth_(kLayout0), visible_(false), positions_(),
      resource_id_(0), size_(0.0f) {}

void VideoEntity::SetDepth(VideoEntity::Depth depth) { depth_ = depth; }

void VideoEntity::SetVisible(bool visible) { visible_ = visible; }

void VideoEntity::AddPosition(const Position &position) {
  positions_.emplace_back(position);
}

void VideoEntity::SetPosition(
    const std::vector<mofu::VideoEntity::Position> &positions) {
  positions_ = positions;
}

void VideoEntity::SetResourceId(int64_t resource_id) {
  resource_id_ = resource_id;
}

VideoEntity::Depth VideoEntity::GetDepth() const { return depth_; }

bool VideoEntity::GetVisible() const { return visible_; }

VideoEntity::Position VideoEntity::GetPosition(int i) const {
  if (positions_.empty() || i < 0 || i >= positions_.size()) {
    LOGE("World object out of index.");
    return {};
  }
  return positions_[i];
}

int64_t VideoEntity::GetResourceId() const { return resource_id_; }

int VideoEntity::GetPositionsNumber() { return positions_.size(); }

AudioEntity::AudioEntity(std::string id) : Entity(std::move(id)) {}

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

int World::GetObjectsNumber() { return objects_.size(); }

Action::Action() : callback_() {}

void Action::SetActionCallback(const std::function<void(WorldPtr)> &callback) {
  callback_ = callback;
}

std::function<void(WorldPtr)> Action::GetActionCallback() const {
  return callback_;
}

Timer::Timer() : start_(), callback_() {}

void Timer::Start() { start_ = std::chrono::system_clock::now(); }

void Timer::SetTimerCallback(
    const std::function<
        bool(std::chrono::time_point<std::chrono::system_clock>)> &callback) {
  callback_ = callback;
}

std::function<bool(std::chrono::time_point<std::chrono::system_clock>)>
Timer::GetTimerCallback() const {
  return callback_;
}

} // namespace mofu
