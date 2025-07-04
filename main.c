#include "include/raylib.h"
#include <stdbool.h>
#include <stdio.h>

// Typedefs
typedef struct Object {
    Rectangle rec;
    Color color;
} Object;

typedef enum Mode {
    normal,
    ai,
    unselected
} Mode;

// Constants
#define TARGET_FPS 60
#define WINDOW_NAME "raypong"
#define RACKET_SPEED 300.0f
#define RACKET_HEIGHT 100
#define RACKET_WIDTH 10
#define BALL_SIZE 15
#define INIT_BALL_SPEED 300.0f
#define BALL_ACCELERATION 10.0f

#define NUM_MODES 2
const Mode GAME_MODES[] = { normal, ai };
const char* MODE_NAMES[] = { "Two players", "Versus AI" };

// Global variables
int g_windowWidth = 800;
int g_windowHeight = 600;
int g_initRacketLeftX;
int g_initRacketLeftY;
int g_initRacketRightX;
int g_initRacketRightY;
int g_initBallX;
int g_initBallY;

// Functions

void calculate_window_related_variables(Rectangle menuRecs[NUM_MODES]) {
    g_windowWidth = GetScreenWidth();
    g_windowHeight = GetScreenHeight();
    g_initRacketLeftX = 0;
    g_initRacketLeftY = g_windowHeight / 2 - RACKET_HEIGHT / 2;
    g_initRacketRightX = g_windowWidth - RACKET_WIDTH;
    g_initRacketRightY = g_windowHeight / 2 - RACKET_HEIGHT / 2;
    g_initBallX = g_windowWidth / 2 - BALL_SIZE / 2;
    g_initBallY = g_windowHeight / 2 - BALL_SIZE / 2;
    for (int i = 0; i < NUM_MODES; i++) {
        menuRecs[i] = (Rectangle){ (float)g_windowWidth / 2 - 125, (float)g_windowHeight / 2 - 50 + i * 80, 250, 60 };
    }
}

void handle_window_resize(Object* ball, Object* racketRight, Object* racketLeft) {
    racketRight->rec.x = g_initRacketRightX;
    racketRight->rec.y = g_initRacketRightY;

    racketLeft->rec.x = g_initRacketLeftX;
    racketLeft->rec.y = g_initRacketRightY;

    ball->rec.x = g_initBallX;
    ball->rec.y = g_initBallY;
}

