#pragma once

#include <Arduino.h>

// Matrix hardware settings
constexpr uint8_t kMatrixPin = 6;
constexpr uint8_t kMatrixWidth = 32;
constexpr uint8_t kMatrixHeight = 8;

enum class PanelProfile : uint8_t {
	Bottom,
	Top,
};

// Select which physical panel is connected for the next upload.
constexpr PanelProfile kActivePanelProfile = PanelProfile::Bottom;

// Bottom panel mapping (already verified).
constexpr bool kBottomMatrixSerpentine = true;
constexpr bool kBottomMatrixRowMajor = false;

// Top panel mapping defaults to the bottom panel until verified.
constexpr bool kTopMatrixSerpentine = true;
constexpr bool kTopMatrixRowMajor = false;

constexpr bool activeMatrixSerpentine() {
	return kActivePanelProfile == PanelProfile::Bottom ? kBottomMatrixSerpentine : kTopMatrixSerpentine;
}

constexpr bool activeMatrixRowMajor() {
	return kActivePanelProfile == PanelProfile::Bottom ? kBottomMatrixRowMajor : kTopMatrixRowMajor;
}

constexpr uint16_t kNumPixels = kMatrixWidth * kMatrixHeight;

// NeoPixel color order and speed for WS2812
constexpr neoPixelType kPixelType = NEO_GRB + NEO_KHZ800;

// Brightness profiles
constexpr uint8_t kBrightnessIndoor = 24;
constexpr uint8_t kBrightnessOutdoor = 64;
constexpr uint8_t kDefaultBrightness = kBrightnessIndoor;

// Set true for Phase 0 mapping/power bring-up tests.
constexpr bool kRunBringUpTest = false;

// Set true for a raw index-chain test (bypasses XY mapping).
constexpr bool kRunRawChainTest = false;

// Set true for a simple orientation test with colored corners and a perimeter tracer.
constexpr bool kRunCornerOrientationTest = false;

// Set true for an even simpler corner-by-corner test sequence.
constexpr bool kRunCornerSequenceTest = false;

// Set true to run the debug showcase sequence instead of the full show.
constexpr bool kRunDebugShowcase = true;

// Update this string whenever a meaningful debug build is prepared.
constexpr char kDebugVersion[] = "2026.7.4.10";

// Animation pacing
constexpr uint16_t kFrameMs = 33;  // ~30 FPS
