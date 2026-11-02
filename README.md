# ESPHome AirPlay

AirPlay 1 and 2 speaker for ESPHome (ESP32, ESP-IDF, ESPHome 2026.10.1 or newer). Built on
[airplay-cpp](https://github.com/mvoss96/airplay-cpp).

```yaml
external_components:
  - source: github://mvoss96/esphome-airplay@main
    components: [airplay]

airplay:
  airplay2: true  # Optional; best with PSRAM

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

Also available: `text_sensor` and `sensor` platforms for track details, an `image` platform for cover art, and
the actions `airplay.play`, `pause`, `next` and `previous`. See [tests/](tests) for complete examples.

License: MIT
