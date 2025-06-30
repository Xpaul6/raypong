#include "include/raylib.h"
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

// Structs
typedef struct Object {
    Rectangle rec;
    Color color;
} Object;

// Enums
enum Dir {
    LU = 1,
    RU = 2,
    RD = 3,
    LD = 4,
};

// Constants
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define RACKET_SPEED 300.0f
#define RACKET_HEIGHT 100
#define RACKET_WIDTH 10
#define BALL_SIZE 15
#define INIT_BALL_SPEED 300.0f
#define BALL_ACCELERATION 10.0f

int main() {

    // Window init
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "raypong");
    SetTargetFPS(60);

    enum Dir ballDirection; 
    int scoreLeft = 0;
    int scoreRight = 0;
    bool isGoal = false;
    float ballSpeed = INIT_BALL_SPEED;
    bool isPaused = true;

    int initialRacketLeftX = 0;
    int initialRacketLeftY = WINDOW_HEIGHT / 2 - RACKET_HEIGHT / 2;
    int initialRacketRightX = WINDOW_WIDTH - RACKET_WIDTH;
    int initialRacketRightY = WINDOW_HEIGHT / 2 - RACKET_HEIGHT / 2;
    int initialBallX = WINDOW_WIDTH / 2 - BALL_SIZE / 2;
    int initialBallY = WINDOW_HEIGHT / 2 - BALL_SIZE / 2;

    // Objects init
    Object racketLeft = {
        {initialRacketLeftX, initialRacketLeftY, RACKET_WIDTH, RACKET_HEIGHT},
        RAYWHITE
    };

    Object racketRight = {
        {initialRacketRightX, initialRacketRightY, RACKET_WIDTH, RACKET_HEIGHT},
        RAYWHITE
    };

    Object ball = {
        {initialBallX, initialBallY, BALL_SIZE, BALL_SIZE},
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

            DrawLine(WINDOW_WIDTH / 2, 0, WINDOW_WIDTH / 2, WINDOW_HEIGHT, GRAY);
            DrawText(TextFormat("%i", scoreLeft), WINDOW_WIDTH / 2 - 50 - MeasureText(TextFormat("%i", scoreLeft), 40) / 2, 50, 40, RAYWHITE);
            DrawText(TextFormat("%i", scoreRight), WINDOW_WIDTH / 2 + 50 - MeasureText(TextFormat("%i", scoreRight), 40) / 2, 50, 40, RAYWHITE);

            DrawRectangleRec(racketLeft.rec, racketLeft.color);
            DrawRectangleRec(racketRight.rec, racketRight.color);
            DrawRectangleRec(ball.rec, ball.color);

            DrawText("Press SPACE to play", WINDOW_WIDTH / 2 - MeasureText("Press SPACE to play", 20) / 2, 5, 20, RAYWHITE);

            EndDrawing();

            continue;
        }

        double delta = GetFrameTime();

        // Player input        

        // Left Racket
        if (IsKeyDown('S')) {
            racketLeft.rec.y += RACKET_SPEED * delta;
        }
        if (racketLeft.rec.y < 0) {
            racketLeft.rec.y = 0;
        }
        if (IsKeyDown('W')) {
            racketLeft.rec.y -= RACKET_SPEED * delta;
        }
        if (racketLeft.rec.y > WINDOW_HEIGHT - RACKET_HEIGHT) {
            racketLeft.rec.y = WINDOW_HEIGHT - RACKET_HEIGHT;
        }

        // Right racket
        if (IsKeyDown(KEY_DOWN)) {
            racketRight.rec.y += RACKET_SPEED * delta;
        }
        if (racketRight.rec.y < 0) {
            racketRight.rec.y = 0;
        }
        if (IsKeyDown(KEY_UP)) {
            racketRight.rec.y -= RACKET_SPEED * delta;
        }
        if (racketRight.rec.y > WINDOW_HEIGHT - RACKET_HEIGHT) {
            racketRight.rec.y = WINDOW_HEIGHT - RACKET_HEIGHT;
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
        if (ball.rec.y >= WINDOW_HEIGHT - BALL_SIZE) {
            ball.rec.y = WINDOW_HEIGHT - BALL_SIZE;
            if (ballDirection == LD) {
                ballDirection = LU;
            } else {
                ballDirection = RU;
            }
        }

        // Ball - rackets colision
        if (CheckCollisionRecs(ball.rec, racketLeft.rec)) {
            ball.rec.x = 0 + RACKET_WIDTH;
            ballSpeed += BALL_ACCELERATION;
            if (ballDirection == LU) {
                ballDirection = RU;
            } else {
                ballDirection = RD;
            }
        }
        if (CheckCollisionRecs(ball.rec, racketRight.rec)) {
            ball.rec.x = WINDOW_WIDTH - RACKET_WIDTH - BALL_SIZE;
            ballSpeed += BALL_ACCELERATION;
            if (ballDirection == RU) {
                ballDirection = LU;
            } else {
                ballDirection = LD;
            }
        }

        // Scoring
        if (ball.rec.x + BALL_SIZE < 0) {
            scoreRight++;
            isGoal = true;
        }
        if (ball.rec.x > WINDOW_WIDTH) {
            scoreLeft++;
            isGoal = true;
        }
        if (isGoal) {
            ball.rec.x = initialBallX;
            ball.rec.y = initialBallY;
            ballDirection = rand() % 4 + 1; 
            ballSpeed = INIT_BALL_SPEED;

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

        DrawLine(WINDOW_WIDTH / 2, 0, WINDOW_WIDTH / 2, WINDOW_HEIGHT, GRAY);
        DrawText(TextFormat("%i", scoreLeft), WINDOW_WIDTH / 2 - 50 - MeasureText(TextFormat("%i", scoreLeft), 40) / 2, 50, 40, RAYWHITE);
        DrawText(TextFormat("%i", scoreRight), WINDOW_WIDTH / 2 + 50 - MeasureText(TextFormat("%i", scoreRight), 40) / 2, 50, 40, RAYWHITE);

        DrawRectangleRec(racketLeft.rec, racketLeft.color);
        DrawRectangleRec(racketRight.rec, racketRight.color);
        DrawRectangleRec(ball.rec, ball.color);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
