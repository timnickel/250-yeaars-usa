#include "scenes.h"

#include "assets/sprites.h"
#include "hardware_config.h"

namespace {
Adafruit_NeoPixel* gMatrix = nullptr;
uint32_t gSceneStartMs = 0;
uint8_t gSceneIndex = 0;
uint8_t gBrightness = kDefaultBrightness;

uint8_t gTwinkle[kNumPixels];

struct Glyph {
    char c;
    uint8_t rows[7];
};

constexpr Glyph kFont[] = {
    {' ', {0, 0, 0, 0, 0, 0, 0}},
    {'4', {0b00010, 0b00110, 0b01010, 0b10010, 0b11111, 0b00010, 0b00010}},
    {'A', {0b01110, 0b10001, 0b10001, 0b11111, 0b10001, 0b10001, 0b10001}},
    {'H', {0b10001, 0b10001, 0b10001, 0b11111, 0b10001, 0b10001, 0b10001}},
    {'P', {0b11110, 0b10001, 0b10001, 0b11110, 0b10000, 0b10000, 0b10000}},
    {'S', {0b01111, 0b10000, 0b10000, 0b01110, 0b00001, 0b00001, 0b11110}},
    {'T', {0b11111, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100}},
    {'U', {0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110}},
    {'Y', {0b10001, 0b10001, 0b01010, 0b00100, 0b00100, 0b00100, 0b00100}},
};

const char* kIntroText = " HAPPY 4TH USA ";

using SceneInitFn = void (*)();
using SceneRenderFn = void (*)(uint32_t);

struct SceneDef {
    SceneInitFn init;
    SceneRenderFn render;
    uint8_t durationSec;
};

uint16_t xyToIndex(uint8_t x, uint8_t y) {
    if (x >= kMatrixWidth || y >= kMatrixHeight) {
        return 0;
    }

    if (kMatrixRowMajor) {
        if (!kMatrixSerpentine) {
            return static_cast<uint16_t>(y) * kMatrixWidth + x;
        }
        if ((y & 1U) == 0U) {
            return static_cast<uint16_t>(y) * kMatrixWidth + x;
        }
        return static_cast<uint16_t>(y) * kMatrixWidth + (kMatrixWidth - 1U - x);
    }

    if (!kMatrixSerpentine) {
        return static_cast<uint16_t>(x) * kMatrixHeight + y;
    }
    if ((x & 1U) == 0U) {
        return static_cast<uint16_t>(x) * kMatrixHeight + y;
    }
    return static_cast<uint16_t>(x) * kMatrixHeight + (kMatrixHeight - 1U - y);
}

void clearAll() {
    gMatrix->clear();
}

void setPixelXY(uint8_t x, uint8_t y, uint32_t color) {
    if (x >= kMatrixWidth || y >= kMatrixHeight) {
        return;
    }
    gMatrix->setPixelColor(xyToIndex(x, y), color);
}

uint32_t rgb(uint8_t r, uint8_t g, uint8_t b) {
    return gMatrix->Color(r, g, b);
}

uint32_t scaleColor(uint32_t color, uint8_t scale) {
    const uint8_t r = static_cast<uint8_t>((((color >> 16) & 0xFFU) * scale) >> 8);
    const uint8_t g = static_cast<uint8_t>((((color >> 8) & 0xFFU) * scale) >> 8);
    const uint8_t b = static_cast<uint8_t>(((color & 0xFFU) * scale) >> 8);
    return rgb(r, g, b);
}

const Glyph* findGlyph(char c) {
    for (const auto& glyph : kFont) {
        if (glyph.c == c) {
            return &glyph;
        }
    }
    return &kFont[0];
}

void drawChar(int16_t originX, int16_t originY, char c, uint32_t color) {
    const Glyph* glyph = findGlyph(c);
    for (uint8_t row = 0; row < 7; ++row) {
        const uint8_t bits = glyph->rows[row];
        for (uint8_t col = 0; col < 5; ++col) {
            if ((bits & (1U << (4U - col))) == 0U) {
                continue;
            }
            const int16_t x = originX + col;
            const int16_t y = originY + row;
            if (x < 0 || y < 0) {
                continue;
            }
            setPixelXY(static_cast<uint8_t>(x), static_cast<uint8_t>(y), color);
        }
    }
}

void drawText(int16_t originX, int16_t originY, const char* text, uint32_t color) {
    int16_t x = originX;
    for (uint16_t i = 0; text[i] != '\0'; ++i) {
        drawChar(x, originY, text[i], color);
        x += 6;
    }
}

void introInit() {
    clearAll();
}

void introRender(uint32_t elapsedMs) {
    clearAll();
    const int16_t textWidth = static_cast<int16_t>(strlen(kIntroText) * 6);
    const int16_t offset = static_cast<int16_t>(elapsedMs / 75U);
    const int16_t x = static_cast<int16_t>(kMatrixWidth) - offset;

    drawText(x, 0, kIntroText, rgb(255, 255, 255));

    if (x + textWidth < 0) {
        drawText(static_cast<int16_t>(kMatrixWidth), 0, kIntroText, rgb(255, 255, 255));
    }
}

void twinkleInit() {
    memset(gTwinkle, 0, sizeof(gTwinkle));
}

void twinkleRender(uint32_t) {
    for (uint16_t i = 0; i < kNumPixels; ++i) {
        uint8_t v = gTwinkle[i];
        if (v > 7) {
            v = static_cast<uint8_t>(v - 7);
        } else {
            v = 0;
        }
        gTwinkle[i] = v;

        const uint8_t baseBlue = 8;
        gMatrix->setPixelColor(i, rgb(v, v, static_cast<uint8_t>(baseBlue + v)));
    }

    for (uint8_t i = 0; i < 6; ++i) {
        const uint16_t idx = static_cast<uint16_t>(random(kNumPixels));
        gTwinkle[idx] = static_cast<uint8_t>(180 + random(75));
    }
}

void stripeInit() {
    clearAll();
}

void stripeRender(uint32_t elapsedMs) {
    const uint8_t shift = static_cast<uint8_t>(elapsedMs / 40U);

    for (uint8_t y = 0; y < kMatrixHeight; ++y) {
        for (uint8_t x = 0; x < kMatrixWidth; ++x) {
            const uint8_t pos = static_cast<uint8_t>(x * 9U + shift);
            const uint8_t band = static_cast<uint8_t>((pos / 42U) % 3U);
            uint32_t color;

            if (band == 0U) {
                color = rgb(200, 0, 0);
            } else if (band == 1U) {
                color = rgb(220, 220, 220);
            } else {
                color = rgb(0, 0, 200);
            }

            const uint8_t pulse = static_cast<uint8_t>((pos & 0x1FU) * 8U);
            const uint8_t brightness = static_cast<uint8_t>(96U + (pulse / 2U));
            setPixelXY(x, y, scaleColor(color, brightness));
        }
    }
}

void fireworksInit() {
    clearAll();
}

void spawnBurst(uint8_t cx, uint8_t cy, uint32_t color, uint8_t radius) {
    for (int8_t dy = -radius; dy <= radius; ++dy) {
        for (int8_t dx = -radius; dx <= radius; ++dx) {
            const int16_t x = static_cast<int16_t>(cx) + dx;
            const int16_t y = static_cast<int16_t>(cy) + dy;
            if (x < 0 || y < 0 || x >= kMatrixWidth || y >= kMatrixHeight) {
                continue;
            }

            const uint8_t dist2 = static_cast<uint8_t>(dx * dx + dy * dy);
            if (dist2 > static_cast<uint8_t>(radius * radius + 1U)) {
                continue;
            }

            uint8_t atten = static_cast<uint8_t>(255U - dist2 * 40U);
            if (atten < 30U) {
                atten = 30U;
            }
            setPixelXY(static_cast<uint8_t>(x), static_cast<uint8_t>(y), scaleColor(color, atten));
        }
    }
}

void fireworksRender(uint32_t elapsedMs) {
    // Fade previous frame to get a persistent trail look.
    for (uint16_t i = 0; i < kNumPixels; ++i) {
        const uint32_t c = gMatrix->getPixelColor(i);
        const uint8_t r = static_cast<uint8_t>(((c >> 16) & 0xFFU) * 220U / 255U);
        const uint8_t g = static_cast<uint8_t>(((c >> 8) & 0xFFU) * 220U / 255U);
        const uint8_t b = static_cast<uint8_t>((c & 0xFFU) * 220U / 255U);
        gMatrix->setPixelColor(i, rgb(r, g, b));
    }

    if ((elapsedMs % 180U) < 33U) {
        const uint8_t cx = static_cast<uint8_t>(random(kMatrixWidth));
        const uint8_t cy = static_cast<uint8_t>(random(kMatrixHeight));
        const uint8_t colorPick = static_cast<uint8_t>(random(3));
        uint32_t burstColor = rgb(255, 255, 255);

        if (colorPick == 0U) {
            burstColor = rgb(255, 40, 40);
        } else if (colorPick == 1U) {
            burstColor = rgb(250, 250, 250);
        } else {
            burstColor = rgb(60, 60, 255);
        }

        const uint8_t radius = static_cast<uint8_t>(1U + random(3));
        spawnBurst(cx, cy, burstColor, radius);
    }
}

void finaleInit() {
    clearAll();
}

void finaleRender(uint32_t elapsedMs) {
    const bool strobeOn = ((elapsedMs / 130U) % 2U) == 0U;

    for (uint8_t y = 0; y < kMatrixHeight; ++y) {
        for (uint8_t x = 0; x < kMatrixWidth; ++x) {
            setPixelXY(x, y, strobeOn ? rgb(12, 12, 12) : rgb(0, 0, 0));
        }
    }

    drawText(6, 0, "USA", rgb(255, 30, 30));
    drawText(18, 0, " ", rgb(0, 0, 0));
    if (!strobeOn) {
        drawText(6, 0, "USA", rgb(60, 60, 255));
    }
}

constexpr SceneDef kScenes[] = {
    {introInit, introRender, 10},
    {twinkleInit, twinkleRender, 12},
    {stripeInit, stripeRender, 12},
    {fireworksInit, fireworksRender, 12},
    {finaleInit, finaleRender, 8},
};

void switchToScene(uint8_t index, uint32_t nowMs) {
    gSceneIndex = static_cast<uint8_t>(index % (sizeof(kScenes) / sizeof(kScenes[0])));
    gSceneStartMs = nowMs;
    kScenes[gSceneIndex].init();
}

}  // namespace

