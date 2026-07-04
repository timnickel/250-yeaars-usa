# Copilot Instructions

## Summary

This repository contains a July 4th WS2812 / NeoPixel LED matrix project for an Arduino Yun-class setup, plus supporting wiring and planning notes. The working project source in the repo is under `july4-lightmatrix/`, and the currently uploadable Arduino IDE sketch copy was also synced to the local sketchbook at `C:\Users\timjn\ArduinoSketchbook\usa_matrix_show`.

The project currently includes:
- A scene-based holiday light show with scrolling text, twinkle, stripe wave, fireworks, and finale effects.
- Multiple hardware and wiring notes covering Arduino board options, resistor guidance, capacitor placement and polarity, and external power recommendations.
- Dedicated matrix verification modes for bring-up and orientation debugging.
- A debug showcase mode that starts with a deployed version string and then runs five curated patriotic/debug patterns.

When continuing work, preserve the iterative workflow already established: first verify mapping/orientation with simple tests, then restore the main show once the matrix layout is confirmed.

## Repository Structure

- `README.md`: high-level project overview.
- `july4-lightmatrix/`: main source tree for the matrix project.
- `july4-lightmatrix/hardware_config.h`: board pin, matrix dimensions, mapping flags, brightness, and test-mode toggles.
- `july4-lightmatrix/scenes.h`: scene/test function declarations.
- `july4-lightmatrix/scenes.cpp`: show scenes plus bring-up/orientation test implementations.
- `july4-lightmatrix/july4-lightmatrix/july4-lightmatrix.ino`: current repo copy of the main Arduino sketch entrypoint.
- `notes/wiring.md`: concise wiring and protection notes.
- `notes/wiring-options.md`: multiple wiring diagrams and board-specific guidance.
- `notes/advanced-show-ideas.md`: ideas for one-panel and two-panel advanced shows.
- `enhancement-ideas.md`: additional project ideas already present in the repo.

## Current Arduino IDE / Upload Workflow

Use Arduino IDE for uploads unless the user explicitly asks to switch tools.

Current stable upload path:
- Sketchbook location: `C:\Users\timjn\ArduinoSketchbook`
- Uploadable sketch folder: `C:\Users\timjn\ArduinoSketchbook\usa_matrix_show`
- Main uploadable sketch: `C:\Users\timjn\ArduinoSketchbook\usa_matrix_show\usa_matrix_show.ino`

Important history:
- The user had an older Arduino IDE and upgraded to a newer version.
- OneDrive-based sketchbook paths caused confusion and sketch naming issues.
- The Arduino CLI config was changed away from OneDrive to the local sketchbook path above.
- The Adafruit NeoPixel library must be installed in the Arduino IDE environment.

When code changes affect the sketch, keep the Arduino sketchbook copy in sync if the user is uploading from Arduino IDE.
Whenever a sketch is prepared for deployment, update the debug/deploy version string in `july4-lightmatrix/hardware_config.h` before syncing the uploadable sketch copy.

## Deployment Checklist

Use this exact checklist before each upload:

1. Open Arduino IDE 2.x.
2. Confirm Sketchbook location is `C:\Users\timjn\ArduinoSketchbook`.
3. Open `C:\Users\timjn\ArduinoSketchbook\usa_matrix_show\usa_matrix_show.ino`.
4. Confirm the `Adafruit NeoPixel` library is installed.
5. Select the Arduino Yun board and the correct COM port.
6. Verify external 5V matrix power is connected and Arduino/matrix grounds are common.
7. Upload the sketch.
8. If the deployment is a debug/showcase build, increment the version string in `kDebugVersion` before upload.
9. Sync the repo sketch changes into `C:\Users\timjn\ArduinoSketchbook\usa_matrix_show` before uploading from Arduino IDE.
10. Use the active test/debug mode intended for that deploy.

## Hardware Decisions and Wiring Guidance

Current target hardware assumptions:
- Arduino Yun or similar 5V logic Arduino board.
- 32x8 WS2812 / NeoPixel matrix.
- External 5V power supply for the matrix.

Wiring guidance already established:
- Data line uses a series resistor in the 330 to 470 ohm range.
- The resistor is non-polarized, so orientation does not matter.
- A 1000 uF electrolytic capacitor should be connected across matrix `+5V` and `GND` near the matrix power input.
- Capacitor polarity matters: positive to `+5V`, negative to `GND`.
- The capacitor does not plug into a special Arduino port; it sits across the matrix power rails.
- Always use a common ground between Arduino and the matrix power supply.
- Do not power the full matrix from the Arduino 5V pin.

## Existing Test / Debug Modes

These toggles live in `july4-lightmatrix/hardware_config.h`.

- `kRunBringUpTest`: simple color/checker/scan test for initial matrix verification.
- `kRunCornerOrientationTest`: four distinct lit corners plus a perimeter tracer.
- `kRunCornerSequenceTest`: one-corner-at-a-time sequence for easiest orientation checking.
- `kRunDebugShowcase`: deployed debug showcase with version/date screen and curated pattern loop.

Current status:
- `kMatrixRowMajor = false` is the current correct mapping assumption.
- `kRunDebugShowcase` is enabled for the current deployed build.
- Current deploy version string: `2026.7.4.1`
- The debug showcase currently presents:
  1. version/date scroll
  2. pulsing `USA`
  3. scrolling `1776`
  4. flag wave
  5. star sweep
  6. sparkler into fireworks

Use the simpler orientation modes again if future panel changes require remapping.

## Current Main Show Behavior

The full scene show currently includes:
1. Intro scrolling text
2. Twinkle scene
3. Red-white-blue stripe wave
4. Fireworks scene
5. Finale scene

Important context from user testing:
- The stripe wave was visible and recognizable.
- Corner testing confirmed the matrix should be treated as column-major (`kMatrixRowMajor = false`).
- The latest debug showcase uploaded successfully and appears to run correctly on the matrix.
- The next goal after this documentation pass is planning a second matrix stacked above the first.

## Dual-Panel / Future Direction

Ideas already documented and should be preserved:
- Side-by-side dual panel panorama mode.
- Stacked vertical mode.
- Master/slave Yun sync over WiFi.
- Deterministic seeded effects across two controllers.
- Expanded July 4th choreography and finale concepts.

See `notes/advanced-show-ideas.md` for those details.

## Working Style for Future Changes

- Prefer small, testable changes.
- For matrix issues, first use simple deterministic test patterns before changing show logic.
- Keep documentation aligned with actual behavior and wiring recommendations.
- If editing the sketch in the repo, remember that the Arduino IDE upload copy may also need updating.
- Avoid reintroducing OneDrive sketchbook paths unless the user explicitly requests that setup.
- When a task is complete and intended to be deployed/shared, include the git step: update the deploy version if applicable, commit the relevant changes, and push to the intended remote.
- If the requested remote is not configured (for example GitLab is requested but only GitHub is configured), treat that as a delivery blocker and surface it explicitly.
