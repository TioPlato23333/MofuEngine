/* Created by Xiaodong Ning (ningxiaodong23333@gmail.com)
 *
 * Copyright (c) 2019 Xiaodong Ning. All rights reserved.
 */

#ifndef CORE_CORE_H_
#define CORE_CORE_H_

#include <chrono>
#include <cstdint>
#include <functional>
#include <map>
#include <string>
#include <vector>

namespace mofu {

class Entity {
public:
  explicit Entity(std::string id);
  virtual ~Entity() = default;

  void SetSourceFile(const std::string &source_file);
  std::string GetSourceFile() const;
  std::string GetId() const;

protected:
  std::string id_;
  std::string source_file_;
};

class VideoEntity : public Entity {
public:
  enum Depth : uint8_t {
    kLayout0,
    kLayout1,
    kLayout2,
    KLayoutMax,
  };

  using Vertex = std::tuple<float, float, float>;

public:
  explicit VideoEntity(const std::string &id);
  ~VideoEntity() override;

  void SetDepth(Depth depth);
  void SetVisible(bool visible);
  void AddRenderVertex(const Vertex &vertex);
  void SetRenderVertice(const std::vector<Vertex> &vertice);
  void SetResourceId(int64_t resource_id);
  Depth GetDepth() const;
  bool GetVisible() const;
  Vertex GetRenderVertex(int i) const;
  std::vector<Vertex> const &GetRenderVertice() const;
  int64_t GetResourceId() const;
  void SetSharedEntity(const VideoEntity *entity);
  const VideoEntity *GetSharedEntity() const;

private:
  Depth depth_;
  bool visible_;
  std::vector<Vertex> render_vertice_;
  int64_t resource_id_;
  float size_;
  const VideoEntity *shared_entity_;
};

class AudioEntity : public Entity {
public:
  explicit AudioEntity(const std::string &id);
  ~AudioEntity() override = default;
};

using VideoEntityPtr = std::shared_ptr<VideoEntity>;
using AudioEntityPtr = std::shared_ptr<AudioEntity>;

class World {
public:
  explicit World(std::string id);
  ~World() = default;

  void AddObject(VideoEntityPtr object);
  void SetObjects(const std::map<std::string, VideoEntityPtr> &objects);
  void AddMusic(AudioEntityPtr music);
  void SetMusics(const std::map<std::string, AudioEntityPtr> &musics);
  VideoEntityPtr GetObject(const std::string &id) const;
  AudioEntityPtr GetMusic(const std::string &id) const;
  std::map<std::string, VideoEntityPtr> const &GetObjects() const;
  std::map<std::string, AudioEntityPtr> const &GetMusics() const;
  std::string GetId() const;

private:
  std::string id_;
  std::map<std::string, VideoEntityPtr> objects_;
  std::map<std::string, AudioEntityPtr> musics_;
};

using WorldPtr = std::shared_ptr<World>;

class Action {
public:
  explicit Action(std::string id);
  virtual ~Action() = default;

  void SetActionCallback(const std::function<void(WorldPtr)> &callback);
  std::function<void(WorldPtr)> GetActionCallback() const;
  std::string GetId() const;

protected:
  std::string id_;
  std::function<void(WorldPtr)> callback_;
};

class Timer {
public:
  Timer();
  ~Timer() = default;

  void Start();
  void SetTimerCallback(
      const std::function<
          bool(std::chrono::time_point<std::chrono::system_clock>)> &callback);
  std::function<bool(std::chrono::time_point<std::chrono::system_clock>)>
  GetTimerCallback() const;
  std::chrono::time_point<std::chrono::system_clock> GetStart() const;

protected:
  std::chrono::time_point<std::chrono::system_clock> start_;
  std::function<bool(std::chrono::time_point<std::chrono::system_clock>)>
      callback_;
};

using ActionPtr = std::shared_ptr<Action>;
using TimerPtr = std::shared_ptr<Timer>;

class Rule {
public:
  Rule() = default;
  virtual ~Rule() = default;

  virtual void ChangeWorld(WorldPtr world) = 0;
};

} // namespace mofu

#endif // CORE_CORE_H_