void draw_game_state(Object ball, Object racketLeft, Object racketRight, int scoreLeft, int scoreRight) {
    ClearBackground(BLACK);

    DrawLine(g_windowWidth / 2, 0, g_windowWidth / 2, g_windowHeight, GRAY);
    DrawText(TextFormat("%i", scoreLeft), g_windowWidth / 2 - 50 - MeasureText(TextFormat("%i", scoreLeft), 40) / 2, 50, 40, RAYWHITE);
    DrawText(TextFormat("%i", scoreRight), g_windowWidth / 2 + 50 - MeasureText(TextFormat("%i", scoreRight), 40) / 2, 50, 40, RAYWHITE);

    DrawRectangleRec(racketLeft.rec, racketLeft.color);
    DrawRectangleRec(racketRight.rec, racketRight.color);
    DrawRectangleRec(ball.rec, ball.color);
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

    draw_game_state(ball, racketLeft, racketRight, scoreLeft, scoreRight);

    DrawText("Press SPACE to play", g_windowWidth / 2 - MeasureText("Press SPACE to play", 20) / 2, 5, 20, RAYWHITE);

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
    if (racketLeft->rec.y > g_windowHeight - RACKET_HEIGHT) {
        racketLeft->rec.y = g_windowHeight - RACKET_HEIGHT;
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
    if (racketRight->rec.y > g_windowHeight - RACKET_HEIGHT) {
        racketRight->rec.y = g_windowHeight - RACKET_HEIGHT;
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
    if (ball->rec.y >= g_windowHeight - BALL_SIZE) {
        ball->rec.y = g_windowHeight - BALL_SIZE;
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
        ball->rec.x = g_windowWidth - RACKET_WIDTH - BALL_SIZE;
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
    if (ball->rec.x > g_windowWidth) {
        *scoreLeft += 1;
        *isGoal = true;
    }
}

void reset_game(Object* ball, Object* racketLeft, Object* racketRight, Vector2* ballVelocity, bool* isGoal, bool* isPaused) {
    ball->rec.x = g_initBallX;
    ball->rec.y = g_initBallY;
    *ballVelocity = init_ball_velocity();

    racketLeft->rec.x = g_initRacketLeftX;
    racketLeft->rec.y = g_initRacketLeftY;
    racketRight->rec.x = g_initRacketRightX;
    racketRight->rec.y = g_initRacketRightY;
    *isGoal = false;
    *isPaused = true;
}

void reset_score(int* scoreLeft, int* scoreRight) {
    *scoreLeft = 0;
    *scoreRight = 0;
}

void render_game(Object ball, Object racketLeft, Object racketRight, int scoreLeft, int scoreRight) {
    BeginDrawing();

    ClearBackground(BLACK);
    
    draw_game_state(ball, racketLeft, racketRight, scoreLeft, scoreRight);

    DrawLine(0, g_windowHeight + 1, g_windowWidth + 1, g_windowHeight + 1, GRAY);
    DrawLine(g_windowWidth + 1, 0, g_windowWidth + 1, g_windowHeight + 1, GRAY);

    EndDrawing();
}

void render_main_menu(Rectangle menuRecs[NUM_MODES], Mode* gameMode) {
    int centerX = g_windowWidth / 2;
    int centerY = g_windowHeight / 2;
    int headerSize = 60;
    int textSize = 30;
    int hoveredRecIndex = -1;

    for (int i = 0; i < NUM_MODES; i++) {
        if (CheckCollisionPointRec(GetMousePosition(), menuRecs[i])) {
            hoveredRecIndex = i;
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                *gameMode = GAME_MODES[i];
            }
        }
    }

    BeginDrawing();

    ClearBackground(BLACK);

    DrawText("raypong", centerX - MeasureText("raypong", headerSize) / 2, centerY - 200, headerSize, RAYWHITE);

    for (int i = 0; i < NUM_MODES; i++) {
        DrawRectangleLinesEx(menuRecs[i], 2.0f, i == hoveredRecIndex ? BLUE : RAYWHITE);
        DrawText(MODE_NAMES[i], centerX - MeasureText(MODE_NAMES[i], textSize) / 2, centerY - 50 + i *80 + 15, textSize, i == hoveredRecIndex ? BLUE : RAYWHITE);
    }

    EndDrawing();
}

// Entry point
int main() {
    // Window init
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(g_windowWidth, g_windowHeight, WINDOW_NAME); 
    SetTargetFPS(TARGET_FPS);

    // Variables definition
    int scoreLeft = 0;
    int scoreRight = 0;
    bool isGoal = false;
    Vector2 ballVelocity = init_ball_velocity();
    bool isPaused = true;
    bool isResized = false;
    Mode gameMode = unselected;
    Rectangle menuRecs[NUM_MODES] = {0};

    calculate_window_related_variables(menuRecs);

    // Objects init
    Object racketLeft = {
        { g_initRacketLeftX, g_initRacketLeftY, RACKET_WIDTH, RACKET_HEIGHT },
        RAYWHITE
    };

    Object racketRight = {
        { g_initRacketRightX, g_initRacketRightY, RACKET_WIDTH, RACKET_HEIGHT },
        RAYWHITE
    };

    Object ball = {
        { g_initBallX, g_initBallY, BALL_SIZE, BALL_SIZE },
        RAYWHITE
    };

    // Main game loop
    while(!WindowShouldClose()) {
        if (IsWindowResized()) {
            isResized = true;
        }

        switch (gameMode) {

            case (unselected):
                SetExitKey(KEY_ESCAPE);
                calculate_window_related_variables(menuRecs);
                render_main_menu(menuRecs, &gameMode);
                break;

            case (ai):
                SetExitKey(KEY_NULL);
                if (isResized) {
                    calculate_window_related_variables(menuRecs);
                }
                if (IsKeyPressed(KEY_ESCAPE)) {
                    gameMode = unselected;
                }
                BeginDrawing();
                ClearBackground(BLACK);
                DrawText("Work in progress", g_windowWidth / 2 - MeasureText("Work in progress", 60) / 2, g_windowHeight / 2 - 50, 60, RAYWHITE);
                EndDrawing();
                break;

            case (normal):
                SetExitKey(KEY_NULL);
                // Game pause handling
                if (isPaused) {
                    // Resize handling
                    if (isResized) {
                        calculate_window_related_variables(menuRecs);
                        handle_window_resize(&ball, &racketRight, &racketLeft);
                        isResized = false;
                    }

                    // Game escape handling
                    if (IsKeyPressed(KEY_ESCAPE)) {
                        gameMode = unselected;
                        reset_game(&ball, &racketLeft, &racketRight, &ballVelocity, &isGoal, &isPaused);
                        reset_score(&scoreLeft, &scoreRight);
                    }

                    render_pause_menu(&isPaused, ball, racketLeft, racketRight, scoreLeft, scoreRight);
                    continue;
                }
                // Game process

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
                break;
        }
    }

    CloseWindow();

    return 0;
}
