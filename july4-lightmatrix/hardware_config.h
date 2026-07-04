#pragma once

#include <Arduino.h>

// Matrix hardware settings
constexpr uint8_t kMatrixPin = 6;
constexpr uint8_t kMatrixWidth = 32;
constexpr uint8_t kMatrixHeight = 8;
constexpr bool kMatrixSerpentine = true;
constexpr bool kMatrixRowMajor = false;
constexpr uint16_t kNumPixels = kMatrixWidth * kMatrixHeight;

// NeoPixel color order and speed for WS2812
constexpr neoPixelType kPixelType = NEO_GRB + NEO_KHZ800;

// Brightness profiles
constexpr uint8_t kBrightnessIndoor = 24;
constexpr uint8_t kBrightnessOutdoor = 64;
constexpr uint8_t kDefaultBrightness = kBrightnessIndoor;

// Set true for Phase 0 mapping/power bring-up tests.
constexpr bool kRunBringUpTest = false;

// Set true for a simple orientation test with colored corners and a perimeter tracer.
constexpr bool kRunCornerOrientationTest = false;

// Set true for an even simpler corner-by-corner test sequence.
constexpr bool kRunCornerSequenceTest = false;

// Set true to run the debug showcase sequence instead of the full show.
constexpr bool kRunDebugShowcase = true;

// Update this string whenever a meaningful debug build is prepared.
constexpr char kDebugVersion[] = "2026.7.4.1";

// Animation pacing
constexpr uint16_t kFrameMs = 33;  // ~30 FPS
