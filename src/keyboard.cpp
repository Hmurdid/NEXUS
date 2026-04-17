#include "keyboard.h"

// розкладка матриці 4x4
// рядки × стовпці
const char keys[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

const int rows[4] = {KBD_R1, KBD_R2, KBD_R3, KBD_R4};
const int cols[4] = {KBD_C1, KBD_C2, KBD_C3, KBD_C4};

void initKeyboard() {
    // рядки — виходи
    for (int i = 0; i < 4; i++) {
        pinMode(rows[i], OUTPUT);
        digitalWrite(rows[i], HIGH);
    }
    // стовпці — входи з підтяжкою
    for (int i = 0; i < 4; i++) {
        pinMode(cols[i], INPUT_PULLUP);
    }
}

char getKeyRaw() {
    for (int r = 0; r < 4; r++) {
        digitalWrite(rows[r], LOW);
        for (int c = 0; c < 4; c++) {
            if (digitalRead(cols[c]) == LOW) {
                digitalWrite(rows[r], HIGH);
                return keys[r][c];
            }
        }
        digitalWrite(rows[r], HIGH);
    }
    return 0;
}

char getKey() {
    char k = getKeyRaw();
    if (k) {
        delay(20); // антидребезг
        while (getKeyRaw() == k); // чекаємо відпускання
        return k;
    }
    return 0;
}

char getKeyHeld(int holdDelayMs, int repeatMs) {
    static char lastKey = 0;
    static unsigned long pressStart = 0;
    static unsigned long lastRepeat = 0;
    static bool inHold = false;

    char k = getKeyRaw();

    if (k == 0) {
        // кнопка відпущена — скидаємо стан
        lastKey = 0;
        inHold = false;
        return 0;
    }

    unsigned long now = millis();

    if (k != lastKey) {
        // нова кнопка
        lastKey = k;
        pressStart = now;
        lastRepeat = now;
        inHold = false;
        delay(20); // антидребезг
        return k; // перше натискання
    }

    // та сама кнопка тримається
    if (!inHold) {
        if (now - pressStart >= holdDelayMs) {
            inHold = true;
            lastRepeat = now;
            return k;
        }
        return 0;
    }

    // вже в режимі повтору
    if (now - lastRepeat >= repeatMs) {
        lastRepeat = now;
        return k;
    }

    return 0;
}

int getKeys(char* outKeys, int maxKeys) {
    int count = 0;

    for (int r = 0; r < 4; r++) {
        digitalWrite(rows[r], LOW);
        for (int c = 0; c < 4; c++) {
            if (digitalRead(cols[c]) == LOW) {
                if (count < maxKeys) {
                    outKeys[count++] = keys[r][c];
                }
            }
        }
        digitalWrite(rows[r], HIGH);
    }

    return count;
}

bool keyPressed(char key) {
    return getKey() == key;
}

void waitForKey(char key) {
    while (getKey() != key);

}

/*

Як працює матриця
Принцип простий — по черзі подаємо `LOW` на кожен рядок і перевіряємо чи є `LOW` на стовпцях:

     C1   C2   C3   C4
R1 [ 1  ][ 2  ][ 3  ][ A  ]
R2 [ 4  ][ 5  ][ 6  ][ B  ]  ← подали LOW на R2
R3 [ 7  ][ 8  ][ 9  ][ C  ]     читаємо C1,C2,C3,C4
R4 [ *  ][ 0  ][ #  ][ D  ]     якщо C2 = LOW → натиснуто '5'

*/