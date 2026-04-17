#include <Arduino.h>
#include "config.h"
#include "display.h"
#include "keyboard.h"
#include "shell.h"

// підключаємо всі додатки
#include "apps/stubs.h"
#include "apps/help.h"
#include "apps/wifi_scan.h"
#include "apps/settings.h"
#include "apps/thermometer.h"
#include "apps/humidity.h"
#include "apps/noise.h"
#include "apps/calculator.h"
#include "apps/timer.h"
#include "apps/game.h"

void setup() {
    Serial.begin(115200);
    Serial.print("MOSI: "); Serial.println(MOSI);
    Serial.print("MISO: "); Serial.println(MISO);
    Serial.print("SCK: ");  Serial.println(SCK);
    Serial.print("SS: ");   Serial.println(SS);
    
    initDisplay();
    initKeyboard();

    // заставка при запуску
    drawCenteredText("NEXUS", 60, COL_PRIMARY, 3);
    drawCenteredText("v1.0", 90, COL_TEXT, 1);
    delay(1500);
}

void loop() {
    switch (currentState) {
        case STATE_SHELL:       runShell();       break;
        case STATE_GAME:        runGame();        break;
        case STATE_PONG:        runPong();        break;
        case STATE_VOLTMETER:   runVoltmeter();   break;
        case STATE_THERMOMETER: runThermometer(); break;
        case STATE_HUMIDITY:    runHumidity();    break;
        case STATE_NOISE:       runNoise();       break;
        case STATE_WIFI:        runWifiScan();    break;
        case STATE_CALC:        runCalculator();  break;
        case STATE_TIMER:       runTimer();       break;
        case STATE_SETTINGS:    runSettings();    break;
        case STATE_SCREENLIGHT: runScreenlight(); break;
        case STATE_LED:         runLed();         break;
        case STATE_THEME:       runTheme();       break;
        case STATE_BATTERY:     runBattery();     break;
        case STATE_HELP:        runHelp();        break;
    }
}
