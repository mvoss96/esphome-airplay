# ESPHome AirPlay

An AirPlay speaker for ESPHome, as an external component. iPhone, iPad, Mac, Music Assistant and other AirPlay
senders can stream to an ESP32 that runs it. Built on [airplay-cpp](https://github.com/mvoss96/airplay-cpp).

- AirPlay 1 and, optionally, AirPlay 2 (including the buffered AAC audio most iOS apps send)
- Playback synchronized to the sender's clock
- Plays through the `speaker_source` media player, next to other media sources such as web radio
- Volume both ways, play/pause/next/previous to the sender
- Track details, track position and cover art (for LVGL or any display)
- Another source on the speaker pauses the sender, which stays connected and resumes here when it plays again

Works with ESPHome 2026.10.1 or newer on ESP32 boards with ESP-IDF. AirPlay 2 and cover art are meant for boards with PSRAM.

## Installation

```yaml
external_components:
  - source: github://mvoss96/esphome-airplay@main
    components: [airplay]
```

Pin a commit instead of `main` for builds that must not change by themselves.

## Configuration

```yaml
airplay:
  name: Kitchen         # Defaults to the device's friendly name
  airplay2: true        # Cannot be used with a password
  task_stack_in_psram: true

media_source:
  - platform: airplay
    id: airplay_source

media_player:
  - platform: speaker_source
    name: Kitchen
    media_pipeline:
      speaker: my_speaker
      sources:
        - airplay_source
```

### `airplay`

| Option | Default | |
|---|---|---|
| `name` | friendly name | Name shown to senders, at most 48 characters |
| `model` | `ESPHome` | Model shown to senders |
| `password` | none | Asked for by senders; AirPlay 1 only |
| `port` | `5000` | RTSP port |
| `airplay2` | `false` | Offer AirPlay 2 |
| `aac_buffer_size` | `131072` | Bytes for buffered AirPlay 2 audio |
| `task_stack_in_psram` | `false` | Put the AirPlay task's stack in PSRAM |

Needs `mdns` (not disabled) and `wifi` or `ethernet`.

### `media_source` platform `airplay`

| Option | Default | |
|---|---|---|
| `buffer_duration` | 2500 ms with PSRAM, else 500 ms | Audio buffer, 100 ms to 5 s; synchronized playback needs about 2 s |

### Track details

```yaml
text_sensor:
  - platform: airplay
    type: title          # title, artist, album, album_artist
    name: Title

sensor:
  - platform: airplay
    type: track_progress # track_progress, track_duration, year, track
    name: Position
```

### Cover art

```yaml
image:
  - platform: airplay
    id: cover
    type: RGB565
    resize: 120x120      # Scaled down to fit, keeping the aspect ratio
    on_image_update:
      - lvgl.image.update:
          id: cover_widget
          src: cover
    on_image_clear:
      - lvgl.widget.hide: cover_widget
```

`format` is `AUTO` (JPEG and PNG), `JPEG` or `PNG`.

### Actions

`airplay.play`, `airplay.pause`, `airplay.next` and `airplay.previous` control the sender; each takes the
`airplay` id if there is more than one.

## Development

`tests/` holds the configurations CI compiles against the stable ESPHome release named in
`.github/workflows/ci.yml`. CI also runs the interop tests of the airplay-cpp version pinned in
`components/airplay/__init__.py`, the only place that names it.

```sh
pip install esphome==2026.10.1
esphome compile tests/airplay2.yaml
```

## License

MIT
