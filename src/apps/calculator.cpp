#include "calculator.h"
#include "../display.h"
#include "../keyboard.h"
#include "../shell.h"

#include <ctype.h>
#include <math.h>

// Простий парсер виразів з пріоритетами: + - * /
// Підтримка десяткових через '.'

static const int MAX_EXPR = 32;

static void skipSpaces(const char*& p) {
    while (*p == ' ') p++;
}

static bool parseNumber(const char*& p, double& out) {
    skipSpaces(p);

    bool neg = false;
    if (*p == '+') { p++; }
    else if (*p == '-') { neg = true; p++; }

    skipSpaces(p);

    // число: digits [ . digits ]
    bool hasDigit = false;
    double val = 0.0;

    while (isdigit((unsigned char)*p)) {
        hasDigit = true;
        val = val * 10.0 + (*p - '0');
        p++;
    }

    if (*p == '.') {
        p++;
        double place = 0.1;
        while (isdigit((unsigned char)*p)) {
            hasDigit = true;
            val += (*p - '0') * place;
            place *= 0.1;
            p++;
        }
    }

    if (!hasDigit) return false;
    out = neg ? -val : val;
    return true;
}

static bool parseFactor(const char*& p, double& out) {
    skipSpaces(p);
    if (*p == '(') {
        p++;
        // expr
        double v;
        if (!parseFactor(p, v)) {
            // try parse full expr later via parseExpr
        }
        // fallback: use parseExpr
    }

    // тільки number (без дужок щоб було компактно)
    return parseNumber(p, out);
}

static bool parseTerm(const char*& p, double& out) {
    if (!parseFactor(p, out)) return false;
    while (true) {
        skipSpaces(p);
        char op = *p;
        if (op != '*' && op != '/') break;
        p++;
        double rhs;
        if (!parseFactor(p, rhs)) return false;
        if (op == '*') out *= rhs;
        else {
            if (fabs(rhs) < 1e-12) return false;
            out /= rhs;
        }
    }
    return true;
}

static bool parseExpr(const char* s, double& out) {
    const char* p = s;
    if (!parseTerm(p, out)) return false;
    while (true) {
        skipSpaces(p);
        char op = *p;
        if (op != '+' && op != '-') break;
        p++;
        double rhs;
        if (!parseTerm(p, rhs)) return false;
        if (op == '+') out += rhs;
        else out -= rhs;
    }
    skipSpaces(p);
    return (*p == '\0');
}

static void drawCalc(const char* expr, const char* result) {
    clearScreen();
    drawTitle("Calculator");

    // expr box
    tft.drawRect(2, 28, SCREEN_W - 4, 32, COL_PRIMARY);
    tft.setTextSize(2);
    tft.setTextColor(COL_TEXT);
    tft.setCursor(6, 36);
    tft.print(expr);

    // result
    tft.setTextSize(1);
    tft.setTextColor(COL_PRIMARY);
    tft.setCursor(4, 70);
    tft.print("=");
    tft.setTextColor(COL_TEXT);
    tft.setCursor(14, 70);
    tft.print(result);

    // hint
    tft.setTextColor(COL_PRIMARY);
    tft.setCursor(4, SCREEN_H - 12);
    tft.print("#=AC  D=(=)/Back");
}

static void appendChar(char* expr, char ch) {
    int len = (int)strlen(expr);
    if (len >= MAX_EXPR - 1) return;
    expr[len] = ch;
    expr[len + 1] = 0;
}

static void backspace(char* expr) {
    int len = (int)strlen(expr);
    if (len <= 0) return;
    expr[len - 1] = 0;
}

void runCalculator() {
    char expr[MAX_EXPR] = {0};
    char result[24] = {0};
    bool needRedraw = true;

    const int FRAME_MS = 33;
    unsigned long lastFrame = 0;

    while (currentState == STATE_CALC) {
        unsigned long now = millis();
        if (now - lastFrame < FRAME_MS) {
            delay(1);
            continue;
        }
        lastFrame = now;

        if (needRedraw) {
            drawCalc(expr, result);
            needRedraw = false;
        }

        char key = getKeyHeld(350, 120);
        if (!key) continue;

        // digits
        if (key >= '0' && key <= '9') {
            // 0 у help був як кома — робимо десяткову крапку через 0 при довгому натисканні не будемо.
            appendChar(expr, key);
            needRedraw = true;
            continue;
        }

        // operators from keypad letters
        if (key == 'A') { appendChar(expr, '+'); needRedraw = true; continue; }
        if (key == 'B') { appendChar(expr, '-'); needRedraw = true; continue; }
        if (key == 'C') { appendChar(expr, '*'); needRedraw = true; continue; }
        if (key == '*') { appendChar(expr, '/'); needRedraw = true; continue; }

        // decimal point
        if (key == '0') {
            // allow 0 as digit already handled; keep here unreachable
        }
        if (key == '#') {
            // AC
            expr[0] = 0;
            result[0] = 0;
            needRedraw = true;
            continue;
        }

        if (key == 'D') {
            if (expr[0] == 0) {
                currentState = STATE_SHELL;
                continue;
            }

            double v;
            if (parseExpr(expr, v)) {
                // формат: якщо ціле — без .0
                if (fabs(v - round(v)) < 1e-9) {
                    snprintf(result, sizeof(result), "%.0f", v);
                } else {
                    snprintf(result, sizeof(result), "%.4f", v);
                }
            } else {
                snprintf(result, sizeof(result), "ERR");
            }
            needRedraw = true;
            continue;
        }

        // backspace на довгому натисканні B? (поки просто B оператор)
    }
}
