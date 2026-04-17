#include "timer.h"
#include "../display.h"
#include "../keyboard.h"
#include "../shell.h"
#include "../config.h"

enum TimerState { TIMER_INPUT, TIMER_RUNNING, TIMER_PAUSED, TIMER_DONE };

static void drawTimerScreen(TimerState state, int inputSec, int remainingSec) {
    clearScreen();
    drawTitle("Timer");

    int centerY = 60;

    if (state == TIMER_INPUT) {
        // Режим вводу
        tft.setTextSize(1);
        tft.setTextColor(COL_TEXT);
        tft.setCursor(4, 30);
        tft.print("Enter seconds:");

        tft.setTextSize(3);
        tft.setTextColor(COL_PRIMARY);
        char buf[12];
        snprintf(buf, sizeof(buf), "%d", inputSec);
        int w = strlen(buf) * 6 * 3;
        tft.setCursor((SCREEN_W - w) / 2, centerY - 10);
        tft.print(buf);

        tft.setTextSize(1);
        tft.setTextColor(COL_TEXT);
        tft.setCursor(4, SCREEN_H - 12);
        tft.print("#=Clear A=Start D=Back");
    } else {
        // Режим відліку
        int mins = remainingSec / 60;
        int secs = remainingSec % 60;

        tft.setTextSize(4);
        tft.setTextColor(state == TIMER_DONE ? ST77XX_RED : COL_PRIMARY);
        char buf[16];
        snprintf(buf, sizeof(buf), "%02d:%02d", mins, secs);
        int w = strlen(buf) * 6 * 4;
        tft.setCursor((SCREEN_W - w) / 2, centerY - 15);
        tft.print(buf);

        if (state == TIMER_PAUSED) {
            tft.setTextSize(1);
            tft.setTextColor(ST77XX_YELLOW);
            tft.setCursor((SCREEN_W - 36) / 2, centerY + 30);
            tft.print("PAUSED");
        } else if (state == TIMER_DONE) {
            tft.setTextSize(1);
            tft.setTextColor(ST77XX_RED);
            tft.setCursor((SCREEN_W - 40) / 2, centerY + 30);
            tft.print("TIME UP!");
        }

        tft.setTextSize(1);
        tft.setTextColor(COL_TEXT);
        tft.setCursor(4, SCREEN_H - 12);
        if (state == TIMER_RUNNING) {
            tft.print("A=Pause B=Stop D=Back");
        } else if (state == TIMER_PAUSED) {
            tft.print("A=Resume B=Stop D=Back");
        } else {
            tft.print("A=Restart B=Stop D=Back");
        }
    }
}

void runTimer() {
    TimerState state = TIMER_INPUT;
    int inputSec = 0;
    int totalSec = 0;
    int remainingSec = 0;
    unsigned long lastTick = 0;

    const int FRAME_MS = 100;
    unsigned long lastFrame = 0;

    while (currentState == STATE_TIMER) {
        unsigned long now = millis();
        if (now - lastFrame < FRAME_MS) {
            delay(1);
            continue;
        }
        lastFrame = now;

        // Тікання таймера
        if (state == TIMER_RUNNING) {
            if (now - lastTick >= 1000) {
                lastTick = now;
                remainingSec--;
                if (remainingSec <= 0) {
                    remainingSec = 0;
                    state = TIMER_DONE;
                    // Сигнал
                    for (int i = 0; i < 3; i++) {
                        digitalWrite(PIN_BUZZER, HIGH);
                        delay(100);
                        digitalWrite(PIN_BUZZER, LOW);
                        delay(100);
                    }
                }
            }
        }

        drawTimerScreen(state, inputSec, remainingSec);

        char key = getKeyHeld(350, 120);

        if (state == TIMER_INPUT) {
            if (key >= '0' && key <= '9') {
                int d = key - '0';
                if (inputSec < 10000) {
                    inputSec = inputSec * 10 + d;
                }
            } else if (key == '#') {
                inputSec = 0;
            } else if (key == 'A' && inputSec > 0) {
                totalSec = inputSec;
                remainingSec = inputSec;
                state = TIMER_RUNNING;
                lastTick = millis();
            } else if (key == 'D') {
                currentState = STATE_SHELL;
            }
        } else if (state == TIMER_RUNNING) {
            if (key == 'A') {
                state = TIMER_PAUSED;
            } else if (key == 'B') {
                state = TIMER_INPUT;
                inputSec = 0;
            } else if (key == 'D') {
                currentState = STATE_SHELL;
            }
        } else if (state == TIMER_PAUSED) {
            if (key == 'A') {
                state = TIMER_RUNNING;
                lastTick = millis();
            } else if (key == 'B') {
                state = TIMER_INPUT;
                inputSec = 0;
            } else if (key == 'D') {
                currentState = STATE_SHELL;
            }
        } else if (state == TIMER_DONE) {
            if (key == 'A') {
                remainingSec = totalSec;
                state = TIMER_RUNNING;
                lastTick = millis();
            } else if (key == 'B') {
                state = TIMER_INPUT;
                inputSec = 0;
            } else if (key == 'D') {
                currentState = STATE_SHELL;
            }
        }
    }
}
