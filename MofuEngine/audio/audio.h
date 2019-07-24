/* Created by Xiaodong Ning (ningxiaodong23333@gmail.com)
 *
 * Copyright (c) 2019 Xiaodong Ning. All rights reserved.
 */

#ifndef AUDIO_AUDIO_H_
#define AUDIO_AUDIO_H_

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}

#include <string>

#include "core/error.h"

namespace mofu {

class AudioDecoder {
public:
  AudioDecoder();
  ~AudioDecoder();

  bool Open(const std::string &filename);
  bool ReadFrame(ErrorInfo *error);
  void Close();
  uint8_t *GetData();

private:
  std::string filename_;
  AVFormatContext *format_ctx_;
  AVCodecContext *codec_ctx_;
  AVCodec *codec_;
  SwrContext *swr_ctx_;
  AVFrame *frame_in_;
  AVFrame *frame_out_;
  int audio_stream_index_;
  uint8_t *buffer_;
};

} // namespace mofu

#endif // AUDIO_AUDIO_H_
