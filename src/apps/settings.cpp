#include "settings.h"
#include "../display.h"
#include "../keyboard.h"
#include "../shell.h"
#include "../config.h"
#include <Adafruit_NeoPixel.h>

// Глобальні змінні теми
uint16_t themePrimary = ST77XX_MAGENTA;
uint16_t themeSelect = ST77XX_MAGENTA;

// RGB LED
Adafruit_NeoPixel rgbStrip = Adafruit_NeoPixel(1, PIN_RGB, NEO_GRB + NEO_KHZ800);
bool rgbInitialized = false;

// Яскравість підсвітки
int backlightLevel = 255;

// Кольори теми
const uint16_t themeColors[] = {
    ST77XX_MAGENTA, // 0 - Magenta
    ST77XX_GREEN,   // 1 - Green
    ST77XX_CYAN,    // 2 - Cyan
    ST77XX_ORANGE,  // 3 - Orange
    ST77XX_BLUE,    // 4 - Blue
    ST77XX_RED,     // 5 - Red
    ST77XX_YELLOW,  // 6 - Yellow
};
const int THEME_COUNT = sizeof(themeColors) / sizeof(themeColors[0]);
int currentTheme = 0;

// RGB кольори
int rgbR = 0, rgbG = 0, rgbB = 0;

// ============ SETTINGS MENU ============
void runSettings() {
    const char* items[] = { "Screenlight", "RGB LED", "Theme" };
    const int ITEM_COUNT = 3;
    int selected = 0;
    
    clearScreen();
    drawTitle("Settings");
    
    // Малюємо меню
    for (int i = 0; i < ITEM_COUNT; i++) {
        int y = 28 + i * 20;
        
        if (i == selected) {
            tft.fillRect(0, y, tft.width(), 18, COL_SELECT);
            tft.setTextColor(COL_BG);
        } else {
            tft.setTextColor(COL_TEXT);
        }
        
        tft.setTextSize(1);
        tft.setCursor(6, y + 4);
        tft.print("> ");
        tft.print(items[i]);
    }
    
    while (currentState == STATE_SETTINGS) {
        char key = getKeyHeld(350, 80);
        
        if (key == '2') {
            selected--;
            if (selected < 0) selected = ITEM_COUNT - 1;
            clearScreen();
            drawTitle("Settings");
            for (int i = 0; i < ITEM_COUNT; i++) {
                int y = 28 + i * 20;
                if (i == selected) {
                    tft.fillRect(0, y, tft.width(), 18, COL_SELECT);
                    tft.setTextColor(COL_BG);
                } else {
                    tft.setTextColor(COL_TEXT);
                }
                tft.setTextSize(1);
                tft.setCursor(6, y + 4);
                tft.print("> ");
                tft.print(items[i]);
            }
        }
        else if (key == '8') {
            selected++;
            if (selected >= ITEM_COUNT) selected = 0;
            clearScreen();
            drawTitle("Settings");
            for (int i = 0; i < ITEM_COUNT; i++) {
                int y = 28 + i * 20;
                if (i == selected) {
                    tft.fillRect(0, y, tft.width(), 18, COL_SELECT);
                    tft.setTextColor(COL_BG);
                } else {
                    tft.setTextColor(COL_TEXT);
                }
                tft.setTextSize(1);
                tft.setCursor(6, y + 4);
                tft.print("> ");
                tft.print(items[i]);
            }
        }
        else if (key == 'A') {
            if (selected == 0) currentState = STATE_SCREENLIGHT;
            else if (selected == 1) currentState = STATE_LED;
            else if (selected == 2) currentState = STATE_THEME;
        }
        else if (key == 'B' || key == 'D') {
            currentState = STATE_SHELL;
        }
    }
}

// ============ SCREENLIGHT ============
void runScreenlight() {
    int level = backlightLevel;
    
    clearScreen();
    drawTitle("Screenlight");
    
    auto drawUI = [&]() {
        tft.setTextColor(COL_TEXT);
        tft.setTextSize(2);
        tft.setCursor(30, 60);
        tft.print(level);
        tft.print(" / 255");
        
        int barW = (level * 100) / 255;
        tft.fillRect(14, 90, 100, 16, COL_BG);
        tft.fillRect(14, 90, barW, 16, COL_PRIMARY);
        tft.drawRect(14, 90, 100, 16, COL_TEXT);
    };
    drawUI();
    
    while (currentState == STATE_SCREENLIGHT) {
        char key = getKeyHeld(350, 80);
        
        if (key == '4' || key == '2') {
            level -= 10; if (level < 5) level = 5;
            clearScreen(); drawTitle("Screenlight"); drawUI();
        }
        else if (key == '6' || key == '8') {
            level += 10; if (level > 255) level = 255;
            clearScreen(); drawTitle("Screenlight"); drawUI();
        }
        else if (key == 'B' || key == 'D') { currentState = STATE_SETTINGS; }
        
        analogWrite(TFT_BL, level);
        backlightLevel = level;
    }
}

