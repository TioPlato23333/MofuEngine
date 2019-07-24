/* Created by Xiaodong Ning (ningxiaodong23333@gmail.com)
 *
 * Copyright (c) 2019 Xiaodong Ning. All rights reserved.
 */

#include "audio/audio.h"
#include "core/log.h"

namespace {

constexpr uint64_t kTargetChannelLayout = AV_CH_LAYOUT_MONO;
constexpr AVSampleFormat kTargetFormat = AV_SAMPLE_FMT_S16;

} // namespace

namespace mofu {

AudioDecoder::AudioDecoder()
    : filename_(""), format_ctx_(nullptr), codec_ctx_(nullptr), codec_(nullptr),
      swr_ctx_(nullptr), frame_in_(nullptr), frame_out_(nullptr),
      audio_stream_index_(-1), buffer_(nullptr) {}

AudioDecoder::~AudioDecoder() { Close(); }

bool AudioDecoder::Open(const std::string &filename) {
  filename_ = filename;
  if (avformat_open_input(&format_ctx_, filename_.c_str(), nullptr, nullptr)) {
    LOGE("Audio decoder avformat_open_input failed in %s.", filename_.c_str());
    return false;
  }
  if (avformat_find_stream_info(format_ctx_, nullptr)) {
    LOGE("Audio decoder avformat_find_stream_info failed in %s.",
         filename_.c_str());
    return false;
  }
  audio_stream_index_ = -1;
  for (int i = 0; i < format_ctx_->nb_streams; ++i) {
    if (format_ctx_->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
      audio_stream_index_ = i;
      break;
    }
  }
  if (audio_stream_index_ < 0) {
    LOGE("Audio decoder find video stream index failed in %s.",
         filename_.c_str());
    return false;
  }
  codec_ctx_ = format_ctx_->streams[audio_stream_index_]->codec;
  codec_ = avcodec_find_decoder(codec_ctx_->codec_id);
  if (!codec_) {
    LOGE("Audio decoder avcodec_find_decoder failed in %s.", filename_.c_str());
    return false;
  }
  if (avcodec_open2(codec_ctx_, codec_, nullptr)) {
    LOGE("Audio decoder avcodec_open2 failed in %s.", filename_.c_str());
    return false;
  }
  frame_in_ = av_frame_alloc();
  frame_out_ = av_frame_alloc();
  if (!frame_in_ || !frame_out_) {
    LOGE("Audio decoder av_frame_alloc failed in %s.", filename_.c_str());
    return false;
  }
  frame_out_->nb_samples = codec_ctx_->frame_size;
  int nb_channels = av_get_channel_layout_nb_channels(kTargetChannelLayout);
  int buffer_size = av_samples_get_buffer_size(
      nullptr, nb_channels, frame_out_->nb_samples, kTargetFormat, 0);
  if (buffer_size <= 0) {
    LOGE("Audio decoder av_samples_get_buffer_size failed in %s.",
         filename_.c_str());
    return false;
  }
  buffer_ = static_cast<uint8_t *>(av_malloc(buffer_size));
  if (!buffer_) {
    LOGE("Audio decoder av_malloc failed in %s.", filename_.c_str());
    return false;
  }
  if (avcodec_fill_audio_frame(frame_out_, nb_channels, kTargetFormat, buffer_,
                               buffer_size, 0) < 0) {
    LOGE("Audio decoder avcodec_fill_audio_frame failed in %s.",
         filename_.c_str());
    return false;
  }
  swr_ctx_ = swr_alloc_set_opts(
      nullptr, kTargetChannelLayout, kTargetFormat, codec_ctx_->sample_rate,
      codec_ctx_->channel_layout, codec_ctx_->sample_fmt,
      codec_ctx_->sample_rate, 0, nullptr);
  if (!swr_ctx_) {
    LOGE("Audio decoder swr_alloc_set_opts failed in %s.", filename_.c_str());
    return false;
  }
  if (swr_init(swr_ctx_)) {
    LOGE("Audio decoder swr_init failed in %s.", filename_.c_str());
    return false;
  }
  return true;
}

bool AudioDecoder::ReadFrame(ErrorInfo *error) {
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
    if (packet.stream_index == audio_stream_index_) {
      if (avcodec_decode_audio4(codec_ctx_, frame_in_, &got_frame, &packet) <
          0) {
        if (error) {
          *error = ErrorInfo(ErrorInfo::kDecoderError, "DecoderError.");
        }
        return false;
      }
    }
    av_free_packet(&packet);
    if (got_frame) {
      if (swr_convert(swr_ctx_, frame_out_->data, frame_out_->nb_samples,
                      (const uint8_t **)frame_in_->data,
                      frame_in_->nb_samples) < 0) {
        if (error) {
          *error = ErrorInfo(ErrorInfo::kDecoderError, "DecoderError.");
        }
        return false;
      }
      if (error) {
        *error = ErrorInfo(ErrorInfo::kOk, "Ok.");
      }
      return true;
    }
  }
}

void AudioDecoder::Close() {
  if (swr_ctx_) {
    swr_free(&swr_ctx_);
    swr_ctx_ = nullptr;
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

uint8_t *AudioDecoder::GetData() { return frame_out_->data[0]; }

} // namespace mofu
