/* Created by Xiaodong Ning (ningxiaodong23333@gmail.com)
 *
 * Copyright (c) 2019 Xiaodong Ning. All rights reserved.
 */

#ifndef VIDEO_VIDEO_H_
#define VIDEO_VIDEO_H_

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
};

#include <string>

#include "core/error.h"

namespace mofu {

class VideoDecoder {
public:
  VideoDecoder();
  ~VideoDecoder();

  bool Open(const std::string &filename);
  bool ReadFrame(ErrorInfo *error);
  void Close();
  double GetPts();
  double GetFrameRate();
  int GetWidth();
  int GetHeight();
  uint8_t *GetData();

private:
  std::string filename_;
  AVFormatContext *format_ctx_;
  AVCodecContext *codec_ctx_;
  AVCodec *codec_;
  SwsContext *sws_ctx_;
  AVFrame *frame_in_;
  AVFrame *frame_out_;
  int video_stream_index_;
  uint8_t *buffer_;
};

using VideoDecoderPtr = std::shared_ptr<VideoDecoder>;

} // namespace mofu

#endif // VIDEO_VIDEO_H_
