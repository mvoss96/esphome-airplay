# ESPHome AirPlay

Turns an ESPHome device into an AirPlay speaker. iPhone, iPad, Mac, Music Assistant and other AirPlay senders
find it in their speaker list and stream to it, AirPlay 1 or AirPlay 2. It is an external component for the
stable ESPHome release, built on [airplay-cpp](https://github.com/mvoss96/airplay-cpp).

## What it does

AirPlay is one source of ESPHome's `speaker_source` media player, next to web radio, Sendspin or anything else.
The source that started last gets the speaker. When another source takes over, the AirPlay sender is paused,
not disconnected: it stays connected and gets the speaker back as soon as it plays again, so the speaker never
has to be selected again on the phone.

Playback follows the sender's clock, so sound stays in sync with video. Volume works both ways, and the media
player's play, pause, next and previous reach the sender. Track title, artist, album, position, the name of the
sending device and the cover art are available as sensors and as an image for LVGL or any display.

## Usage

```yaml
external_components:
  - source: github://mvoss96/esphome-airplay@v0.1.0
    components: [airplay]

airplay:
  airplay2: true

media_source:
  - platform: airplay
    id: airplay_source

media_player:
  - platform: speaker_source
    name: Speaker
    media_pipeline:
      speaker: my_speaker
      sources: [airplay_source]
```

Use a tag, `main` or a full commit hash; ESPHome does not accept short hashes. [tests/](tests) has complete
configurations with all sensors, the cover art and the actions `airplay.play`, `pause`, `next` and `previous`.

| `airplay` option | Default | |
|---|---|---|
| `name` | friendly name | Name in the sender's speaker list |
| `airplay2` | `false` | Offer AirPlay 2 |
| `password` | none | AirPlay 1 only |
| `task_stack_in_psram` | `false` | Saves 8 KB of internal RAM |

`media_source` takes `buffer_duration` (default 2.5 s with PSRAM, 500 ms without) and `output_queue`: how much
audio is written ahead to the speaker, default 250 ms. More rides out a busy CPU; the speaker's buffer must hold it.

## Hardware

ESP32 boards with Wi-Fi and the ESP-IDF framework. Tested on the ESP32-S3 with PSRAM; the plain ESP32 is
built in CI. PSRAM is strongly recommended: synchronized playback buffers about 2.5 s of audio (460 KB), and
AirPlay 2 adds 128 KB for the AAC stream most iOS apps send. Without PSRAM AirPlay 1 still works, unsynchronized
and with a short buffer.

## Limitations

- 44.1 kHz, 16-bit stereo: no lossless high resolution, no video or screen mirroring
- AirPlay 2 groups with HomePods or other AirPlay 2 speakers are not supported, and the speaker cannot be added
  to the Home app
- A password works with AirPlay 1 only
- Music Assistant ignores AirPlay 2 pause requests from speakers other than Apple's. The component also sends
  the pause through the older DACP remote control, and disconnects a sender that still keeps playing

## License

MIT
