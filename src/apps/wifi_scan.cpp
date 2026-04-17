#include "wifi_scan.h"
#include "../display.h"
#include "../keyboard.h"
#include "../shell.h"
#include <WiFi.h>

// Сканує мережі та показує список
void drawWifiList(int offset, int selected) {
    clearScreen();
    drawTitle("WiFi Scan");
    
    int n = WiFi.scanComplete();
    if (n <= 0) {
        drawCenteredText("Scanning...", 70, COL_TEXT, 1);
        return;
    }
    
    int vis = (tft.height() - 28) / 18;
    if (vis < 1) vis = 1;
    
    for (int i = 0; i < vis; i++) {
        int idx = offset + i;
        if (idx >= n) break;
        
        int y = 28 + i * 18;
        
        if (idx == selected) {
            tft.fillRect(0, y, tft.width(), 18, COL_SELECT);
            tft.setTextColor(COL_BG);
        } else {
            tft.setTextColor(COL_TEXT);
        }
        
        tft.setTextSize(1);
        tft.setCursor(2, y + 2);
        
        // SSID (обрізаємо до 12 символів)
        String ssid = WiFi.SSID(idx);
        if (ssid.length() == 0) ssid = "(hidden)";
        if (ssid.length() > 12) ssid = ssid.substring(0, 12);
        tft.print(ssid);
        
        // RSSI (сила сигналу)
        int rssi = WiFi.RSSI(idx);
        char rssiStr[8];
        snprintf(rssiStr, sizeof(rssiStr), "%ddB", rssi);
        tft.setCursor(tft.width() - 42, y + 2);
        tft.print(rssiStr);
        
        // Іконка шифрування
        wifi_auth_mode_t enc = WiFi.encryptionType(idx);
        tft.setCursor(tft.width() - 48, y + 2);
        if (enc == WIFI_AUTH_OPEN) {
            tft.print("O "); // Open
        } else {
            tft.print("* "); // Secured
        }
    }
    
    // Номер сторінки
    char page[16];
    snprintf(page, sizeof(page), "%d/%d networks", n > 0 ? n : 0, n > 0 ? n : 0);
    tft.setTextColor(COL_PRIMARY);
    tft.setTextSize(1);
    int pageW = strlen(page) * 6;
    tft.setCursor((tft.width() - pageW) / 2, tft.height() - 10);
    tft.print(page);
}

void runWifiScan() {
    // Запуск сканування
    WiFi.mode(WIFI_STA);
    WiFi.scanNetworks(true); // асинхронно
    
    int selected = 0;
    int offset = 0;
    int totalNetworks = 0;
    bool scanning = true;
    int lastSelected = -1;
    int lastOffset = -1;
    bool needRedraw = true;

    const int FRAME_MS = 100; // ~10 FPS
    unsigned long lastFrame = 0;
    
    while (currentState == STATE_WIFI) {
        // FPS limit
        unsigned long now = millis();
        if (now - lastFrame < FRAME_MS) {
            delay(1);
            continue;
        }
        lastFrame = now;
        
        // Перевіряємо чи сканування завершено
        int n = WiFi.scanComplete();
        if (n >= 0) {
            scanning = false;
            needRedraw = true;
            totalNetworks = n;
        }
        
        if (needRedraw || selected != lastSelected || offset != lastOffset) {
            drawWifiList(offset, selected);
            lastSelected = selected;
            lastOffset = offset;
            needRedraw = false;
        }
        
        char key = getKeyHeld(350, 150);
        
        if (key == '2') {
            // вгору
            selected--;
            if (selected < 0) {
                selected = totalNetworks - 1;
                int vis = (tft.height() - 28) / 18;
                offset = totalNetworks - vis;
                if (offset < 0) offset = 0;
            } else if (selected < offset) {
                offset--;
            }
        }
        else if (key == '8') {
            // вниз
            if (totalNetworks > 0) {
                selected++;
                if (selected >= totalNetworks) {
                    selected = 0;
                    offset = 0;
                } else {
                    int vis = (tft.height() - 28) / 18;
                    if (selected >= offset + vis) {
                        offset++;
                    }
                }
            }
        }
        else if (key == 'A') {
            // пересканувати
            WiFi.scanDelete();
            WiFi.scanNetworks(true);
            scanning = true;
            selected = 0;
            offset = 0;
        }
        else if (key == 'B' || key == 'D') {
            // вихід
            WiFi.scanDelete();
            WiFi.mode(WIFI_OFF);
            currentState = STATE_SHELL;
        }
    }
}
