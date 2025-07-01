#include "include/raylib.h"
#include <stdbool.h>

// Structs
typedef struct Object {
    Rectangle rec;
    Color color;
} Object;

// Enums
enum Dir {
    LU = 1, // Left - Up
    RU = 2, // Right - Up
    RD = 3, // Right - Down
    LD = 4, // Left - Down
};

// Constants
#define TARGET_FPS 60
#define WINDOW_NAME "raypong"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define RACKET_SPEED 300.0f
#define RACKET_HEIGHT 100
#define RACKET_WIDTH 10
#define BALL_SIZE 15
#define INIT_BALL_SPEED 300.0f
#define BALL_ACCELERATION 10.0f

#define INIT_RACKET_LEFT_X 0
#define INIT_RACKET_LEFT_Y WINDOW_HEIGHT / 2 - RACKET_HEIGHT / 2
#define INIT_RACKET_RIGHT_X WINDOW_WIDTH - RACKET_WIDTH
#define INIT_RACKET_RIGHT_Y WINDOW_HEIGHT / 2 - RACKET_HEIGHT / 2
#define INIT_BALL_X WINDOW_WIDTH / 2 - BALL_SIZE / 2
#define INIT_BALL_Y WINDOW_HEIGHT / 2 - BALL_SIZE / 2

