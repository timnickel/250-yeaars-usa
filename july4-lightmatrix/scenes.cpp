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
    {'.', {0, 0, 0, 0, 0, 0b00100, 0b00100}},
    {'0', {0b01110, 0b10001, 0b10011, 0b10101, 0b11001, 0b10001, 0b01110}},
    {'1', {0b00100, 0b01100, 0b00100, 0b00100, 0b00100, 0b00100, 0b01110}},
    {'2', {0b01110, 0b10001, 0b00001, 0b00010, 0b00100, 0b01000, 0b11111}},
    {'4', {0b00010, 0b00110, 0b01010, 0b10010, 0b11111, 0b00010, 0b00010}},
    {'6', {0b00110, 0b01000, 0b10000, 0b11110, 0b10001, 0b10001, 0b01110}},
    {'7', {0b11111, 0b00001, 0b00010, 0b00100, 0b01000, 0b01000, 0b01000}},
    {'A', {0b01110, 0b10001, 0b10001, 0b11111, 0b10001, 0b10001, 0b10001}},
    {'E', {0b11111, 0b10000, 0b10000, 0b11110, 0b10000, 0b10000, 0b11111}},
    {'H', {0b10001, 0b10001, 0b10001, 0b11111, 0b10001, 0b10001, 0b10001}},
    {'L', {0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b11111}},
    {'O', {0b01110, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110}},
    {'P', {0b11110, 0b10001, 0b10001, 0b11110, 0b10000, 0b10000, 0b10000}},
    {'R', {0b11110, 0b10001, 0b10001, 0b11110, 0b10100, 0b10010, 0b10001}},
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

constexpr uint16_t kDebugPhaseDurationsMs[] = {
    7000, 3500, 3500, 3500, 3500,
    3500, 3500, 3500, 3000, 3500,
    3500, 3500, 3000, 3000, 3500,
    3500, 3500, 3500, 3500, 3500,
};

