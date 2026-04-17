#include "help.h"
#include "../display.h"
#include "../keyboard.h"
#include "../shell.h"

// Слайди — масив пар (заголовок, текст)
// '\n' розділяє рядки
const char* helpSlides[] = {
    "Navigation",
    "|    || UP ||    ||Select|\n\n| L  ||    || R  ||Reject|\n\n|    ||DOWN||    ||Extra |\n\n|    ||    ||    ||Back  |",

    "Calculator",
    "| 1 || 2 || 3 || A(+)|\n\n| 4 || 5 || 6 || B(-)|\n\n| 7 || 8 || 9 || C(*)|\n\n| *(/)|| . || # || D(=)|",


    "Keyboard",
    "If you hold some buttons\nyou can scroll faster.\nAlso includes multi-keys\nsupports. (with bugs!)\nIt can be usefull in games",

    "About",
    "NEXUS v1.0\nSchool Project. I had no \nidea what to do so I just\ndecided to put all I can\ninto one device.\n\nBy Eros (Kostya)"
};

const int SLIDE_COUNT = sizeof(helpSlides) / sizeof(helpSlides[0]);

void drawSlide(int index) {
    clearScreen();
    
    // Заголовок
    tft.setTextColor(COL_PRIMARY);
    tft.setTextSize(2);
    tft.setCursor(4, 5);
    tft.print(helpSlides[index * 2]);
    
    // Лінія під заголовком
    tft.drawFastHLine(0, 24, tft.width(), COL_PRIMARY);
    
    // Текст (багаторядковий)
    tft.setTextColor(COL_TEXT);
    tft.setTextSize(1);
    
    const char* text = helpSlides[index * 2 + 1];
    int y = 32;
    
    while (*text && y < tft.height() - 20) {
        // малюємо один рядок
        int x = 4;
        while (*text && *text != '\n' && x < tft.width() - 4) {
            tft.setCursor(x, y);
            tft.print(*text);
            x += 6; // ширина символу 6px при textSize(1)
            text++;
        }
        
        if (*text == '\n') text++; // пропускаємо \n
        y += 10; // висота рядка
    }
    
    // Номер сторінки внизу
    tft.setTextColor(COL_PRIMARY);
    tft.setTextSize(1);
    char page[16];
    snprintf(page, sizeof(page), "%d/%d", index + 1, SLIDE_COUNT / 2);
    int pageW = strlen(page) * 6;
    tft.setCursor((tft.width() - pageW) / 2, tft.height() - 12);
    tft.print(page);
}

void runHelp() {
    int currentSlide = 0;
    drawSlide(currentSlide);
    
    const int FRAME_MS = 33; // ~30 FPS
    unsigned long lastFrame = millis();
    
    while (currentState == STATE_HELP) {
        // FPS limit
        unsigned long now = millis();
        if (now - lastFrame < FRAME_MS) {
            delay(1);
            continue;
        }
        lastFrame = now;
        
        char key = getKeyHeld(350, 150);
        
        if (key == '6' || key == '8') {
            // вперед
            currentSlide++;
            if (currentSlide >= SLIDE_COUNT / 2) {
                currentSlide = 0;
            }
            drawSlide(currentSlide);
        }
        else if (key == '4' || key == '2') {
            // назад
            currentSlide--;
            if (currentSlide < 0) {
                currentSlide = SLIDE_COUNT / 2 - 1;
            }
            drawSlide(currentSlide);
        }
        else if (key == 'D') {
            // вихід
            currentState = STATE_SHELL;
        }
    }
}
