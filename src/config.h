#pragma once

// ========================
// TFT ST7735
// ========================
#define TFT_SCK     12
#define TFT_MOSI    11 // NO NEED! 
#define TFT_CS      10 
#define TFT_DC       9
#define TFT_RST      8
#define TFT_BL      14

// ========================
// 4x4 Матриця кнопок
// ========================
#define KBD_R1       5
#define KBD_R2       6
#define KBD_R3       7
#define KBD_R4      15
#define KBD_C1       1
#define KBD_C2       2
#define KBD_C3       3
#define KBD_C4       4

// ========================
// Датчики
// ========================
#define PIN_DHT11   16
#define PIN_KY037   17
#define PIN_VOLT    18
#define PIN_BATTERY 34

// ========================
// Інше
// ========================
#define PIN_BUZZER  21
#define PIN_LED     22
#define PIN_RGB     48  // WS2812 onboard

// ========================
// Дисплей
// ========================
#define SCREEN_W   128
#define SCREEN_H   160

// ========================
// Кольори (shortcuts)
// ========================
#define COL_BG      ST77XX_BLACK
#define COL_TEXT    ST77XX_WHITE
#define COL_ACCENT  ST77XX_YELLOW

// Змінні кольори теми
extern uint16_t themePrimary;
extern uint16_t themeSelect;
#define COL_PRIMARY themePrimary
#define COL_SELECT  themeSelect
