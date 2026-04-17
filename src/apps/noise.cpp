#include "noise.h"
#include "../display.h"
#include "../keyboard.h"
#include "../shell.h"
#include "../config.h"

#define HISTORY_SIZE 64

static uint16_t history[HISTORY_SIZE];
static int historyIndex = 0;
static bool historyFull = false;

static void drawNoiseScreen(uint16_t currentValue) {
    clearScreen();
    drawTitle("Noise");

    // ВЕРХНЯ ПОЛОВИНА - параметри (під заголовком)
    int centerY = 50;

    // Велике значення (ADC)
    tft.setTextSize(3);
    tft.setTextColor(COL_PRIMARY);
    char vStr[16];
    snprintf(vStr, sizeof(vStr), "%u", (unsigned)currentValue);
    int textW = strlen(vStr) * 6 * 3;
    tft.setCursor((SCREEN_W - textW) / 2, centerY - 10);
    tft.print(vStr);

    // Підпис
    tft.setTextSize(1);
    tft.setTextColor(COL_TEXT);
    const char* label = "KY-037 (analog)";
    int labelW = strlen(label) * 6;
    tft.setCursor((SCREEN_W - labelW) / 2, centerY + 20);
    tft.print(label);

    // НИЖНЯ ПОЛОВИНА - графік
    int graphTop = 80;
    int graphHeight = SCREEN_H - graphTop - 5;
    int graphWidth = SCREEN_W - 4;
    int graphX = 2;

    int count = historyFull ? HISTORY_SIZE : historyIndex;
    if (count > 1) {
        // min/max
        uint16_t minV = history[0], maxV = history[0];
        for (int i = 1; i < count; i++) {
            if (history[i] < minV) minV = history[i];
            if (history[i] > maxV) maxV = history[i];
        }
        if (maxV == minV) {
            if (maxV < 4095) maxV++;
            else minV--;
        }
        float range = (float)(maxV - minV);
        if (range < 1.0f) range = 1.0f;

        // нові дані справа, хвіст вліво
        int rightEdge = graphX + graphWidth;
        for (int i = 0; i < count - 1; i++) {
            int idx1 = (historyIndex - 1 - i + HISTORY_SIZE) % HISTORY_SIZE;
            int idx2 = (historyIndex - 2 - i + HISTORY_SIZE) % HISTORY_SIZE;

            int x1 = rightEdge - (i * graphWidth) / count;
            int x2 = rightEdge - ((i + 1) * graphWidth) / count;

            int y1 = graphTop + graphHeight - (int)(((float)history[idx1] - minV) / range * graphHeight);
            int y2 = graphTop + graphHeight - (int)(((float)history[idx2] - minV) / range * graphHeight);

            if (y1 < graphTop) y1 = graphTop;
            if (y1 > graphTop + graphHeight) y1 = graphTop + graphHeight;
            if (y2 < graphTop) y2 = graphTop;
            if (y2 > graphTop + graphHeight) y2 = graphTop + graphHeight;

            tft.drawLine(x1, y1, x2, y2, COL_PRIMARY);
        }

        // min/max текст
        tft.setTextSize(1);
        tft.setTextColor(COL_TEXT);
        char minMax[28];
        snprintf(minMax, sizeof(minMax), "min %u max %u", (unsigned)minV, (unsigned)maxV);
        tft.setCursor(2, SCREEN_H - 10);
        tft.print(minMax);
    }
}

void runNoise() {
    // на ESP32-S3 можна читати аналог напряму
    pinMode(PIN_KY037, INPUT);

    historyIndex = 0;
    historyFull = false;

    const int FRAME_MS = 100; // ~10 FPS
    unsigned long lastFrame = 0;

    while (currentState == STATE_NOISE) {
        unsigned long now = millis();
        if (now - lastFrame < FRAME_MS) {
            delay(1);
            continue;
        }
        lastFrame = now;

        uint16_t v = (uint16_t)analogRead(PIN_KY037);

        history[historyIndex] = v;
        historyIndex++;
        if (historyIndex >= HISTORY_SIZE) {
            historyIndex = 0;
            historyFull = true;
        }

        drawNoiseScreen(v);

        char key = getKeyHeld(350, 150);
        if (key == 'B' || key == 'D') {
            currentState = STATE_SHELL;
        }
    }
}
