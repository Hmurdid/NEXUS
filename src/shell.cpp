#include "shell.h"

State currentState = STATE_SHELL;

struct MenuItem {
    const char* name;
    const char* icon;
    State state;
};

MenuItem menu[] = {
    { "Help",        "?", STATE_HELP        },
    { "Thermometer", ">", STATE_THERMOMETER },
    { "Humidity",    ">", STATE_HUMIDITY    },
    { "WiFi Scan",   ">", STATE_WIFI        },
    { "Noise",       ">", STATE_NOISE       },
    { "Calculator",  ">", STATE_CALC        },
    { "Timer",       ">", STATE_TIMER       },
    { "Games",       ">", STATE_GAME        },
    { "Settings",    "*", STATE_SETTINGS    },
    { "Voltmeter",   "!", STATE_VOLTMETER   },
    { "Battery",     "!", STATE_BATTERY     },
};

const int MENU_SIZE = sizeof(menu) / sizeof(menu[0]);
const int ITEM_H = 19;
const int LIST_TOP = 26;
int selectedItem = 0;
int scrollOffset = 0;

static int visibleItems() {
    // Після tft.setRotation() реальна висота змінюється.
    // Рахуємо кількість пунктів, які фізично влазять на екран.
    int h = tft.height();
    int items = (h - LIST_TOP) / ITEM_H;
    if (items < 1) items = 1;
    return items;
}

static void normalizeMenuScroll() {
    int vis = visibleItems();

    // clamp scrollOffset to valid range
    int maxOffset = MENU_SIZE - vis;
    if (maxOffset < 0) maxOffset = 0;

    if (scrollOffset < 0) scrollOffset = 0;
    if (scrollOffset > maxOffset) scrollOffset = maxOffset;

    // ensure selected item is visible
    if (selectedItem < scrollOffset) {
        scrollOffset = selectedItem;
    } else if (selectedItem >= scrollOffset + vis) {
        scrollOffset = selectedItem - vis + 1;
    }

    if (scrollOffset < 0) scrollOffset = 0;
    if (scrollOffset > maxOffset) scrollOffset = maxOffset;
}

static int lastSelected = -1;
static int lastOffset = -1;
static bool firstDraw = true;

void drawShell() {
    int vis = visibleItems();
    int w = tft.width();
    int listH = vis * ITEM_H;

    bool fullRedraw = firstDraw || (lastOffset != scrollOffset);
    
    if (fullRedraw) {
        // Очищаємо тільки область списку, не весь екран
        tft.fillRect(0, LIST_TOP, w, listH, COL_BG);
        
        // Заголовок тільки при першому малюванні
        if (firstDraw) {
            tft.fillRect(0, 0, w, LIST_TOP, COL_BG);
            drawTitle("NEXUS");
            firstDraw = false;
        }
        
        // Малюємо всі пункти
        for (int i = 0; i < vis; i++) {
            int menuIndex = i + scrollOffset;
            if (menuIndex >= MENU_SIZE) break;

            int y = LIST_TOP + i * ITEM_H;

            if (menuIndex == selectedItem) {
                tft.fillRect(0, y, w, ITEM_H, COL_SELECT);
                tft.setTextColor(COL_BG);
            } else {
                tft.setTextColor(COL_TEXT);
            }

            tft.setTextSize(1);
            tft.setCursor(6, y + 6);
            tft.print(menu[menuIndex].icon);
            tft.print(" ");
            tft.print(menu[menuIndex].name);
        }

        // смужка скролу
        tft.fillRect(w - 3, LIST_TOP, 3, listH, COL_BG);
        if (MENU_SIZE > vis) {
            int barHeight = (vis * listH) / MENU_SIZE;
            if (barHeight < 6) barHeight = 6;
            int maxOffset = MENU_SIZE - vis;
            int barY = LIST_TOP + (scrollOffset * (listH - barHeight)) / maxOffset;
            tft.fillRect(w - 3, barY, 3, barHeight, COL_PRIMARY);
        }
    } else if (lastSelected != selectedItem) {
        // Тільки змінився вибір — перемальовуємо 2 рядки
        int oldY = LIST_TOP + (lastSelected - scrollOffset) * ITEM_H;
        int newY = LIST_TOP + (selectedItem - scrollOffset) * ITEM_H;
        
        // Спочатку малюємо новий виділений (чим швидше тим краще)
        tft.fillRect(0, newY, w, ITEM_H, COL_SELECT);
        tft.setTextColor(COL_BG, COL_SELECT);
        tft.setTextSize(1);
        tft.setCursor(6, newY + 6);
        tft.print(menu[selectedItem].icon);
        tft.print(" ");
        tft.print(menu[selectedItem].name);
        
        // Потім знімаємо виділення зі старого
        tft.fillRect(0, oldY, w, ITEM_H, COL_BG);
        tft.setTextColor(COL_TEXT, COL_BG);
        tft.setCursor(6, oldY + 6);
        tft.print(menu[lastSelected].icon);
        tft.print(" ");
        tft.print(menu[lastSelected].name);
    }
    
    lastSelected = selectedItem;
    lastOffset = scrollOffset;
}

void runShell() {
    firstDraw = true;  // скидаємо при вході
    normalizeMenuScroll();
    drawShell();

    const int FRAME_MS = 33; // ~30 FPS
    unsigned long lastFrame = millis();

    while (currentState == STATE_SHELL) {
        // FPS limit
        unsigned long now = millis();
        if (now - lastFrame < FRAME_MS) {
            delay(1);
            continue;
        }
        lastFrame = now;

        char key = getKeyHeld(350, 80);

        if (key == '2') {
            selectedItem--;
            if (selectedItem < 0) selectedItem = MENU_SIZE - 1;

            normalizeMenuScroll();
            firstDraw = true;
            clearScreen();
            drawShell();
        }
        else if (key == '8') {
            selectedItem++;
            if (selectedItem >= MENU_SIZE) selectedItem = 0;

            normalizeMenuScroll();
            firstDraw = true;
            clearScreen();
            drawShell();
        }
        else if (key == 'A') {
            currentState = menu[selectedItem].state;
        }
    }
}
