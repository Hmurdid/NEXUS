#include "humidity.h"
#include "../display.h"
#include "../keyboard.h"
#include "../shell.h"
#include "../config.h"
#include <DHT.h>

#define HISTORY_SIZE 64

static DHT dht(PIN_DHT11, DHT22);
static bool dhtInitialized = false;
static float history[HISTORY_SIZE];
static int historyIndex = 0;
static bool historyFull = false;

static void drawGraph(float currentValue, const char* label) {
    clearScreen();
    drawTitle("Humidity");
    
    // ВЕРХНЯ ПОЛОВИНА - параметри (під заголовком)
    int centerY = 50;
    
    // Велике значення вологості
    tft.setTextSize(3);
    tft.setTextColor(COL_PRIMARY);
    char tempStr[16];
    snprintf(tempStr, sizeof(tempStr), "%.1f%%", currentValue);
    int textW = strlen(tempStr) * 6 * 3;
    tft.setCursor((SCREEN_W - textW) / 2, centerY - 10);
    tft.print(tempStr);
    
    // Підпис
    tft.setTextSize(1);
    tft.setTextColor(COL_TEXT);
    int labelW = strlen(label) * 6;
    tft.setCursor((SCREEN_W - labelW) / 2, centerY + 20);
    tft.print(label);
    
    int graphTop = 80;
    
    // НИЖНЯ ПОЛОВИНА - графік
    int graphHeight = SCREEN_H - graphTop - 5;
    int graphWidth = SCREEN_W - 4;
    int graphX = 2;
    
    if (historyFull || historyIndex > 1) {
        int count = historyFull ? HISTORY_SIZE : historyIndex;
        
        // Знаходимо min/max для масштабування
        float minV = history[0], maxV = history[0];
        for (int i = 1; i < count; i++) {
            if (history[i] < minV) minV = history[i];
            if (history[i] > maxV) maxV = history[i];
        }
        
        // Мінімальний діапазон
        if (maxV - minV < 5.0) {
            minV -= 2.5;
            maxV += 2.5;
        }
        
        float range = maxV - minV;
        if (range < 0.001) range = 1.0;
        
        // Малюємо графік: нові дані СПРАВА, хвіст тягнеться ВЛІВО
        // Йдемо від найновішої (справа) до найстарішої (зліва)
        int rightEdge = graphX + graphWidth;
        for (int i = 0; i < count - 1; i++) {
            // idx1 - новіша (правіше), idx2 - старіша (лівіше)
            int idx1 = (historyIndex - 1 - i + HISTORY_SIZE) % HISTORY_SIZE;
            int idx2 = (historyIndex - 2 - i + HISTORY_SIZE) % HISTORY_SIZE;
            
            // x починається з правого краю і йде вліво
            int x1 = rightEdge - (i * graphWidth) / count;
            int x2 = rightEdge - ((i + 1) * graphWidth) / count;
            
            int y1 = graphTop + graphHeight - (int)((history[idx1] - minV) / range * graphHeight);
            int y2 = graphTop + graphHeight - (int)((history[idx2] - minV) / range * graphHeight);
            
            // Обмеження
            if (y1 < graphTop) y1 = graphTop;
            if (y1 > graphTop + graphHeight) y1 = graphTop + graphHeight;
            if (y2 < graphTop) y2 = graphTop;
            if (y2 > graphTop + graphHeight) y2 = graphTop + graphHeight;
            
            tft.drawLine(x1, y1, x2, y2, COL_PRIMARY);
        }
    }
    
    // Мітки min/max
    tft.setTextSize(1);
    tft.setTextColor(COL_TEXT);
    if (historyFull || historyIndex > 0) {
        int count = historyFull ? HISTORY_SIZE : historyIndex;
        float minV = history[0], maxV = history[0];
        for (int i = 1; i < count; i++) {
            if (history[i] < minV) minV = history[i];
            if (history[i] > maxV) maxV = history[i];
        }
        char minMax[24];
        snprintf(minMax, sizeof(minMax), "min %.0f%% max %.0f%%", minV, maxV);
        tft.setCursor(2, SCREEN_H - 10);
        tft.print(minMax);
    }
}

void runHumidity() {
    if (!dhtInitialized) {
        dht.begin();
        dhtInitialized = true;
    }
    
    // Скидаємо історію при вході
    historyIndex = 0;
    historyFull = false;
    
    const int FRAME_MS = 500; // Оновлення кожні 500мс
    unsigned long lastFrame = 0;
    
    while (currentState == STATE_HUMIDITY) {
        unsigned long now = millis();
        
        // FPS limit
        if (now - lastFrame < FRAME_MS) {
            delay(10);
            continue;
        }
        lastFrame = now;
        
        // Читаємо вологість
        float hum = dht.readHumidity();
        
        // Перевіряємо помилку читання
        if (isnan(hum)) {
            hum = 0;
        }
        
        // Додаємо в історію
        history[historyIndex] = hum;
        historyIndex++;
        if (historyIndex >= HISTORY_SIZE) {
            historyIndex = 0;
            historyFull = true;
        }
        
        // Малюємо
        drawGraph(hum, "DHT11 Sensor");
        
        // Перевіряємо кнопки
        char key = getKeyHeld(350, 150);
        if (key == 'B' || key == 'D') {
            currentState = STATE_SHELL;
        }
    }
}
