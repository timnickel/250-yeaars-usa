# Light Matrix - 4th of July

This repository now contains a working starter implementation for a 32x8 WS2812/NeoPixel July 4th matrix show, built from the original project plan.

## Project Structure

```text
250-yeaars-usa/
  july4-lightmatrix.ino
  scenes.h
  scenes.cpp
  hardware_config.h
  assets/
    sprites.h
  notes/
    wiring.md
```

## What Is Implemented

1. Phase 0 bring-up mode for mapping and power validation.
2. Phase 1 effects:
   - Twinkle stars
   - Red/white/blue stripe wave
   - Scrolling text ("HAPPY 4TH USA")
3. Phase 2 mini show scene engine with deterministic durations:
   - Intro scroll
   - Twinkle
   - Stripe wave
   - Fireworks
   - Finale
4. Brightness profiles in configuration.

## Hardware Assumptions

- Arduino 5V logic board (Uno/Nano/Yun-class)
- 32x8 WS2812 matrix
- External 5V supply for the matrix

## Wiring Summary

1. D6 data pin -> 330 ohm resistor -> matrix DIN
2. External 5V PSU -> matrix +5V and GND
3. Arduino GND -> matrix GND
4. Arduino GND -> PSU GND (shared ground)
5. 1000 uF capacitor across +5V and GND near matrix input

See [notes/wiring.md](notes/wiring.md) for details and troubleshooting.
For multiple board-specific wiring diagrams, see [notes/wiring-options.md](notes/wiring-options.md).

## Build / Run

1. Open this folder in Arduino IDE or PlatformIO.
2. Install `Adafruit NeoPixel` library if not already installed.
3. Open [july4-lightmatrix.ino](july4-lightmatrix.ino).
4. Verify board/port settings and upload.

## Configuration

Use [hardware_config.h](hardware_config.h) to adjust:

- Matrix pin and dimensions
- Serpentine/row-major mapping options
- Indoor/outdoor brightness presets
- Bring-up test mode

Set `kRunBringUpTest = true` for initial validation. Set it back to `false` to run the full scene show.

## Next Backlog

- Add mode button support (short/long press)
- Add countdown trigger scene
- Add web command layer
- Add additional sprite-based patriotic scenes