// Functions
void render_pause_menu(bool* isPaused, Object ball, Object racketLeft, Object racketRight, int scoreLeft, int scoreRight) {
        if (IsKeyDown(KEY_SPACE)) {
            *isPaused = false;
            return;
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
}

void handle_player_input(float delta, Object* racketLeft, Object* racketRight) {
    // Left Racket
    if (IsKeyDown('S')) {
        racketLeft->rec.y += RACKET_SPEED * delta;
    }
    if (racketLeft->rec.y < 0) {
        racketLeft->rec.y = 0;
    }
    if (IsKeyDown('W')) {
        racketLeft->rec.y -= RACKET_SPEED * delta;
    }
    if (racketLeft->rec.y > WINDOW_HEIGHT - RACKET_HEIGHT) {
        racketLeft->rec.y = WINDOW_HEIGHT - RACKET_HEIGHT;
    }

    // Right racket
    if (IsKeyDown(KEY_DOWN)) {
        racketRight->rec.y += RACKET_SPEED * delta;
    }
    if (racketRight->rec.y < 0) {
        racketRight->rec.y = 0;
    }
    if (IsKeyDown(KEY_UP)) {
        racketRight->rec.y -= RACKET_SPEED * delta;
    }
    if (racketRight->rec.y > WINDOW_HEIGHT - RACKET_HEIGHT) {
        racketRight->rec.y = WINDOW_HEIGHT - RACKET_HEIGHT;
    }
}

void calc_ball_move(Object* ball, enum Dir ballDirection, float ballSpeed, double delta) {
    switch (ballDirection) {
        case LU:
            ball->rec.x -= ballSpeed * delta;
            ball->rec.y -= ballSpeed * delta;
            break;
        case RU:
            ball->rec.x += ballSpeed * delta;
            ball->rec.y -= ballSpeed * delta;
            break;
        case RD:
            ball->rec.x += ballSpeed * delta;
            ball->rec.y += ballSpeed * delta;
            break;
        case LD:
            ball->rec.x -= ballSpeed * delta;
            ball->rec.y += ballSpeed * delta;
            break;
    }
}

void calc_ball_screen_collision(Object* ball, enum Dir* ballDirection) {
        if (ball->rec.y <= 0) {
            ball->rec.y = 0;
            if (*ballDirection == LU) {
                *ballDirection = LD;
            } else {
                *ballDirection = RD;
            }
        }
        if (ball->rec.y >= WINDOW_HEIGHT - BALL_SIZE) {
            ball->rec.y = WINDOW_HEIGHT - BALL_SIZE;
            if (*ballDirection == LD) {
                *ballDirection = LU;
            } else {
                *ballDirection = RU;
            }
        }
}

void calc_ball_racket_collision(Object* ball, Object* racketLeft, Object* racketRight, enum Dir* ballDirection, float* ballSpeed) {
    if (CheckCollisionRecs(ball->rec, racketLeft->rec)) {
        ball->rec.x = 0 + RACKET_WIDTH;
        *ballSpeed += BALL_ACCELERATION;
        if (*ballDirection == LU) {
            *ballDirection = RU;
        } else {
            *ballDirection = RD;
        }
    }
    if (CheckCollisionRecs(ball->rec, racketRight->rec)) {
        ball->rec.x = WINDOW_WIDTH - RACKET_WIDTH - BALL_SIZE;
        *ballSpeed += BALL_ACCELERATION;
        if (*ballDirection == RU) {
            *ballDirection = LU;
        } else {
            *ballDirection = LD;
        }
    }
}

void check_scoring(Object* ball, Object* racketLeft, Object* racketRight, enum Dir* ballDirection, float* ballSpeed, int* scoreLeft, int* scoreRight, bool* isGoal, bool* isPaused) {
        if (ball->rec.x + BALL_SIZE < 0) {
            *scoreRight += 1;
            *isGoal = true;
        }
        if (ball->rec.x > WINDOW_WIDTH) {
            *scoreLeft += 1;
            *isGoal = true;
        }
        if (*isGoal) {
            ball->rec.x = INIT_BALL_X;
            ball->rec.y = INIT_BALL_Y;
            *ballDirection = GetRandomValue(1, 4); 
            *ballSpeed = INIT_BALL_SPEED;

            racketLeft->rec.x = INIT_RACKET_LEFT_X;
            racketLeft->rec.y = INIT_RACKET_LEFT_Y;
            racketRight->rec.x = INIT_RACKET_RIGHT_X;
            racketRight->rec.y = INIT_RACKET_RIGHT_Y;
            *isGoal = false;
            *isPaused = true;
        }
}

void render_game(Object ball, Object racketLeft, Object racketRight, int scoreLeft, int scoreRight) {
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

// Entry point
int main() {
    // Window init
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME); 
    SetTargetFPS(TARGET_FPS);

    // Objects init
    Object racketLeft = {
        { INIT_RACKET_LEFT_X, INIT_RACKET_LEFT_Y, RACKET_WIDTH, RACKET_HEIGHT },
        RAYWHITE
    };

    Object racketRight = {
        { INIT_RACKET_RIGHT_X, INIT_RACKET_RIGHT_Y, RACKET_WIDTH, RACKET_HEIGHT },
        RAYWHITE
    };

    Object ball = {
        { INIT_BALL_X, INIT_BALL_Y, BALL_SIZE, BALL_SIZE },
        RAYWHITE
    };

    // Variables definition
    enum Dir ballDirection = GetRandomValue(1, 4);
    int scoreLeft = 0;
    int scoreRight = 0;
    bool isGoal = false;
    float ballSpeed = INIT_BALL_SPEED;
    bool isPaused = true;

    // Main game loop
    while(!WindowShouldClose()) {
        if (isPaused) {
            render_pause_menu(&isPaused, ball, racketLeft, racketRight, scoreLeft, scoreRight);
            continue;
        }

        double delta = GetFrameTime();

        // Player input        
        handle_player_input(delta, &racketLeft, &racketRight);

        // Calculations

        calc_ball_move(&ball, ballDirection, ballSpeed, delta);

        calc_ball_screen_collision(&ball, &ballDirection);

        calc_ball_racket_collision(&ball, &racketLeft, &racketRight, &ballDirection, &ballSpeed);

        check_scoring(&ball, &racketLeft, &racketRight, &ballDirection, &ballSpeed, &scoreLeft, &scoreRight, &isGoal, &isPaused);

        // Render
        render_game(ball, racketLeft, racketRight, scoreLeft, scoreRight);
    }

    CloseWindow();

    return 0;
}
