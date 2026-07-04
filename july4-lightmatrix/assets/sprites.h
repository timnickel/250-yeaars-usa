#pragma once

#include <Arduino.h>

// 8x8 star glyph (LSB left) that can be reused for future patriotic scenes.
constexpr uint8_t kStar8x8[8] = {
    0b00011000,
    0b10011001,
    0b01011010,
    0b00111100,
    0b11111111,
    0b00111100,
    0b01011010,
    0b10011001,
};
