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
      resource_id_(0), size_(0.0f), shared_entity_(nullptr) {}

VideoEntity::~VideoEntity() { shared_entity_ = nullptr; }

void VideoEntity::SetDepth(VideoEntity::Depth depth) { depth_ = depth; }

void VideoEntity::SetVisible(bool visible) { visible_ = visible; }

void VideoEntity::AddPosition(const Position &position) {
  positions_.emplace_back(position);
}

void VideoEntity::SetPositions(
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

std::vector<VideoEntity::Position> const &VideoEntity::GetPositions() const {
  return positions_;
}

void VideoEntity::SetSharedEntity(const VideoEntity *entity) {
  shared_entity_ = entity;
}

const VideoEntity *VideoEntity::GetSharedEntity() const {
  return shared_entity_;
}

AudioEntity::AudioEntity(std::string id) : Entity(std::move(id)) {}

World::World(std::string id) : id_(std::move(id)), objects_{}, musics_{} {}

void World::AddObject(VideoEntityPtr object) {
  objects_.emplace(std::make_pair(object->GetId(), object));
}

void World::SetObjects(const std::map<std::string, VideoEntityPtr> &objects) {
  objects_ = objects;
}

void World::AddMusic(mofu::AudioEntityPtr music) {
  musics_.emplace(std::make_pair(music->GetId(), music));
}

void World::SetMusics(const std::map<std::string, AudioEntityPtr> &musics) {
  musics_ = musics;
}

VideoEntityPtr World::GetObject(const std::string &id) const {
  auto it = objects_.find(id);
  if (it == objects_.end()) {
    LOGE("World object not found.");
    return nullptr;
  }
  return it->second;
}

AudioEntityPtr World::GetMusic(const std::string &id) const {
  auto it = musics_.find(id);
  if (it == musics_.end()) {
    LOGE("World music not found.");
    return nullptr;
  }
  return it->second;
}

std::map<std::string, VideoEntityPtr> const &World::GetObjects() const {
  return objects_;
}

std::map<std::string, AudioEntityPtr> const &World::GetMusics() const {
  return musics_;
}

std::string World::GetId() const { return id_; }

Action::Action(std::string id) : id_(std::move(id)), callback_() {}

void Action::SetActionCallback(const std::function<void(WorldPtr)> &callback) {
  callback_ = callback;
}

std::string Action::GetId() const { return id_; }

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

std::chrono::time_point<std::chrono::system_clock> Timer::GetStart() const {
  return start_;
}

} // namespace mofu
