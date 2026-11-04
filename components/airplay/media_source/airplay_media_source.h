#pragma once

#include "esphome/core/defines.h"

#ifdef USE_ESP32

#include "esphome/components/airplay/airplay_hub.h"
#include "esphome/components/audio/audio.h"
#include "esphome/components/media_source/media_source.h"
#include "esphome/core/hal.h"

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <string>

namespace esphome::airplay_ {

/// Feeds the audio of the AirPlay hub to a media player and passes the player's commands to the sender.
class AirPlayMediaSource final : public AirPlayChild, public media_source::MediaSource {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;

  // MediaSource interface
  bool play_uri(const std::string &uri) override;
  void handle_command(media_source::MediaSourceCommand command) override;
  bool can_handle(const std::string &uri) const override;
  // Next and previous are forwarded to the sender
  bool has_internal_playlist() const override { return true; }
  void notify_volume_changed(float volume) override;
  void notify_audio_played(uint32_t frames, int64_t timestamp) override;

  // ===== From the hub, on the AirPlay task =====
  /// Writes interleaved 16-bit stereo audio; nothing unless this source is the active one
  size_t write_audio(const uint8_t *data, size_t length) {
    return this->write_output(data, length, 0, this->stream_info_);
  }
  void on_session_changed() { this->enable_loop_soon_any_context(); }
  /// The session ended; a sender that takes over next starts with the speaker's volume
  void on_session_ended() { this->hold_sender_volume_(SENDER_VOLUME_HOLD_MS); }
  // The speaker's volume wins: the sender's volume is followed only while this source has the speaker, and not in the
  // first moments after it got it, when senders send the volume they start with
  void on_sender_volume(float volume) {
    if (!this->follows_sender_())
      return;
    this->sender_volume_.store(volume, std::memory_order_relaxed);
    this->request_volume_(volume);
  }
  void on_sender_mute(bool muted) {
    if (this->follows_sender_())
      this->request_mute_(muted);
  }

  /// After this source gets the speaker, the sender's volume is not followed for this long: senders send the volume
  /// they start with, and the speaker's volume is sent to them instead
  static constexpr uint32_t SENDER_VOLUME_HOLD_MS = 2000;
  /// While the volume is changed on the device, and this long after, the sender's volume is not followed. Music
  /// Assistant sends the volume back over AirPlay with a delay, which made the volume jump while the knob turned.
  static constexpr uint32_t LOCAL_VOLUME_HOLD_MS = 1000;

 protected:
  /// THREAD CONTEXT: any
  bool follows_sender_() const;
  /// Does not follow the sender's volume for `duration_ms` from now, or longer if already so. THREAD CONTEXT: any
  void hold_sender_volume_(uint32_t duration_ms);

  /// Whether the AirPlay sender may have the speaker
  enum class SpeakerClaim : uint8_t {
    /// It may: its session takes the speaker when it starts playing. Also when no sender is connected.
    SPEAKER_CLAIM_OWNER,
    /// Another source took the speaker and the sender is paused: it takes the speaker back when it plays again
    SPEAKER_CLAIM_WAITING,
    /// Another source took the speaker and the sender has not paused, or was disconnected for it. It takes the
    /// speaker back only after it paused and plays again; connecting again by itself does not count.
    SPEAKER_CLAIM_DECLINED,
  };

  void set_claim_(SpeakerClaim claim);
  /// Takes the speaker away from the sender and pauses it, or disconnects a sender that cannot be paused
  void decline_sender_();
  void check_pause_();

  audio::AudioStreamInfo stream_info_{16, ::airplay::NUM_CHANNELS, ::airplay::SAMPLE_RATE};
  bool play_requested_{false};
  uint8_t play_requests_{0};  // Since the sender last got the speaker or the media player last took it
  SpeakerClaim claim_{SpeakerClaim::SPEAKER_CLAIM_OWNER};
  // The sender that lost the speaker; any other sender that starts playing takes it
  uint32_t declined_address_{0};
  // When a declined sender disconnected; 0 while it is connected
  uint32_t declined_end_ms_{0};
  // The sender's volume is followed from this time on; set when this source gets the speaker
  std::atomic<uint32_t> follow_sender_from_ms_{0};
  std::atomic<float> sender_volume_{-1.0f};  // The volume last taken from the sender
  uint32_t volume_address_{0};               // The sender that was last given the speaker's volume; main loop only
};

}  // namespace esphome::airplay_

#endif  // USE_ESP32
