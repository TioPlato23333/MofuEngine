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
  explicit Entity(std::string source_file);
  virtual ~Entity() = default;

  std::string GetSourceFile() const;

protected:
  std::string source_file_;
};

enum EntityDepth : int8_t {
  kLayout0,
  kLayout1,
  kLayout2,
  KLayoutMax,
};

struct Position {
  float x;
  float y;
};

class VideoEntity : public Entity {
public:
  VideoEntity(std::string source_file, EntityDepth depth);
  ~VideoEntity() override = default;

  void SetVisible(bool visible);
  void SetPosition(const Position &position);
  EntityDepth GetDepth() const;
  bool GetVisible() const;
  Position GetPosition() const;

private:
  EntityDepth depth_;
  bool visible_;
  Position position_;
};

class AudioEntity : public Entity {
public:
  explicit AudioEntity(std::string source_file);
  ~AudioEntity() override = default;
};

class World {
public:
  World();
  ~World() = default;

private:
  std::vector<VideoEntity> objects;
  AudioEntity music;
};

} // namespace mofu

#endif // CORE_CORE_H_
