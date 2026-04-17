#include "game.h"
#include "../display.h"
#include "../keyboard.h"
#include "../shell.h"
#include "../config.h"

// Pong - класична гра
// Гравець зліва (кнопки 2=вгору, 8=вниз), AI справа

#define PADDLE_W 4
#define PADDLE_H 20
#define BALL_SIZE 4
#define BALL_SPEED 2

static int playerY = SCREEN_H / 2 - PADDLE_H / 2;
static int aiY = SCREEN_H / 2 - PADDLE_H / 2;
static int ballX, ballY;
static int ballVX, ballVY;
static int playerScore = 0;
static int aiScore = 0;

static void resetBall() {
    ballX = SCREEN_W / 2 - BALL_SIZE / 2;
    ballY = SCREEN_H / 2 - BALL_SIZE / 2;
    ballVX = (random(0, 2) == 0 ? 1 : -1) * BALL_SPEED;
    ballVY = (random(0, 2) == 0 ? 1 : -1) * (BALL_SPEED - 1);
    if (ballVY == 0) ballVY = 1;
}

static void drawPong() {
    clearScreen();

    int W = tft.width();   // 160 (після rotation)
    int H = tft.height();  // 128 (після rotation)

    // Рамка поля
    tft.drawRect(0, 0, W, H, COL_PRIMARY);

    // Центральна лінія
    for (int y = 2; y < H - 2; y += 8) {
        tft.drawFastVLine(W / 2, y, 4, COL_PRIMARY);
    }

    // Ракетка гравця (ліва)
    tft.fillRect(4, playerY, PADDLE_W, PADDLE_H, COL_PRIMARY);

    // Ракетка AI (права)
    tft.fillRect(W - 4 - PADDLE_W, aiY, PADDLE_W, PADDLE_H, COL_PRIMARY);

    // М'яч
    tft.fillRect(ballX, ballY, BALL_SIZE, BALL_SIZE, COL_PRIMARY);

    // Рахунок
    tft.setTextSize(2);
    tft.setTextColor(COL_PRIMARY);
    char score[8];
    snprintf(score, sizeof(score), "%d", playerScore);
    tft.setCursor(W / 2 - 20, 4);
    tft.print(score);
    snprintf(score, sizeof(score), "%d", aiScore);
    tft.setCursor(W / 2 + 8, 4);
    tft.print(score);

    // Підказка
    tft.setTextSize(1);
    tft.setTextColor(COL_TEXT);
    tft.setCursor(2, H - 10);
    tft.print("2=Up 8=Down D=Exit");
}

// ============ GAMES MENU ============
void runGame() {
    const char* items[] = { "Pong" };
    const int ITEM_COUNT = 1;
    int selected = 0;

    clearScreen();
    drawTitle("Games");

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

    while (currentState == STATE_GAME) {
        char key = getKeyHeld(350, 80);

        if (key == '2') {
            selected--;
            if (selected < 0) selected = ITEM_COUNT - 1;
            clearScreen(); drawTitle("Games");
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
            clearScreen(); drawTitle("Games");
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
            if (selected == 0) currentState = STATE_PONG;
        }
        else if (key == 'B' || key == 'D') {
            currentState = STATE_SHELL;
        }
    }
}

// ============ PONG GAME ============
void runPong() {
    int W = tft.width();   // 160
    int H = tft.height();  // 128

    playerY = H / 2 - PADDLE_H / 2;
    aiY = H / 2 - PADDLE_H / 2;
    playerScore = 0;
    aiScore = 0;

    // Reset ball to center
    ballX = W / 2 - BALL_SIZE / 2;
    ballY = H / 2 - BALL_SIZE / 2;
    ballVX = (random(0, 2) == 0 ? 1 : -1) * BALL_SPEED;
    ballVY = (random(0, 2) == 0 ? 1 : -1) * (BALL_SPEED - 1);
    if (ballVY == 0) ballVY = 1;

    const int FRAME_MS = 33;
    unsigned long lastFrame = 0;

    while (currentState == STATE_PONG) {
        unsigned long now = millis();
        if (now - lastFrame < FRAME_MS) {
            delay(1);
            continue;
        }
        lastFrame = now;

        char key = getKeyHeld(200, 80);
        if (key == '2') {
            playerY -= 6;
            if (playerY < 2) playerY = 2;
        } else if (key == '8') {
            playerY += 6;
            if (playerY > H - PADDLE_H - 2) playerY = H - PADDLE_H - 2;
        } else if (key == 'D') {
            currentState = STATE_GAME;
            return;
        }

        int aiTarget = ballY - PADDLE_H / 2;
        if (aiY < aiTarget - 2) aiY += 2;
        else if (aiY > aiTarget + 2) aiY -= 2;
        if (aiY < 2) aiY = 2;
        if (aiY > H - PADDLE_H - 2) aiY = H - PADDLE_H - 2;

        ballX += ballVX;
        ballY += ballVY;

        // Відбиття від верху/низу (Y)
        if (ballY <= 2 || ballY >= H - BALL_SIZE - 2) {
            ballVY = -ballVY;
            ballY = (ballY <= 2) ? 3 : H - BALL_SIZE - 3;
        }

        // Відбиття від ракетки гравця (ліва, X ~ 4..8)
        if (ballX <= 4 + PADDLE_W && ballX >= 4 &&
            ballY + BALL_SIZE >= playerY && ballY <= playerY + PADDLE_H) {
            ballVX = abs(ballVX) + 1;
            if (ballVX > 4) ballVX = 4;
            int hitPos = (ballY + BALL_SIZE / 2) - playerY;
            ballVY = (hitPos - PADDLE_H / 2) / 3;
            if (ballVY == 0) ballVY = 1;
        }

        // Відбиття від ракетки AI (права, X ~ W-8..W-4)
        if (ballX + BALL_SIZE >= W - 4 - PADDLE_W && ballX <= W - 4 &&
            ballY + BALL_SIZE >= aiY && ballY <= aiY + PADDLE_H) {
            ballVX = -abs(ballVX) - 1;
            if (ballVX < -4) ballVX = -4;
            int hitPos = (ballY + BALL_SIZE / 2) - aiY;
            ballVY = (hitPos - PADDLE_H / 2) / 3;
            if (ballVY == 0) ballVY = 1;
        }

        // Гол
        if (ballX < 0) {
            aiScore++;
            // Reset ball
            ballX = W / 2 - BALL_SIZE / 2;
            ballY = H / 2 - BALL_SIZE / 2;
            ballVX = BALL_SPEED;
            ballVY = (random(0, 2) == 0 ? 1 : -1);
        } else if (ballX > W) {
            playerScore++;
            ballX = W / 2 - BALL_SIZE / 2;
            ballY = H / 2 - BALL_SIZE / 2;
            ballVX = -BALL_SPEED;
            ballVY = (random(0, 2) == 0 ? 1 : -1);
        }

        drawPong();
    }
}