void scenesBegin(Adafruit_NeoPixel& matrix) {
    gMatrix = &matrix;
    gMatrix->setBrightness(gBrightness);
    switchToScene(0, millis());
}

void scenesUpdate(uint32_t nowMs) {
    const SceneDef& scene = kScenes[gSceneIndex];
    const uint32_t elapsed = nowMs - gSceneStartMs;

    if (elapsed >= static_cast<uint32_t>(scene.durationSec) * 1000UL) {
        switchToScene(static_cast<uint8_t>(gSceneIndex + 1U), nowMs);
    }

    kScenes[gSceneIndex].render(nowMs - gSceneStartMs);
    gMatrix->show();
}

void scenesSetBrightness(uint8_t brightness) {
    gBrightness = brightness;
    if (gMatrix != nullptr) {
        gMatrix->setBrightness(gBrightness);
    }
}

void runBringUpTest(Adafruit_NeoPixel& matrix, uint32_t nowMs) {
    const uint8_t phase = static_cast<uint8_t>((nowMs / 1000U) % 4U);

    for (uint8_t y = 0; y < kMatrixHeight; ++y) {
        for (uint8_t x = 0; x < kMatrixWidth; ++x) {
            const uint32_t off = matrix.Color(0, 0, 0);
            uint32_t color = off;

            if (phase == 0U) {
                color = matrix.Color(80, 0, 0);
            } else if (phase == 1U) {
                color = matrix.Color(0, 80, 0);
            } else if (phase == 2U) {
                color = matrix.Color(0, 0, 80);
            } else {
                const bool checker = ((x + y) & 1U) == 0U;
                color = checker ? matrix.Color(80, 80, 80) : off;
            }

            const uint16_t index = xyToIndex(x, y);
            matrix.setPixelColor(index, color);
        }
    }

    if (phase == 3U) {
        const uint8_t scanX = static_cast<uint8_t>((nowMs / 80U) % kMatrixWidth);
        const uint8_t scanY = static_cast<uint8_t>((nowMs / 140U) % kMatrixHeight);
        matrix.setPixelColor(xyToIndex(scanX, scanY), matrix.Color(255, 255, 255));
    }

    matrix.show();
}
