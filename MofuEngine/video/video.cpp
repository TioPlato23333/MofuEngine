/* Created by Xiaodong Ning (ningxiaodong23333@gmail.com)
 *
 * Copyright (c) 2019 Xiaodong Ning. All rights reserved.
 */

#include "video/video.h"

#include <cmath>

#include "core/log.h"

namespace {

constexpr AVPixelFormat kTargetFormat = AV_PIX_FMT_RGBA;

} // namespace

namespace mofu {

VideoDecoder::VideoDecoder()
    : filename_(""), format_ctx_(nullptr), codec_ctx_(nullptr), codec_(nullptr),
      sws_ctx_(nullptr), frame_in_(nullptr), frame_out_(nullptr),
      video_stream_index_(-1), buffer_(nullptr) {}

VideoDecoder::~VideoDecoder() { Close(); }

bool VideoDecoder::Open(const std::string &filename) {
  filename_ = filename;
  if (avformat_open_input(&format_ctx_, filename_.c_str(), nullptr, nullptr)) {
    LOGE("Video decoder avformat_open_input failed in %s.", filename_.c_str());
    return false;
  }
  if (avformat_find_stream_info(format_ctx_, nullptr) < 0) {
    LOGE("Video decoder avformat_find_stream_info failed in %s.",
         filename_.c_str());
    return false;
  }
  video_stream_index_ = -1;
  for (int i = 0; i < format_ctx_->nb_streams; ++i) {
    if (format_ctx_->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
      video_stream_index_ = i;
      break;
    }
  }
  if (video_stream_index_ < 0) {
    LOGE("Video decoder find video stream index failed in %s.",
         filename_.c_str());
    return false;
  }
  codec_ctx_ = format_ctx_->streams[video_stream_index_]->codec;
  codec_ = avcodec_find_decoder(codec_ctx_->codec_id);
  if (!codec_) {
    LOGE("Video decoder avcodec_find_decoder failed in %s.", filename_.c_str());
    return false;
  }
  if (avcodec_open2(codec_ctx_, codec_, nullptr)) {
    LOGE("Video decoder avcodec_open2 failed in %s.", filename_.c_str());
    return false;
  }
  frame_in_ = av_frame_alloc();
  frame_out_ = av_frame_alloc();
  if (!frame_in_ || !frame_out_) {
    LOGE("Video decoder av_frame_alloc failed in %s.", filename_.c_str());
    return false;
  }
  size_t buffer_size = av_image_get_buffer_size(
      kTargetFormat, codec_ctx_->width, codec_ctx_->height, 1);
  buffer_ = static_cast<uint8_t *>(av_malloc(buffer_size));
  if (!buffer_) {
    LOGE("Video decoder av_malloc failed in %s.", filename_.c_str());
    return false;
  }
  if (av_image_fill_arrays(frame_out_->data, frame_out_->linesize, buffer_,
                           kTargetFormat, codec_ctx_->width, codec_ctx_->height,
                           1) < 0) {
    LOGE("Video decoder av_image_fill_arrays failed in %s.", filename_.c_str());
    return false;
  }
  sws_ctx_ =
      sws_getContext(codec_ctx_->width, codec_ctx_->height, codec_ctx_->pix_fmt,
                     codec_ctx_->width, codec_ctx_->height, kTargetFormat,
                     SWS_BILINEAR, nullptr, nullptr, nullptr);
  if (!sws_ctx_) {
    LOGE("Video decoder sws_getContext failed in %s.", filename_.c_str());
    return false;
  }
  return true;
}

bool VideoDecoder::ReadFrame(ErrorInfo *error) {
  while (true) {
    AVPacket packet;
    int got_frame = 0;
    av_init_packet(&packet);
    packet.data = nullptr;
    packet.size = 0;
    if (av_read_frame(format_ctx_, &packet)) {
      if (error) {
        *error = ErrorInfo(ErrorInfo::kEof, "Eof.");
      }
      return false;
    }
    if (packet.stream_index == video_stream_index_) {
      if (avcodec_decode_video2(codec_ctx_, frame_in_, &got_frame, &packet) <
          0) {
        if (error) {
          *error = ErrorInfo(ErrorInfo::kDecoderError, "DecoderError.");
        }
        return false;
      }
    }
    av_free_packet(&packet);
    if (got_frame) {
      sws_scale(sws_ctx_, frame_in_->data, frame_in_->linesize, 0,
                codec_ctx_->height, frame_out_->data, frame_out_->linesize);
      if (error) {
        *error = ErrorInfo(ErrorInfo::kOk, "Ok.");
      }
      return true;
    }
  }
}

void VideoDecoder::Close() {
  if (sws_ctx_) {
    sws_freeContext(sws_ctx_);
    sws_ctx_ = nullptr;
  }
  if (buffer_) {
    av_free(buffer_);
    buffer_ = nullptr;
  }
  if (frame_in_) {
    av_frame_free(&frame_in_);
    frame_in_ = nullptr;
  }
  if (frame_out_) {
    av_frame_free(&frame_out_);
    frame_out_ = nullptr;
  }
  if (codec_ctx_) {
    avcodec_close(codec_ctx_);
    codec_ctx_ = nullptr;
  }
  if (format_ctx_) {
    avformat_close_input(&format_ctx_);
    format_ctx_ = nullptr;
  }
}

double VideoDecoder::GetPts() {
  return av_q2d(format_ctx_->streams[video_stream_index_]->time_base) *
         frame_in_->pts;
}

double VideoDecoder::GetFrameRate() {
  auto stream = format_ctx_->streams[video_stream_index_];
  auto r_frame_rate = stream->r_frame_rate;
  auto avg_frame_rate = stream->avg_frame_rate;
  auto codec_frame_rate = codec_ctx_->framerate;
  if (avg_frame_rate.num > 0 && avg_frame_rate.den > 0 &&
      r_frame_rate.num > 0 && r_frame_rate.den > 0 &&
      av_q2d(avg_frame_rate) < 70 && av_q2d(r_frame_rate) > 210) {
    r_frame_rate = avg_frame_rate;
  }
  if (codec_ctx_->ticks_per_frame > 1) {
    if (codec_frame_rate.num > 0 && codec_frame_rate.den > 0 &&
        (r_frame_rate.num == 0 ||
         (av_q2d(codec_frame_rate) < av_q2d(r_frame_rate) * 0.7 &&
          std::fabs(1.0 - av_q2d(av_div_q(avg_frame_rate, r_frame_rate))) >
              0.1))) {
      r_frame_rate = codec_frame_rate;
    }
  }
  return av_q2d(r_frame_rate);
}

int VideoDecoder::GetWidth() {
  return format_ctx_->streams[video_stream_index_]->codec->width;
}

int VideoDecoder::GetHeight() {
  return format_ctx_->streams[video_stream_index_]->codec->height;
}

uint8_t *VideoDecoder::GetData() { return frame_out_->data[0]; }

} // namespace mofu
