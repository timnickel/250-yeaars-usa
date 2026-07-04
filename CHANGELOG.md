# Changelog

## 2026-07-04 - Debug Showcase Deploy (v2026.7.4.1)

- Confirmed the matrix orientation using corner tests.
- Updated matrix mapping to column-major by setting `kMatrixRowMajor = false`.
- Added and validated orientation/debug test modes:
  - bring-up test
  - corner orientation test
  - corner sequence test
- Added a debug showcase deployment mode with:
  - version/date scroll (`2026.7.4.1`)
  - pulsing `USA`
  - scrolling `1776`
  - flag wave
  - star sweep
  - sparkler into fireworks
- Synced the active uploadable sketch to `C:\Users\timjn\ArduinoSketchbook\usa_matrix_show`.

## 2026-07-03 - Wiring, IDE, and Upload Stabilization

- Created project wiring documentation and board-specific wiring options.
- Clarified capacitor placement, polarity, and purpose.
- Clarified resistor range (`330` to `470` ohm) and noted that resistor orientation does not matter.
- Moved Arduino sketchbook workflow away from OneDrive to reduce Arduino IDE path issues.
- Established Arduino IDE upload path at `C:\Users\timjn\ArduinoSketchbook\usa_matrix_show`.
- Documented the requirement to install the `Adafruit NeoPixel` library in Arduino IDE.

## 2026-07-02 - Initial Project Scaffold

- Built the initial July 4th LED matrix project from the README plan.
- Added core sketch, hardware config, scene engine, sprite asset file, and wiring notes.
- Implemented the first main show sequence:
  - intro scroll
  - twinkle
  - stripe wave
  - fireworks
  - finale
- Added notes for future dual-panel / advanced show ideas.

## Ongoing Rules

- Always bump the deployed version string in `kDebugVersion` before syncing and uploading a new deployable debug build.
- Keep the Arduino sketchbook copy in sync whenever the repo sketch changes are intended for upload.
