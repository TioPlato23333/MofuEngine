/* Created by Xiaodong Ning (ningxiaodong23333@gmail.com)
 *
 * Copyright (c) 2019 Xiaodong Ning. All rights reserved.
 */

#ifndef CORE_CORE_H_
#define CORE_CORE_H_

#include <cstdint>
#include <string>
#include <vector>

namespace mofu {

class Entity {
public:
  Entity();
  virtual ~Entity() = default;

  void SetSourceFile(const std::string &source_file);
  std::string GetSourceFile() const;

protected:
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

  struct Position {
    float x;
    float y;
  };

public:
  VideoEntity();
  ~VideoEntity() override = default;

  void SetDepth(Depth depth);
  void SetVisible(bool visible);
  void SetPosition(const Position &position);
  Depth GetDepth() const;
  bool GetVisible() const;
  Position GetPosition() const;

private:
  Depth depth_;
  bool visible_;
  Position position_;
};

class AudioEntity : public Entity {
public:
  AudioEntity() = default;
  ~AudioEntity() override = default;
};

using VideoEntityPtr = std::shared_ptr<VideoEntity>;
using AudioEntityPtr = std::shared_ptr<AudioEntity>;

class World {
public:
  World();
  ~World() = default;

  void AddObject(VideoEntityPtr object);
  void SetObjects(const std::vector<VideoEntityPtr> &objects);
  void SetMusic(AudioEntityPtr music);
  VideoEntityPtr GetObject(int i) const;
  AudioEntityPtr GetMusic() const;

private:
  std::vector<VideoEntityPtr> objects_;
  AudioEntityPtr music_;
};

using WorldPtr = std::shared_ptr<World>;

} // namespace mofu

#endif // CORE_CORE_H_