// ============ RGB LED ============
void runLed() {
    if (!rgbInitialized) {
        rgbStrip.begin();
        rgbStrip.setBrightness(50);
        rgbStrip.show();
        rgbInitialized = true;
    }
    
    int colorMode = 0;
    const char* colorNames[] = { "Custom", "Red", "Green", "Blue", "Cyan", "Magenta", "Yellow", "White", "OFF" };
    const int MODE_COUNT = 9;
    
    auto drawUI = [&]() {
        tft.setTextColor(COL_TEXT);
        tft.setTextSize(1);
        tft.setCursor(6, 28);
        tft.print(colorNames[colorMode]);
        
        tft.setCursor(6, 45);
        tft.print("R:"); tft.print(rgbR);
        tft.setCursor(60, 45);
        tft.print("G:"); tft.print(rgbG);
        tft.setCursor(6, 58);
        tft.print("B:"); tft.print(rgbB);
        
        tft.setCursor(6, 85);
        tft.print("1-3 R 4-6 G 7-9 B");
        tft.setCursor(6, 98);
        tft.print("2/8 Mode");
    };
    
    clearScreen();
    drawTitle("RGB LED");
    drawUI();
    
    while (currentState == STATE_LED) {
        char key = getKeyHeld(350, 80);
        bool changed = false;
        
        if (key == '1') { rgbR -= 15; if (rgbR < 0) rgbR = 0; colorMode = 0; changed = true; }
        else if (key == '3') { rgbR += 15; if (rgbR > 255) rgbR = 255; colorMode = 0; changed = true; }
        else if (key == '4') { rgbG -= 15; if (rgbG < 0) rgbG = 0; colorMode = 0; changed = true; }
        else if (key == '6') { rgbG += 15; if (rgbG > 255) rgbG = 255; colorMode = 0; changed = true; }
        else if (key == '7') { rgbB -= 15; if (rgbB < 0) rgbB = 0; colorMode = 0; changed = true; }
        else if (key == '9') { rgbB += 15; if (rgbB > 255) rgbB = 255; colorMode = 0; changed = true; }
        else if (key == '2') { colorMode++; if (colorMode >= MODE_COUNT) colorMode = 0; changed = true; }
        else if (key == '8') { colorMode--; if (colorMode < 0) colorMode = MODE_COUNT - 1; changed = true; }
        else if (key == 'B' || key == 'D') { currentState = STATE_SETTINGS; }
        
        if (changed) {
            clearScreen(); drawTitle("RGB LED"); drawUI();
        }
        
        // Встановлюємо колір
        if (colorMode == 0) {
            rgbStrip.setPixelColor(0, rgbStrip.Color(rgbR, rgbG, rgbB));
        } else if (colorMode == 8) {
            rgbStrip.setPixelColor(0, rgbStrip.Color(0, 0, 0));
        } else {
            uint8_t preset[7][3] = {{255,0,0},{0,255,0},{0,0,255},{0,255,255},{255,0,255},{255,255,0},{255,255,255}};
            rgbStrip.setPixelColor(0, rgbStrip.Color(preset[colorMode-1][0], preset[colorMode-1][1], preset[colorMode-1][2]));
            rgbR = preset[colorMode-1][0];
            rgbG = preset[colorMode-1][1];
            rgbB = preset[colorMode-1][2];
        }
        rgbStrip.show();
    }
}

// ============ THEME ============
void runTheme() {
    int selected = currentTheme;
    
    auto drawUI = [&]() {
        tft.fillRect(14, 40, 100, 40, themeColors[selected]);
        tft.setTextColor(COL_TEXT);
        tft.setTextSize(2);
        tft.setCursor(50, 100);
        tft.print(selected + 1);
        tft.print("/");
        tft.print(THEME_COUNT);
    };
    
    clearScreen();
    drawTitle("Theme");
    drawUI();
    
    while (currentState == STATE_THEME) {
        char key = getKeyHeld(350, 80);
        
        if (key == '4' || key == '2') {
            selected--; if (selected < 0) selected = THEME_COUNT - 1;
            clearScreen(); drawTitle("Theme"); drawUI();
        }
        else if (key == '6' || key == '8') {
            selected++; if (selected >= THEME_COUNT) selected = 0;
            clearScreen(); drawTitle("Theme"); drawUI();
        }
        else if (key == 'A') {
            currentTheme = selected;
            themePrimary = themeColors[selected];
            themeSelect = themeColors[selected];
        }
        else if (key == 'B' || key == 'D') { currentState = STATE_SETTINGS; }
    }
}
