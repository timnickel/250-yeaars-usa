#pragma once

#include <Arduino.h>

// Matrix hardware settings
constexpr uint8_t kMatrixPin = 6;
constexpr uint8_t kMatrixWidth = 32;
constexpr uint8_t kMatrixHeight = 8;
constexpr bool kMatrixSerpentine = true;
constexpr bool kMatrixRowMajor = true;
constexpr uint16_t kNumPixels = kMatrixWidth * kMatrixHeight;

// NeoPixel color order and speed for WS2812
constexpr neoPixelType kPixelType = NEO_GRB + NEO_KHZ800;

// Brightness profiles
constexpr uint8_t kBrightnessIndoor = 24;
constexpr uint8_t kBrightnessOutdoor = 64;
constexpr uint8_t kDefaultBrightness = kBrightnessIndoor;

// Set true for Phase 0 mapping/power bring-up tests.
constexpr bool kRunBringUpTest = false;

// Animation pacing
constexpr uint16_t kFrameMs = 33;  // ~30 FPS