uint16_t xyToIndex(uint8_t x, uint8_t y) {
    if (x >= kMatrixWidth || y >= kMatrixHeight) {
        return 0;
    }

    if (activeMatrixRowMajor()) {
        if (!activeMatrixSerpentine()) {
            return static_cast<uint16_t>(y) * kMatrixWidth + x;
        }
        if ((y & 1U) == 0U) {
            return static_cast<uint16_t>(y) * kMatrixWidth + x;
        }
        return static_cast<uint16_t>(y) * kMatrixWidth + (kMatrixWidth - 1U - x);
    }

    if (!activeMatrixSerpentine()) {
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

uint32_t wheelColor(uint8_t pos) {
    if (pos < 85U) {
        return rgb(static_cast<uint8_t>(255U - pos * 3U), static_cast<uint8_t>(pos * 3U), 0);
    }
    if (pos < 170U) {
        pos = static_cast<uint8_t>(pos - 85U);
        return rgb(0, static_cast<uint8_t>(255U - pos * 3U), static_cast<uint8_t>(pos * 3U));
    }
    pos = static_cast<uint8_t>(pos - 170U);
    return rgb(static_cast<uint8_t>(pos * 3U), 0, static_cast<uint8_t>(255U - pos * 3U));
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

int16_t measureTextWidth(const char* text) {
    int16_t width = 0;
    for (uint16_t i = 0; text[i] != '\0'; ++i) {
        width += 6;
    }
    return width;
}

void drawCenteredText(const char* text, int16_t originY, uint32_t color) {
    const int16_t width = measureTextWidth(text);
    const int16_t originX = (static_cast<int16_t>(kMatrixWidth) - width) / 2;
    drawText(originX, originY, text, color);
}

void drawStarSprite(int16_t originX, int16_t originY, uint32_t color) {
    for (uint8_t row = 0; row < 8; ++row) {
        for (uint8_t col = 0; col < 8; ++col) {
            if ((kStar8x8[row] & (1U << col)) == 0U) {
                continue;
            }
            const int16_t x = originX + col;
            const int16_t y = originY + row;
            if (x < 0 || y < 0 || x >= kMatrixWidth || y >= kMatrixHeight) {
                continue;
            }
            setPixelXY(static_cast<uint8_t>(x), static_cast<uint8_t>(y), color);
        }
    }
}

void renderScrollingText(const char* text, uint32_t elapsedMs, uint32_t color, uint16_t speedMs) {
    clearAll();
    const int16_t textWidth = measureTextWidth(text);
    const int16_t offset = static_cast<int16_t>(elapsedMs / speedMs);
    const int16_t x = static_cast<int16_t>(kMatrixWidth) - offset;

    drawText(x, 0, text, color);
    if (x + textWidth < 0) {
        drawText(static_cast<int16_t>(kMatrixWidth), 0, text, color);
    }
}

void renderUsaPulse(uint32_t elapsedMs) {
    clearAll();
    const uint8_t pulse = static_cast<uint8_t>(96U + ((elapsedMs / 20U) % 96U));
    drawCenteredText("USA", 0, scaleColor(rgb(255, 255, 255), pulse));
}

void renderStarSweep(uint32_t elapsedMs) {
    for (uint8_t y = 0; y < kMatrixHeight; ++y) {
        for (uint8_t x = 0; x < kMatrixWidth; ++x) {
            setPixelXY(x, y, rgb(0, 0, 24));
        }
    }

    for (uint8_t i = 0; i < 8; ++i) {
        const uint8_t sx = static_cast<uint8_t>((i * 7U + (elapsedMs / 250U)) % kMatrixWidth);
        const uint8_t sy = static_cast<uint8_t>((i * 3U + (elapsedMs / 600U)) % kMatrixHeight);
        setPixelXY(sx, sy, rgb(32, 32, 72));
    }

    const int16_t travel = static_cast<int16_t>(kMatrixWidth + 8U);
    const int16_t starX = static_cast<int16_t>(travel - 1 - ((elapsedMs / 140U) % travel)) - 7;
    drawStarSprite(starX, 0, rgb(255, 255, 255));
}

void renderSparkler(uint32_t elapsedMs) {
    clearAll();
    const uint8_t baseX = static_cast<uint8_t>(kMatrixWidth / 2U);

    for (uint8_t y = 0; y < kMatrixHeight; ++y) {
        const uint8_t warm = static_cast<uint8_t>(40U + y * 10U);
        setPixelXY(baseX, static_cast<uint8_t>(kMatrixHeight - 1U - y), rgb(warm, warm / 4U, 0));
    }

    for (uint8_t i = 0; i < 10; ++i) {
        const uint8_t sparkX = static_cast<uint8_t>((baseX + i * 5U + (elapsedMs / 90U)) % kMatrixWidth);
        const uint8_t sparkY = static_cast<uint8_t>((i * 11U + (elapsedMs / 60U)) % kMatrixHeight);
        const uint32_t color = (i & 1U) == 0U ? rgb(255, 255, 255) : rgb(80, 80, 255);
        setPixelXY(sparkX, sparkY, color);
    }
}

void renderDiagonalRainbow(uint32_t elapsedMs) {
    for (uint8_t y = 0; y < kMatrixHeight; ++y) {
        for (uint8_t x = 0; x < kMatrixWidth; ++x) {
            const uint8_t hue = static_cast<uint8_t>(x * 7U + y * 14U + (elapsedMs / 20U));
            setPixelXY(x, y, wheelColor(hue));
        }
    }
}

void renderVerticalScanner(uint32_t elapsedMs) {
    clearAll();
    const uint8_t x = static_cast<uint8_t>((elapsedMs / 70U) % kMatrixWidth);
    const uint8_t xm1 = static_cast<uint8_t>((x + kMatrixWidth - 1U) % kMatrixWidth);
    const uint8_t xm2 = static_cast<uint8_t>((x + kMatrixWidth - 2U) % kMatrixWidth);
    for (uint8_t y = 0; y < kMatrixHeight; ++y) {
        setPixelXY(xm2, y, rgb(10, 10, 30));
        setPixelXY(xm1, y, rgb(30, 30, 80));
        setPixelXY(x, y, rgb(255, 255, 255));
    }
}

void renderHorizontalScanner(uint32_t elapsedMs) {
    clearAll();
    const uint8_t y = static_cast<uint8_t>((elapsedMs / 120U) % kMatrixHeight);
    const uint8_t ym1 = static_cast<uint8_t>((y + kMatrixHeight - 1U) % kMatrixHeight);
    for (uint8_t x = 0; x < kMatrixWidth; ++x) {
        setPixelXY(x, ym1, rgb(20, 0, 30));
        setPixelXY(x, y, rgb(255, 60, 60));
    }
}

void renderCheckerPulse(uint32_t elapsedMs) {
    uint8_t wave = static_cast<uint8_t>((elapsedMs / 10U) % 64U);
    if (wave > 31U) {
        wave = static_cast<uint8_t>(63U - wave);
    }
    const uint8_t s = static_cast<uint8_t>(90U + wave * 5U);
    for (uint8_t y = 0; y < kMatrixHeight; ++y) {
        for (uint8_t x = 0; x < kMatrixWidth; ++x) {
            const bool even = ((x + y) & 1U) == 0U;
            const uint32_t c = even ? rgb(255, 30, 30) : rgb(30, 30, 255);
            setPixelXY(x, y, scaleColor(c, s));
        }
    }
}

void renderBarsBreathing(uint32_t elapsedMs) {
    const uint8_t shift = static_cast<uint8_t>(elapsedMs / 18U);
    for (uint8_t y = 0; y < kMatrixHeight; ++y) {
        for (uint8_t x = 0; x < kMatrixWidth; ++x) {
            const uint8_t band = static_cast<uint8_t>((x / 11U) % 3U);
            uint32_t base = rgb(220, 220, 220);
            if (band == 0U) {
                base = rgb(220, 20, 20);
            } else if (band == 2U) {
                base = rgb(20, 20, 220);
            }
            const uint8_t pulse = static_cast<uint8_t>(90U + ((shift + x + y * 3U) & 0x3FU) * 2U);
            setPixelXY(x, y, scaleColor(base, pulse));
        }
    }
}

void renderCenterBurst(uint32_t elapsedMs) {
    clearAll();
    const int8_t cx = static_cast<int8_t>(kMatrixWidth / 2U);
    const int8_t cy = static_cast<int8_t>(kMatrixHeight / 2U);
    const uint8_t radius = static_cast<uint8_t>((elapsedMs / 150U) % 12U);
    for (uint8_t y = 0; y < kMatrixHeight; ++y) {
        for (uint8_t x = 0; x < kMatrixWidth; ++x) {
            const int8_t dx = static_cast<int8_t>(x) - cx;
            const int8_t dy = static_cast<int8_t>(y) - cy;
            const uint8_t d2 = static_cast<uint8_t>(dx * dx + dy * dy);
            if (d2 <= static_cast<uint8_t>(radius * radius)) {
                setPixelXY(x, y, rgb(255, 255, 255));
            } else if (d2 <= static_cast<uint8_t>((radius + 2U) * (radius + 2U))) {
                setPixelXY(x, y, rgb(40, 40, 160));
            }
        }
    }
}

void renderTroopSalute(uint32_t elapsedMs) {
    const uint8_t part = static_cast<uint8_t>((elapsedMs / 1700U) % 2U);

    if (part == 0U) {
        renderScrollingText("SALUTE HERO", elapsedMs, rgb(240, 240, 240), 78U);
        return;
    }

    // Ceremonial march stripes with star markers.
    clearAll();
    const uint8_t shift = static_cast<uint8_t>(elapsedMs / 40U);

    for (uint8_t y = 0; y < kMatrixHeight; ++y) {
        for (uint8_t x = 0; x < kMatrixWidth; ++x) {
            const uint8_t stripe = static_cast<uint8_t>(((x + shift) / 3U) % 3U);
            uint32_t c = rgb(20, 20, 100);
            if (stripe == 0U) {
                c = rgb(200, 25, 25);
            } else if (stripe == 1U) {
                c = rgb(220, 220, 220);
            }
            setPixelXY(x, y, c);
        }
    }

    for (uint8_t i = 0; i < 4; ++i) {
        const uint8_t sx = static_cast<uint8_t>((i * 8U + (elapsedMs / 90U)) % kMatrixWidth);
        const uint8_t sy = static_cast<uint8_t>(1U + ((i + (elapsedMs / 400U)) % 6U));
        setPixelXY(sx, sy, rgb(255, 255, 255));
    }
}

void renderCometTrail(uint32_t elapsedMs) {
    for (uint16_t i = 0; i < kNumPixels; ++i) {
        const uint32_t c = gMatrix->getPixelColor(i);
        const uint8_t r = static_cast<uint8_t>(((c >> 16) & 0xFFU) * 200U / 255U);
        const uint8_t g = static_cast<uint8_t>(((c >> 8) & 0xFFU) * 200U / 255U);
        const uint8_t b = static_cast<uint8_t>((c & 0xFFU) * 200U / 255U);
        gMatrix->setPixelColor(i, rgb(r, g, b));
    }

    const uint8_t x = static_cast<uint8_t>((elapsedMs / 55U) % kMatrixWidth);
    const uint8_t y = static_cast<uint8_t>((elapsedMs / 160U + x / 3U) % kMatrixHeight);
    setPixelXY(x, y, rgb(255, 255, 255));
}

void renderColorWipe(uint32_t elapsedMs) {
    const uint8_t phase = static_cast<uint8_t>((elapsedMs / 1200U) % 4U);
    uint32_t color = rgb(220, 20, 20);
    if (phase == 1U) {
        color = rgb(220, 220, 220);
    } else if (phase == 2U) {
        color = rgb(20, 20, 220);
    } else if (phase == 3U) {
        color = rgb(255, 180, 20);
    }

    const uint8_t fill = static_cast<uint8_t>((elapsedMs / 45U) % (kNumPixels + 1U));
    clearAll();
    for (uint16_t i = 0; i < fill; ++i) {
        gMatrix->setPixelColor(i, color);
    }
}

void renderConfetti(uint32_t) {
    for (uint16_t i = 0; i < kNumPixels; ++i) {
        const uint32_t c = gMatrix->getPixelColor(i);
        const uint8_t r = static_cast<uint8_t>(((c >> 16) & 0xFFU) * 215U / 255U);
        const uint8_t g = static_cast<uint8_t>(((c >> 8) & 0xFFU) * 215U / 255U);
        const uint8_t b = static_cast<uint8_t>((c & 0xFFU) * 215U / 255U);
        gMatrix->setPixelColor(i, rgb(r, g, b));
    }

    for (uint8_t i = 0; i < 12; ++i) {
        const uint16_t idx = static_cast<uint16_t>(random(kNumPixels));
        const uint8_t pick = static_cast<uint8_t>(random(3));
        uint32_t color = rgb(255, 255, 255);
        if (pick == 0U) {
            color = rgb(255, 40, 40);
        } else if (pick == 1U) {
            color = rgb(255, 255, 255);
        } else {
            color = rgb(60, 60, 255);
        }
        gMatrix->setPixelColor(idx, color);
    }
}

uint8_t getDebugPhase(uint32_t nowMs, uint32_t* phaseElapsedMs) {
    uint32_t totalCycleMs = 0;
    for (uint8_t i = 0; i < (sizeof(kDebugPhaseDurationsMs) / sizeof(kDebugPhaseDurationsMs[0])); ++i) {
        totalCycleMs += kDebugPhaseDurationsMs[i];
    }
    uint32_t inCycle = nowMs % totalCycleMs;

    for (uint8_t phase = 0; phase < (sizeof(kDebugPhaseDurationsMs) / sizeof(kDebugPhaseDurationsMs[0])); ++phase) {
        const uint16_t duration = kDebugPhaseDurationsMs[phase];
        if (inCycle < duration) {
            *phaseElapsedMs = inCycle;
            return phase;
        }
        inCycle -= duration;
    }

    *phaseElapsedMs = 0;
    return 0;
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

void megaFireworksRender(uint32_t elapsedMs) {
    // Keep trails brighter and spawn extra bursts for a heavier fireworks segment.
    for (uint16_t i = 0; i < kNumPixels; ++i) {
        const uint32_t c = gMatrix->getPixelColor(i);
        const uint8_t r = static_cast<uint8_t>(((c >> 16) & 0xFFU) * 232U / 255U);
        const uint8_t g = static_cast<uint8_t>(((c >> 8) & 0xFFU) * 232U / 255U);
        const uint8_t b = static_cast<uint8_t>((c & 0xFFU) * 232U / 255U);
        gMatrix->setPixelColor(i, rgb(r, g, b));
    }

    if ((elapsedMs % 120U) < 33U) {
        for (uint8_t burst = 0; burst < 2; ++burst) {
            const uint8_t cx = static_cast<uint8_t>(random(kMatrixWidth));
            const uint8_t cy = static_cast<uint8_t>(random(kMatrixHeight));
            const uint8_t colorPick = static_cast<uint8_t>(random(3));

            uint32_t burstColor = rgb(255, 255, 255);
            if (colorPick == 0U) {
                burstColor = rgb(255, 50, 50);
            } else if (colorPick == 2U) {
                burstColor = rgb(70, 70, 255);
            }

            const uint8_t radius = static_cast<uint8_t>(1U + random(3));
            spawnBurst(cx, cy, burstColor, radius);
        }
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

void runRawChainTest(Adafruit_NeoPixel& matrix, uint32_t nowMs) {
    const uint8_t phase = static_cast<uint8_t>((nowMs / 1200U) % 5U);

    if (phase < 4U) {
        uint32_t color = matrix.Color(0, 0, 0);
        if (phase == 0U) {
            color = matrix.Color(120, 0, 0);
        } else if (phase == 1U) {
            color = matrix.Color(0, 120, 0);
        } else if (phase == 2U) {
            color = matrix.Color(0, 0, 120);
        } else {
            color = matrix.Color(90, 90, 90);
        }

        for (uint16_t i = 0; i < kNumPixels; ++i) {
            matrix.setPixelColor(i, color);
        }
    } else {
        matrix.clear();
        const uint16_t idx = static_cast<uint16_t>((nowMs / 40U) % kNumPixels);
        matrix.setPixelColor(idx, matrix.Color(255, 255, 255));
    }

    matrix.show();
}

void runCornerOrientationTest(Adafruit_NeoPixel& matrix, uint32_t nowMs) {
    matrix.clear();

    // Distinct corner colors make orientation obvious at a glance.
    matrix.setPixelColor(xyToIndex(0, 0), matrix.Color(180, 0, 0));
    matrix.setPixelColor(xyToIndex(kMatrixWidth - 1U, 0), matrix.Color(0, 180, 0));
    matrix.setPixelColor(xyToIndex(0, kMatrixHeight - 1U), matrix.Color(0, 0, 180));
    matrix.setPixelColor(
        xyToIndex(kMatrixWidth - 1U, kMatrixHeight - 1U), matrix.Color(180, 180, 180));

    // Trace clockwise around the perimeter so row/column direction can be verified.
    const uint16_t perimeter = static_cast<uint16_t>(2U * kMatrixWidth + 2U * kMatrixHeight - 4U);
    const uint16_t pos = static_cast<uint16_t>((nowMs / 120U) % perimeter);

    uint8_t x = 0;
    uint8_t y = 0;

    if (pos < kMatrixWidth) {
        x = static_cast<uint8_t>(pos);
        y = 0;
    } else if (pos < static_cast<uint16_t>(kMatrixWidth + kMatrixHeight - 1U)) {
        x = static_cast<uint8_t>(kMatrixWidth - 1U);
        y = static_cast<uint8_t>(pos - kMatrixWidth + 1U);
    } else if (pos < static_cast<uint16_t>(2U * kMatrixWidth + kMatrixHeight - 2U)) {
        x = static_cast<uint8_t>(kMatrixWidth - 1U -
                                 (pos - (kMatrixWidth + kMatrixHeight - 1U)));
        y = static_cast<uint8_t>(kMatrixHeight - 1U);
    } else {
        x = 0;
        y = static_cast<uint8_t>(kMatrixHeight - 1U -
                                 (pos - (2U * kMatrixWidth + kMatrixHeight - 2U)));
    }

    matrix.setPixelColor(xyToIndex(x, y), matrix.Color(255, 180, 0));
    matrix.show();
}

void runCornerSequenceTest(Adafruit_NeoPixel& matrix, uint32_t nowMs) {
    matrix.clear();

    const uint8_t phase = static_cast<uint8_t>((nowMs / 900U) % 4U);

    if (phase == 0U) {
        matrix.setPixelColor(xyToIndex(0, 0), matrix.Color(255, 0, 0));
    } else if (phase == 1U) {
        matrix.setPixelColor(xyToIndex(kMatrixWidth - 1U, 0), matrix.Color(0, 255, 0));
    } else if (phase == 2U) {
        matrix.setPixelColor(
            xyToIndex(kMatrixWidth - 1U, kMatrixHeight - 1U), matrix.Color(255, 255, 255));
    } else {
        matrix.setPixelColor(xyToIndex(0, kMatrixHeight - 1U), matrix.Color(0, 0, 255));
    }

    matrix.show();
}

void runDebugShowcase(Adafruit_NeoPixel& matrix, uint32_t nowMs) {
    static uint8_t previousPhase = 0xFFU;

    uint32_t phaseElapsedMs = 0;
    const uint8_t phase = getDebugPhase(nowMs, &phaseElapsedMs);
    if (phase != previousPhase) {
        clearAll();
        if (phase == 7U) {
            fireworksInit();
        }
        previousPhase = phase;
    }

    if (phase == 0U) {
        renderStarSweep(phaseElapsedMs);
    } else if (phase == 1U) {
        renderUsaPulse(phaseElapsedMs);
    } else if (phase == 2U) {
        renderScrollingText("1776", phaseElapsedMs, rgb(255, 40, 40), 85U);
    } else if (phase == 3U) {
        renderScrollingText("HAPPY", phaseElapsedMs, rgb(240, 240, 240), 80U);
    } else if (phase == 4U) {
        renderScrollingText("4TH", phaseElapsedMs, rgb(60, 60, 255), 80U);
    } else if (phase == 5U) {
        twinkleRender(phaseElapsedMs);
    } else if (phase == 6U) {
        stripeRender(phaseElapsedMs);
    } else if (phase == 7U) {
        fireworksRender(phaseElapsedMs);
    } else if (phase == 8U) {
        finaleRender(phaseElapsedMs);
    } else if (phase == 9U) {
        renderStarSweep(phaseElapsedMs);
    } else if (phase == 10U) {
        renderSparkler(phaseElapsedMs);
    } else if (phase == 11U) {
        renderStarSweep(phaseElapsedMs);
    } else if (phase == 12U) {
        renderVerticalScanner(phaseElapsedMs);
    } else if (phase == 13U) {
        renderHorizontalScanner(phaseElapsedMs);
    } else if (phase == 14U) {
        renderCheckerPulse(phaseElapsedMs);
    } else if (phase == 15U) {
        renderBarsBreathing(phaseElapsedMs);
    } else if (phase == 16U) {
        renderTroopSalute(phaseElapsedMs);
    } else if (phase == 17U) {
        renderCometTrail(phaseElapsedMs);
    } else if (phase == 18U) {
        megaFireworksRender(phaseElapsedMs);
    } else {
        renderConfetti(phaseElapsedMs);
    }

    matrix.show();
}
