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

## Why The Capacitor Is Needed

The capacitor acts like a small local energy buffer. When the matrix changes brightness quickly, it helps absorb short power dips and reduces flicker, resets, and startup glitches. It does not replace the external 5V supply; it just smooths the power seen by the matrix.

## Capacitor Polarity

If the 1000 uF part is an electrolytic capacitor, polarity matters:

1. Capacitor positive lead to matrix +5V.
2. Capacitor negative lead to matrix GND.
3. The negative side is usually marked with a stripe on the body.
4. The negative lead is often the shorter leg.
5. Do not reverse it.

## Bring-Up Safety

1. Start with low brightness (`kBrightnessIndoor` in `hardware_config.h`).
2. Enable bring-up mode by setting `kRunBringUpTest = true`.
3. Validate orientation and serpentine mapping before running full scenes.
4. Do not power the matrix from the Arduino 5V pin.

## Troubleshooting

- Random flicker usually means grounding or power quality issues.
- If orientation is wrong, adjust `kMatrixSerpentine` or `kMatrixRowMajor` in `hardware_config.h`.
- If animation appears mirrored, validate physical panel orientation and update XY mapping settings.
