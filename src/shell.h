#pragma once

#include <Arduino.h>
#include "display.h"
#include "keyboard.h"

// всі можливі стани системи
enum State {
    STATE_SHELL,
    STATE_GAME,
    STATE_PONG,
    STATE_VOLTMETER,
    STATE_THERMOMETER,
    STATE_HUMIDITY,
    STATE_NOISE,
    STATE_WIFI,
    STATE_CALC,
    STATE_TIMER,
    STATE_SETTINGS,
    STATE_SCREENLIGHT,
    STATE_LED,
    STATE_THEME,
    STATE_BATTERY,
    STATE_HELP,
};

// поточний стан — extern бо використовується скрізь
extern State currentState;

// головна функція шелу
void runShell();