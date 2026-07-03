# Wiring Notes (32x8 WS2812 Matrix)

## Core Wiring

1. Arduino data pin D6 -> 330 ohm resistor -> matrix DIN.
2. External 5V PSU + -> matrix +5V.
3. External 5V PSU - -> matrix GND.
4. Arduino GND -> matrix GND.
5. Arduino GND -> PSU GND (common ground).

## Protection Parts

- 330 ohm series resistor on data line.
- 1000 uF electrolytic capacitor across +5V and GND near the matrix input terminals.
- The capacitor does not connect to the Arduino/Yun board; it connects to the matrix power rails.

## Bring-Up Safety

1. Start with low brightness (`kBrightnessIndoor` in `hardware_config.h`).
2. Enable bring-up mode by setting `kRunBringUpTest = true`.
3. Validate orientation and serpentine mapping before running full scenes.
4. Do not power the matrix from the Arduino 5V pin.

## Troubleshooting

- Random flicker usually means grounding or power quality issues.
- If orientation is wrong, adjust `kMatrixSerpentine` or `kMatrixRowMajor` in `hardware_config.h`.
- If animation appears mirrored, validate physical panel orientation and update XY mapping settings.
