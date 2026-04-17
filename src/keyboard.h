#pragma once

#include <Arduino.h>
#include "config.h"

// ініціалізація матриці
void initKeyboard();

// повертає символ натиснутої кнопки або 0 якщо нічого
char getKey();

// неблокуюче читання — повертає що натиснуто прямо зараз
char getKeyRaw();

// зажата кнопка з повтором
// повертає символ кожен repeatMs мс після початкової затримки holdDelayMs
char getKeyHeld(int holdDelayMs = 400, int repeatMs = 100);

// зчитує всі натиснуті кнопки в масив (max 16)
// повертає кількість знайдених кнопок
int getKeys(char* outKeys, int maxKeys);

// чекає поки натиснуть конкретну кнопку
void waitForKey(char key);

// перевіряє чи натиснута конкретна кнопка прямо зараз
bool keyPressed(char key);
