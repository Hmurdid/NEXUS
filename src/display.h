#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include "config.h"

// глобальний об'єкт дисплею
// extern означає "він існує в display.cpp, але доступний скрізь"
extern Adafruit_ST7735 tft;

// ініціалізація
void initDisplay();

// допоміжні функції
void drawTitle(const char* title);
void drawCenteredText(const char* text, int y, uint16_t color, uint8_t size);
void clearScreen();