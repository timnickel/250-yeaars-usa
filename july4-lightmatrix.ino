#include <Adafruit_NeoPixel.h>

#include "hardware_config.h"
#include "scenes.h"

Adafruit_NeoPixel matrix(kNumPixels, kMatrixPin, kPixelType);

void setup() {
    randomSeed(analogRead(A0));

    matrix.begin();
    matrix.clear();
    matrix.setBrightness(kDefaultBrightness);
    matrix.show();

    scenesBegin(matrix);
}

void loop() {
    static uint32_t lastFrame = 0;

    const uint32_t nowMs = millis();
    if (nowMs - lastFrame < kFrameMs) {
        return;
    }
    lastFrame = nowMs;

    if (kRunBringUpTest) {
        runBringUpTest(matrix, nowMs);
        return;
    }

    scenesUpdate(nowMs);
}
