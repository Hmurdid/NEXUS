#include "display.h"

// створюємо об'єкт дисплею
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

void initDisplay() {
    // налаштування SPI пінів для hardware SPI
    SPI.begin(TFT_SCK, -1, TFT_MOSI, -1);

    // підсвітка
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    // ресет
    pinMode(TFT_RST, OUTPUT);
    digitalWrite(TFT_RST, LOW);
    delay(100);
    digitalWrite(TFT_RST, HIGH);
    delay(100);

    // ініціалізація
    tft.initR(INITR_BLACKTAB);
    tft.setRotation(1);
    tft.fillScreen(COL_BG);
}

void clearScreen() {
    tft.fillScreen(COL_BG);
}

// малює заголовок з лінією внизу
void drawTitle(const char* title) {
    tft.setTextColor(COL_PRIMARY);
    tft.setTextSize(2);
    tft.setCursor(4, 5);
    tft.println(title);
    tft.drawFastHLine(0, 22, SCREEN_W, COL_PRIMARY);
}

// малює текст по центру екрану
void drawCenteredText(const char* text, int y, uint16_t color, uint8_t size) {
    tft.setTextSize(size);
    tft.setTextColor(color);
    // кожен символ = 6px * size широкий
    int textWidth = strlen(text) * 6 * size;
    int x = (SCREEN_W - textWidth) / 2;
    tft.setCursor(x, y);
    tft.println(text);
}