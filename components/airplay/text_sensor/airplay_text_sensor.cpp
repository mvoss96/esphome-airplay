#include "airplay_text_sensor.h"

#if defined(USE_ESP32) && defined(USE_AIRPLAY_METADATA)

#include "esphome/core/log.h"

namespace esphome::airplay_ {

ESPHOME_LOG_TAG(TAG, "airplay.text_sensor");

void AirPlayTextSensor::setup() {
  if (this->metadata_type_ == AirPlayMetadataType::AIRPLAY_METADATA_TYPE_SENDER) {
    this->parent_->add_sender_name_callback([this](const char *name) {
      if (this->state != name)
        this->publish_state(name);
    });
    return;
  }
  this->parent_->add_metadata_callback([this](const ::airplay::Metadata &metadata) {
    const char *value;
    switch (this->metadata_type_) {
      case AirPlayMetadataType::AIRPLAY_METADATA_TYPE_ARTIST:
        value = metadata.artist;
        break;
      case AirPlayMetadataType::AIRPLAY_METADATA_TYPE_ALBUM:
        value = metadata.album;
        break;
      case AirPlayMetadataType::AIRPLAY_METADATA_TYPE_ALBUM_ARTIST:
        value = metadata.album_artist;
        break;
      default:
        value = metadata.title;
        break;
    }
    if (this->state != value)
      this->publish_state(value);
  });
}

void AirPlayTextSensor::dump_config() { LOG_TEXT_SENSOR("", "AirPlay Text Sensor", this); }

}  // namespace esphome::airplay_

#endif  // USE_ESP32 && USE_AIRPLAY_METADATA
