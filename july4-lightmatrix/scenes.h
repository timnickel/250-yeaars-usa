#pragma once

#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

void scenesBegin(Adafruit_NeoPixel& matrix);
void scenesUpdate(uint32_t nowMs);
void scenesSetBrightness(uint8_t brightness);
void runBringUpTest(Adafruit_NeoPixel& matrix, uint32_t nowMs);
void runCornerOrientationTest(Adafruit_NeoPixel& matrix, uint32_t nowMs);
void runCornerSequenceTest(Adafruit_NeoPixel& matrix, uint32_t nowMs);
void runDebugShowcase(Adafruit_NeoPixel& matrix, uint32_t nowMs);
