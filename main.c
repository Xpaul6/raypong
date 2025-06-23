#include "include/raylib.h"
#include <time.h>
#include <stdlib.h>
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
bool isPaused = true;

int main() {

    // Window init
    InitWindow(windowWidth, windowHeight, "raypong");
    SetTargetFPS(60);

    int initialRacketLeftX = 0;
    int initialRacketLeftY = windowHeight / 2 - racketHeigth / 2;
    int initialRacketRightX = windowWidth - racketWidth;
    int initialRacketRightY = windowHeight / 2 - racketHeigth / 2;
    int initialBallX = windowWidth / 2 - ballSize / 2;
    int initialBallY = windowHeight / 2 - ballSize / 2;

    // Objects init
    Object racketLeft = {
        {initialRacketLeftX, initialRacketLeftY, racketWidth, racketHeigth},
        RAYWHITE
    };

    Object racketRight = {
        {initialRacketRightX, initialRacketRightY, racketWidth, racketHeigth},
        RAYWHITE
    };

    Object ball = {
        {initialBallX, initialBallY, ballSize, ballSize},
        RAYWHITE
    };
    srand(time(NULL));
    ballDirection = rand() % 4 + 1; 

    // Main game loop
    while(!WindowShouldClose()) {
        if (isPaused) {
            if (IsKeyDown(KEY_SPACE)) {
                isPaused = false;
            }
            BeginDrawing();

            ClearBackground(BLACK);

            DrawLine(windowWidth / 2, 0, windowWidth / 2, windowHeight, GRAY);
            DrawText(TextFormat("%i", scoreLeft), windowWidth / 2 - 50 - MeasureText(TextFormat("%i", scoreLeft), 40) / 2, 50, 40, RAYWHITE);
            DrawText(TextFormat("%i", scoreRight), windowWidth / 2 + 50 - MeasureText(TextFormat("%i", scoreRight), 40) / 2, 50, 40, RAYWHITE);

            DrawRectangleRec(racketLeft.rec, racketLeft.color);
            DrawRectangleRec(racketRight.rec, racketRight.color);
            DrawRectangleRec(ball.rec, ball.color);

            DrawText("Press SPACE to play", windowWidth / 2 - MeasureText("Press SPACE to play", 20) / 2, 5, 20, RAYWHITE);

            EndDrawing();

            continue;
        }

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
            ball.rec.x = initialBallX;
            ball.rec.y = initialBallY;
            ballDirection = rand() % 4 + 1; 
            ballSpeed = ballSpeedInit;

            racketLeft.rec.x = initialRacketLeftX;
            racketLeft.rec.y = initialRacketLeftY;
            racketRight.rec.x = initialRacketRightX;
            racketRight.rec.y = initialRacketRightY;
            isGoal = false;
            isPaused = true;
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
