# Changelog

## 2026-07-04 - Remove On-Screen Version Debug (v2026.7.4.10)

- Removed the visible version/debug text segment from the active showcase loop.
- Replaced the first segment with a non-text visual segment.
- Bumped deploy version to `2026.7.4.10`.

## 2026-07-04 - Troop Salute Segment (v2026.7.4.9)

- Removed the Citadel-themed segment from the showcase loop.
- Added a troop-salute segment with:
  - scrolling "SALUTE HERO" text
  - ceremonial red/white/blue marching stripes with star markers
- Added needed text glyphs (`E`, `L`, `O`, `R`) for the new salute phrase.
- Bumped deploy version to `2026.7.4.9`.

## 2026-07-04 - Rainbow Removal (v2026.7.4.8)

- Removed rainbow-style visuals from the active single-board showcase loop.
- Replaced the dedicated rainbow segment with a non-rainbow visual segment.
- Changed confetti colors to patriotic red/white/blue only.
- Bumped deploy version to `2026.7.4.8`.

## 2026-07-04 - CoV Theme Refinement (v2026.7.4.7)

- Refined the horizontal Citadel of Void segment with a darker CoV-style palette and stronger void atmosphere.
- Added moving hazard rails (hot pink/red), cyan-indigo void windows, and a horizontal nil-like pulse.
- Bumped deploy version to `2026.7.4.7`.

## 2026-07-04 - Void Citadel Pattern (v2026.7.4.6)

- Added a new horizontal "Citadel of Void" inspired visual segment to the single-board showcase.
- Replaced the former center-burst segment with the new void-citadel motion pattern.
- Bumped deploy version to `2026.7.4.6`.

## 2026-07-04 - Fireworks Boost (v2026.7.4.5)

- Removed the solid color-wipe segment from the single-board showcase loop.
- Replaced that segment with an additional high-intensity fireworks section.
- Bumped deploy version to `2026.7.4.5`.

## 2026-07-04 - Single Board 20-Pattern Showcase (v2026.7.4.4)

- Switched active panel back to the verified first matrix (`PanelProfile::Bottom`).
- Added a 20-segment debug showcase optimized for tonight:
  - 4 text segments (`version`, `USA`, `1776`, `HAPPY`, `4TH`)
  - 15 visual segments (twinkle, waves, fireworks, strobe, sweeps, scanners, wipes, confetti, and more)
- Disabled raw-chain diagnostic mode for this run.
- Enabled debug showcase mode for immediate upload/use.
- Bumped deploy version to `2026.7.4.4`.

## 2026-07-04 - Second Matrix Raw Chain Test (v2026.7.4.3)

- Added `kRunRawChainTest` mode to bypass XY mapping and test raw LED chain integrity.
- Enabled raw chain test for the next upload to diagnose the second panel showing only one dot.
- Disabled corner sequence for this diagnostic pass.
- Bumped deploy version to `2026.7.4.3`.

## 2026-07-04 - Top Panel Corner Test Prep (v2026.7.4.2)

- Added selectable panel profiles so bottom and top matrix mappings can be tracked separately.
- Kept the verified bottom panel mapping as column-major.
- Set the active target panel to the top matrix for the next upload.
- Re-enabled the simple corner-sequence test for the next top-panel verification pass.
- Bumped the deploy version to `2026.7.4.2`.

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
