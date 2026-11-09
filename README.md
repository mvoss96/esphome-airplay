# ESPHome AirPlay

Turns an ESPHome device into an AirPlay speaker. iPhone, iPad, Mac, Music Assistant and other AirPlay senders
find it in their speaker list and stream to it, AirPlay 1 or AirPlay 2. It is an external component for the
stable ESPHome release, built on [airplay-cpp](https://github.com/mvoss96/airplay-cpp).

## What it does

AirPlay is one source of ESPHome's `speaker_source` media player, next to web radio, Sendspin or anything else.
The source that started last gets the speaker. When another source takes over, the AirPlay sender is paused,
not disconnected: it stays connected and gets the speaker back as soon as it plays again, so the speaker never
has to be selected again on the phone.

Playback follows the sender's clock, so sound stays in sync with video and with other AirPlay 2 speakers in a
group (within a few ms of a HomePod). Volume works both ways, and the media player's play, pause, next and
previous reach the sender. Track title, artist, album, position, the name of the sending device and the cover art
are available as sensors and as an image for LVGL or any display.

The Home app can add the speaker (room, groups, Siri), and the setup survives restarts. Its tile controls playback
without Apple's MFi authentication, also on an iPad or another device that is not the one playing, and shows the
title and cover, also while a Mac or Music Assistant plays. With "Only people sharing this home", other devices
and Music Assistant are refused, as by a HomePod; with "Require password", senders ask for that password.

## Usage

```yaml
external_components:
  - source: github://mvoss96/esphome-airplay@v0.8.0
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
| `ptp` | with `airplay2` | AirPlay 2 timing with PTP, which macOS needs; uses UDP ports 319 and 320 |
| `password` | none | AirPlay 1 only |
| `remote_control` | with `airplay2` | The Home app's tile and Control Center control the sender through the speaker; takes 4 more sockets |
| `remote_cover_size` | `262144` | Largest cover kept in PSRAM for the tile while a Mac or Music Assistant plays; `0` for none |
| `homekit_identity` | `0` | Another number makes the speaker a new accessory for the Home app, e.g. when a home cannot add it again |
| `task_stack_in_psram` | `false` | Saves 8 KB of internal RAM |

`media_source` takes `buffer_duration` (default 2.5 s with PSRAM, 500 ms without) and `output_queue`: how much
audio is written ahead to the speaker, default 250 ms. More rides out a busy CPU; the speaker's buffer must hold
it.

## Hardware

ESP32 boards with Wi-Fi and the ESP-IDF framework. Tested on the ESP32-S3 with PSRAM; the plain ESP32, the
ESP32-C3 and the ESP32-C6 are built in CI. PSRAM is strongly recommended: synchronized playback buffers about
2.5 s of audio (460 KB), and AirPlay 2 adds 128 KB for the buffered audio that iOS apps (AAC) and Music Assistant
(ALAC) send. Without PSRAM AirPlay 1 still works, unsynchronized and with a short buffer.

Set `wifi: power_save_mode: none`. The component turns power saving off while it plays, but pairing, the Home
app's tile and Music Assistant's remote control also need a fast network: with power saving, pings take 300 ms or
more, and pairing or a pause can take seconds.

## Limitations

- The `password` option works with AirPlay 1 only; for AirPlay 2, set "Require password" in the Home app
- Music Assistant ignores AirPlay 2 pause requests from speakers other than Apple's. The component also sends
  the pause through the older DACP remote control, and disconnects a sender that still keeps playing

## License

MIT
