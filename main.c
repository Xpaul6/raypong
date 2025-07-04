#include "include/raylib.h"
#include <stdbool.h>
#include <stdio.h>

// Structs
typedef struct Object {
    Rectangle rec;
    Color color;
} Object;

// Constants
#define TARGET_FPS 60
#define WINDOW_NAME "raypong"
#define RACKET_SPEED 300.0f
#define RACKET_HEIGHT 100
#define RACKET_WIDTH 10
#define BALL_SIZE 15
#define INIT_BALL_SPEED 300.0f
#define BALL_ACCELERATION 10.0f

int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 600;
int INIT_RACKET_LEFT_X;
int INIT_RACKET_LEFT_Y;
int INIT_RACKET_RIGHT_X;
int INIT_RACKET_RIGHT_Y;
int INIT_BALL_X;
int INIT_BALL_Y;

// Functions

void calculate_window_related_variables() {
    WINDOW_WIDTH = GetScreenWidth();
    WINDOW_HEIGHT = GetScreenHeight();
    INIT_RACKET_LEFT_X = 0;
    INIT_RACKET_LEFT_Y = WINDOW_HEIGHT / 2 - RACKET_HEIGHT / 2;
    INIT_RACKET_RIGHT_X = WINDOW_WIDTH - RACKET_WIDTH;
    INIT_RACKET_RIGHT_Y = WINDOW_HEIGHT / 2 - RACKET_HEIGHT / 2;
    INIT_BALL_X = WINDOW_WIDTH / 2 - BALL_SIZE / 2;
    INIT_BALL_Y = WINDOW_HEIGHT / 2 - BALL_SIZE / 2;
}

void handle_window_resize(Object* ball, Object* racketRight, Object* racketLeft) {
    racketRight->rec.x = INIT_RACKET_RIGHT_X;
    racketRight->rec.y = INIT_RACKET_RIGHT_Y;

    racketLeft->rec.x = INIT_RACKET_LEFT_X;
    racketLeft->rec.y = INIT_RACKET_RIGHT_Y;

    ball->rec.x = INIT_BALL_X;
    ball->rec.y = INIT_BALL_Y;
}

Vector2 init_ball_velocity() {
    Vector2 ballVelocity = (Vector2){INIT_BALL_SPEED, INIT_BALL_SPEED};
    GetRandomValue(0, 1) == 1 ? ballVelocity.x = -ballVelocity.x : 1;
    GetRandomValue(0, 1) == 1 ? ballVelocity.y = -ballVelocity.y : 1;
    return ballVelocity;
}

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

void calc_ball_move(Object* ball, Vector2 ballVelocity, double delta) {
    ball->rec.x += ballVelocity.x * delta;
    ball->rec.y += ballVelocity.y * delta;
}

void calc_ball_screen_collision(Object* ball, Vector2* ballVelocity) {
    if (ball->rec.y <= 0) {
        ball->rec.y = 0;
        ballVelocity->y *= -1;
    }
    if (ball->rec.y >= WINDOW_HEIGHT - BALL_SIZE) {
        ball->rec.y = WINDOW_HEIGHT - BALL_SIZE;
        ballVelocity->y *= -1;
    }
}

void calc_ball_racket_collision(Object* ball, Object* racketLeft, Object* racketRight, Vector2* ballVelocity) {
    if (CheckCollisionRecs(ball->rec, racketLeft->rec)) {
        ball->rec.x = 0 + RACKET_WIDTH;
        ballVelocity->x *= -1;
        ballVelocity->x += BALL_ACCELERATION;
        ballVelocity->y += (ballVelocity->y > 0) ? BALL_ACCELERATION : -BALL_ACCELERATION;
    }
    if (CheckCollisionRecs(ball->rec, racketRight->rec)) {
        ball->rec.x = WINDOW_WIDTH - RACKET_WIDTH - BALL_SIZE;
        ballVelocity->x += BALL_ACCELERATION;
        ballVelocity->x *= -1;
        ballVelocity->y += (ballVelocity->y > 0) ? BALL_ACCELERATION : -BALL_ACCELERATION;
    }
}

void check_scoring(Object* ball, int* scoreLeft, int* scoreRight, bool* isGoal) {
    if (ball->rec.x + BALL_SIZE < 0) {
        *scoreRight += 1;
        *isGoal = true;
    }
    if (ball->rec.x > WINDOW_WIDTH) {
        *scoreLeft += 1;
        *isGoal = true;
    }
}

void reset_game(Object* ball, Object* racketLeft, Object* racketRight, Vector2* ballVelocity, bool* isGoal, bool* isPaused) {
    ball->rec.x = INIT_BALL_X;
    ball->rec.y = INIT_BALL_Y;
    *ballVelocity = init_ball_velocity();

    racketLeft->rec.x = INIT_RACKET_LEFT_X;
    racketLeft->rec.y = INIT_RACKET_LEFT_Y;
    racketRight->rec.x = INIT_RACKET_RIGHT_X;
    racketRight->rec.y = INIT_RACKET_RIGHT_Y;
    *isGoal = false;
    *isPaused = true;
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

    DrawLine(0, WINDOW_HEIGHT + 1, WINDOW_WIDTH + 1, WINDOW_HEIGHT + 1, GRAY);
    DrawLine(WINDOW_WIDTH + 1, 0, WINDOW_WIDTH + 1, WINDOW_HEIGHT + 1, GRAY);

    EndDrawing();
}

// Entry point
int main() {
    // Window init
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME); 
    SetTargetFPS(TARGET_FPS);

    calculate_window_related_variables();

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
    int scoreLeft = 0;
    int scoreRight = 0;
    bool isGoal = false;
    Vector2 ballVelocity = init_ball_velocity();
    bool isPaused = true;
    bool isResized = false;

    // Main game loop
    while(!WindowShouldClose()) {
        if (IsWindowResized()) {
            isResized = true;
        }

        if (isPaused) {
            if (isResized) {
                calculate_window_related_variables();
                handle_window_resize(&ball, &racketRight, &racketLeft);
                isResized = false;
            }
            render_pause_menu(&isPaused, ball, racketLeft, racketRight, scoreLeft, scoreRight);
            continue;
        }

        double delta = GetFrameTime();

        // Player input        
        handle_player_input(delta, &racketLeft, &racketRight);

        // Calculations
        calc_ball_move(&ball, ballVelocity, delta);

        calc_ball_screen_collision(&ball, &ballVelocity);

        calc_ball_racket_collision(&ball, &racketLeft, &racketRight, &ballVelocity);

        check_scoring(&ball, &scoreLeft, &scoreRight, &isGoal);
        if (isGoal) {
            reset_game(&ball, &racketLeft, &racketRight, &ballVelocity, &isGoal, &isPaused);
        }

        // Render
        render_game(ball, racketLeft, racketRight, scoreLeft, scoreRight);
    }

    CloseWindow();

    return 0;
}
