#include "include/raylib.h"
#include <time.h>
#include <stdbool.h>

typedef struct Object {
    Rectangle rec;
    Color color;
} Object;

// Constants
#define windowWidth 800
#define windowHeight 600
#define racketSpeed 300.0f
#define racketHeigth 100
#define racketWidth 10
#define ballSize 15
#define ballSpeedInit 300.0f
#define ballAcc 10.0f

// Variables
enum Dir {
    LU = 1,
    RU = 2,
    RD = 3,
    LD = 4,
};
enum Dir ballDirection; 
int scoreLeft = 0;
int scoreRight = 0;
bool isGoal = false;
float ballSpeed = ballSpeedInit;

int main() {

    // Window init
    InitWindow(windowWidth, windowHeight, "raypong");
    SetTargetFPS(60);

    // Objects init
    Object racketLeft = {
        {0, windowHeight / 2 + racketHeigth / 2, racketWidth, racketHeigth},
        RAYWHITE
    };

    Object racketRight = {
        {windowWidth - racketWidth, windowHeight / 2 + racketHeigth / 2, racketWidth, racketHeigth},
        RAYWHITE
    };

    Object ball = {
        {windowWidth / 2 + ballSize / 2, windowHeight / 2 + ballSize / 2, ballSize, ballSize},
        RAYWHITE
    };
    ballDirection = time(NULL) % 2 == 0 ? 2 : 1; 

    // Main game loop
    while(!WindowShouldClose()) {
        double delta = GetFrameTime();

        // Player input        

        // Left Racket
        if (IsKeyDown('S')) {
            racketLeft.rec.y += racketSpeed * delta;
        }
        if (racketLeft.rec.y < 0) {
            racketLeft.rec.y = 0;
        }
        if (IsKeyDown('W')) {
            racketLeft.rec.y -= racketSpeed * delta;
        }
        if (racketLeft.rec.y > windowHeight - racketHeigth) {
            racketLeft.rec.y = windowHeight - racketHeigth;
        }

        // Right racket
        if (IsKeyDown(KEY_DOWN)) {
            racketRight.rec.y += racketSpeed * delta;
        }
        if (racketRight.rec.y < 0) {
            racketRight.rec.y = 0;
        }
        if (IsKeyDown(KEY_UP)) {
            racketRight.rec.y -= racketSpeed * delta;
        }
        if (racketRight.rec.y > windowHeight - racketHeigth) {
            racketRight.rec.y = windowHeight - racketHeigth;
        }

        // Calculations

        // Ball movement
        switch (ballDirection) {
        case LU:
            ball.rec.x -= ballSpeed * delta;
            ball.rec.y -= ballSpeed * delta;
            break;
        case RU:
            ball.rec.x += ballSpeed * delta;
            ball.rec.y -= ballSpeed * delta;
            break;
        case RD:
            ball.rec.x += ballSpeed * delta;
            ball.rec.y += ballSpeed * delta;
            break;
        case LD:
            ball.rec.x -= ballSpeed * delta;
            ball.rec.y += ballSpeed * delta;
            break;
        }

        // Ball - screen collision
        if (ball.rec.y <= 0) {
            ball.rec.y = 0;
            if (ballDirection == LU) {
                ballDirection = LD;
            } else {
                ballDirection = RD;
            }
        }
        if (ball.rec.y >= windowHeight - ballSize) {
            ball.rec.y = windowHeight - ballSize;
            if (ballDirection == LD) {
                ballDirection = LU;
            } else {
                ballDirection = RU;
            }
        }

        // Ball - rackets colision
        if (CheckCollisionRecs(ball.rec, racketLeft.rec)) {
            ball.rec.x = 0 + racketWidth;
            ballSpeed += ballAcc;
            if (ballDirection == LU) {
                ballDirection = RU;
            } else {
                ballDirection = RD;
            }
        }
        if (CheckCollisionRecs(ball.rec, racketRight.rec)) {
            ball.rec.x = windowWidth - racketWidth - ballSize;
            ballSpeed += ballAcc;
            if (ballDirection == RU) {
                ballDirection = LU;
            } else {
                ballDirection = LD;
            }
        }

        // Scoring
        if (ball.rec.x + ballSize < 0) {
            scoreRight++;
            isGoal = true;
        }
        if (ball.rec.x > windowWidth) {
            scoreLeft++;
            isGoal = true;
        }
        if (isGoal) {
            ball.rec.x = windowWidth / 2 + ballSize / 2;
            ball.rec.y = windowHeight / 2 + ballSize / 2;
            ballDirection = time(NULL) % 2 == 0 ? 2 : 1; 
            ballSpeed = ballSpeedInit;
            isGoal = false;
        }

        // Render
        BeginDrawing();

        ClearBackground(BLACK);

        DrawLine(windowWidth / 2, 0, windowWidth / 2, windowHeight, GRAY);
        DrawText(TextFormat("%i", scoreLeft), windowWidth / 2 - 50 - MeasureText(TextFormat("%i", scoreLeft), 40) / 2, 50, 40, RAYWHITE);
        DrawText(TextFormat("%i", scoreRight), windowWidth / 2 + 50 - MeasureText(TextFormat("%i", scoreRight), 40) / 2, 50, 40, RAYWHITE);

        DrawRectangleRec(racketLeft.rec, racketLeft.color);
        DrawRectangleRec(racketRight.rec, racketRight.color);
        DrawRectangleRec(ball.rec, ball.color);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
