#include "include/raylib.h"
#include "include/raymath.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// Constants & typedefs
#define TARGET_FPS 60
#define WINDOW_NAME "raypong"
#define RACKET_SPEED 300.0f
#define RACKET_HEIGHT 100
#define RACKET_WIDTH 10
#define BALL_SIZE 15
#define INIT_BALL_SPEED 300.0f
#define BALL_ACCELERATION 10.0f

typedef struct Object {
    Rectangle rec;
    Color color;
} Object;

typedef enum Mode {
    normal,
    ai,
    unselected
} Mode;

#define NUM_MODES 2
const Mode GAME_MODES[] = { normal, ai };
const char* MODE_NAMES[] = { "Two players", "Versus AI" };

typedef struct GameState {
    int windowWidth;
    int windowHeight;
    int initRacketLeftX;
    int initRacketLeftY;
    int initRacketRightX;
    int initRacketRightY;
    int initBallX;
    int initBallY;
    int scoreLeft;
    int scoreRight;
    bool isGoal;
    Vector2 ballVelocity;
    bool isPaused;
    Mode gameMode;
    Rectangle menuRecs[NUM_MODES];
    Object racketLeft;
    Object racketRight;
    Object ball;
    int predictedBallY;
} GameState;

// Functions
Vector2 init_ball_velocity() {
    Vector2 ballVelocity = (Vector2){INIT_BALL_SPEED, INIT_BALL_SPEED};
    GetRandomValue(0, 1) == 1 ? ballVelocity.x = -ballVelocity.x : 1;
    GetRandomValue(0, 1) == 1 ? ballVelocity.y = -ballVelocity.y : 1;
    return ballVelocity;
}

GameState init_game_state() {
    GameState gameState = {
        .windowWidth = 800,
        .windowHeight = 600,
        .scoreLeft = 0,
        .scoreRight = 0,
        .isGoal = false,
        .ballVelocity = init_ball_velocity(),
        .isPaused = true,
        .gameMode = unselected,
        .menuRecs = {0},
    };
    gameState.predictedBallY = gameState.windowHeight / 2 - RACKET_HEIGHT / 2;
    gameState.initRacketLeftY = gameState.windowHeight / 2 - RACKET_HEIGHT / 2;
    gameState.initRacketRightX = gameState.windowWidth - RACKET_WIDTH;
    gameState.initRacketRightY = gameState.windowHeight / 2 - RACKET_HEIGHT / 2;
    gameState.initBallX = gameState.windowWidth / 2 - BALL_SIZE / 2;
    gameState.initBallY = gameState.windowHeight / 2 - BALL_SIZE / 2;
    gameState.racketLeft = (Object){
        { gameState.initRacketLeftX, gameState.initRacketLeftY, RACKET_WIDTH, RACKET_HEIGHT },
        RAYWHITE
    };
    gameState.racketRight = (Object){
        { gameState.initRacketRightX, gameState.initRacketRightY, RACKET_WIDTH, RACKET_HEIGHT },
        RAYWHITE
    };
    gameState.ball = (Object){
        { gameState.initBallX, gameState.initBallY, BALL_SIZE, BALL_SIZE },
        RAYWHITE
    };
    return gameState;
}

void calculate_window_related_variables(GameState* gameState) {
    gameState->windowWidth = GetScreenWidth();
    gameState->windowHeight = GetScreenHeight();
    gameState->initRacketLeftX = 0;
    gameState->initRacketLeftY = gameState->windowHeight / 2 - RACKET_HEIGHT / 2;
    gameState->initRacketRightX = gameState->windowWidth - RACKET_WIDTH;
    gameState->initRacketRightY = gameState->windowHeight / 2 - RACKET_HEIGHT / 2;
    gameState->initBallX = gameState->windowWidth / 2 - BALL_SIZE / 2;
    gameState->initBallY = gameState->windowHeight / 2 - BALL_SIZE / 2;
    for (int i = 0; i < NUM_MODES; i++) {
        gameState->menuRecs[i] = (Rectangle){ (float)gameState->windowWidth / 2 - 125, (float)gameState->windowHeight / 2 - 50 + i * 80, 250, 60 };
    }
}

void handle_window_resize(GameState* gameState) {
    gameState->racketRight.rec.x = gameState->initRacketRightX;
    gameState->racketRight.rec.y = gameState->initRacketRightY;

    gameState->racketLeft.rec.x = gameState->initRacketLeftX;
    gameState->racketLeft.rec.y = gameState->initRacketLeftY;

    gameState->ball.rec.x = gameState->initBallX;
    gameState->ball.rec.y = gameState->initBallY;
}

void draw_game_state(const GameState* gameState) {
    ClearBackground(BLACK);

    DrawLine(gameState->windowWidth / 2, 0, gameState->windowWidth / 2, gameState->windowHeight, GRAY);
    DrawText(TextFormat("%i", gameState->scoreLeft), gameState->windowWidth / 2 - 50 - MeasureText(TextFormat("%i", gameState->scoreLeft), 40) / 2, 50, 40, RAYWHITE);
    DrawText(TextFormat("%i", gameState->scoreRight), gameState->windowWidth / 2 + 50 - MeasureText(TextFormat("%i", gameState->scoreRight), 40) / 2, 50, 40, RAYWHITE);

    DrawRectangleRec(gameState->racketLeft.rec, gameState->racketLeft.color);
    DrawRectangleRec(gameState->racketRight.rec, gameState->racketRight.color);
    DrawRectangleRec(gameState->ball.rec, gameState->ball.color);
}

void render_pause_menu(GameState* gameState) {
    if (IsKeyDown(KEY_SPACE)) {
        gameState->isPaused = false;
        return;
    }
    BeginDrawing();

    draw_game_state(gameState);

    DrawText("Press SPACE to play", gameState->windowWidth / 2 - MeasureText("Press SPACE to play", 20) / 2, 5, 20, RAYWHITE);

    EndDrawing();
}

void handle_player_input(GameState* gameState, float delta) {
    // Left Racket
    if (IsKeyDown('S')) gameState->racketLeft.rec.y += RACKET_SPEED * delta;
    if (IsKeyDown('W')) gameState->racketLeft.rec.y -= RACKET_SPEED * delta;
    gameState->racketLeft.rec.y = Clamp(gameState->racketLeft.rec.y, 0, gameState->windowHeight - RACKET_HEIGHT);

    // Right racket
    if (gameState->gameMode != ai) {
        if (IsKeyDown(KEY_DOWN)) gameState->racketRight.rec.y += RACKET_SPEED * delta;
        if (IsKeyDown(KEY_UP)) gameState->racketRight.rec.y -= RACKET_SPEED * delta;
        gameState->racketRight.rec.y = Clamp(gameState->racketRight.rec.y, 0, gameState->windowHeight - RACKET_HEIGHT);
    }
}

void handle_ai_move(GameState* gameState, float delta) {
    float racket_center = gameState->racketRight.rec.y + RACKET_HEIGHT / 2;

    if (racket_center < gameState->predictedBallY) gameState->racketRight.rec.y += RACKET_SPEED * delta;
    if (racket_center > gameState->predictedBallY) gameState->racketRight.rec.y -= RACKET_SPEED * delta;

    gameState->racketRight.rec.y = Clamp(gameState->racketRight.rec.y, 0, gameState->windowHeight - RACKET_HEIGHT);
}

void calc_ai_move(GameState* gameState) {
    float distanceX = gameState->racketRight.rec.x - gameState->ball.rec.x;
    float timeToRacket = fabsf(distanceX / gameState->ballVelocity.x);
    float remTime = timeToRacket;
    float Y = gameState->ball.rec.y;
    float Vy = gameState->ballVelocity.y;
    float timeToBounce;

    while (remTime > 0) {
        if (Vy > 0) timeToBounce = (gameState->windowHeight - BALL_SIZE - Y) / Vy;
        else timeToBounce = (0 - Y) / Vy;

        if (timeToBounce >= remTime) {
            gameState->predictedBallY = Y + Vy * remTime;
            break;
        }
        else {
            remTime -= timeToBounce;
            Y += timeToBounce * Vy;
            Vy *= -1;
        }
    }
}

void calc_ball_move(GameState* gameState, float delta) {
    gameState->ball.rec.x += gameState->ballVelocity.x * delta;
    gameState->ball.rec.y += gameState->ballVelocity.y * delta;
}

void calc_ball_screen_collision(GameState* gameState) {
    if (gameState->ball.rec.y <= 0) {
        gameState->ball.rec.y = 0;
        gameState->ballVelocity.y *= -1;
    }
    if (gameState->ball.rec.y >= gameState->windowHeight - BALL_SIZE) {
        gameState->ball.rec.y = gameState->windowHeight - BALL_SIZE;
        gameState->ballVelocity.y *= -1;
    }
}

void calc_ball_racket_collision(GameState* gameState) {
    if (CheckCollisionRecs(gameState->ball.rec, gameState->racketLeft.rec)) {
        gameState->ball.rec.x = 0 + RACKET_WIDTH;
        gameState->ballVelocity.x *= -1;
        gameState->ballVelocity.x += BALL_ACCELERATION;
        gameState->ballVelocity.y += (gameState->ballVelocity.y > 0) ? BALL_ACCELERATION : -BALL_ACCELERATION;
        if (gameState->gameMode == ai) calc_ai_move(gameState);
    }
    if (CheckCollisionRecs(gameState->ball.rec, gameState->racketRight.rec)) {
        gameState->ball.rec.x = gameState->windowWidth - RACKET_WIDTH - BALL_SIZE;
        gameState->ballVelocity.x += BALL_ACCELERATION;
        gameState->ballVelocity.x *= -1;
        gameState->ballVelocity.y += (gameState->ballVelocity.y > 0) ? BALL_ACCELERATION : -BALL_ACCELERATION;
        if (gameState->gameMode == ai) gameState->predictedBallY = gameState->windowHeight / 2 - RACKET_HEIGHT / 2;
    }
}

void check_scoring(GameState* gameState) {
    if (gameState->ball.rec.x + BALL_SIZE < 0) {
        gameState->scoreRight += 1;
        gameState->isGoal = true;
    }
    if (gameState->ball.rec.x > gameState->windowWidth) {
        gameState->scoreLeft += 1;
        gameState->isGoal = true;
    }
}

void reset_game(GameState* gameState) {
    gameState->ball.rec.x = gameState->initBallX;
    gameState->ball.rec.y = gameState->initBallY;
    gameState->ballVelocity = init_ball_velocity();

    gameState->racketLeft.rec.x = gameState->initRacketLeftX;
    gameState->racketLeft.rec.y = gameState->initRacketLeftY;
    gameState->racketRight.rec.x = gameState->initRacketRightX;
    gameState->racketRight.rec.y = gameState->initRacketRightY;
    gameState->isGoal = false;
    gameState->isPaused = true;
}

void reset_score(GameState* gameState) {
    gameState->scoreLeft = 0;
    gameState->scoreRight = 0;
}

void render_game(const GameState* gameState) {
    BeginDrawing();

    ClearBackground(BLACK);
    
    draw_game_state(gameState);

    DrawLine(0, gameState->windowHeight + 1, gameState->windowWidth + 1, gameState->windowHeight + 1, GRAY);
    DrawLine(gameState->windowWidth + 1, 0, gameState->windowWidth + 1, gameState->windowHeight + 1, GRAY);

    EndDrawing();
}

void render_main_menu(GameState* gameState) {
    int centerX = gameState->windowWidth / 2;
    int centerY = gameState->windowHeight / 2;
    int headerSize = 60;
    int textSize = 30;
    int hoveredRecIndex = -1;

    for (int i = 0; i < NUM_MODES; i++) {
        if (CheckCollisionPointRec(GetMousePosition(), gameState->menuRecs[i])) {
            hoveredRecIndex = i;
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                gameState->gameMode = GAME_MODES[i];
            }
        }
    }

    BeginDrawing();

    ClearBackground(BLACK);

    DrawText("raypong", centerX - MeasureText("raypong", headerSize) / 2, centerY - 200, headerSize, RAYWHITE);

    for (int i = 0; i < NUM_MODES; i++) {
        DrawRectangleLinesEx(gameState->menuRecs[i], 2.0f, i == hoveredRecIndex ? BLUE : RAYWHITE);
        DrawText(MODE_NAMES[i], centerX - MeasureText(MODE_NAMES[i], textSize) / 2, centerY - 50 + i *80 + 15, textSize, i == hoveredRecIndex ? BLUE : RAYWHITE);
    }

    EndDrawing();
}

// Entry point
int main() {
    GameState gameState = init_game_state();

    // Window init
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(gameState.windowWidth, gameState.windowHeight, WINDOW_NAME); 
    SetTargetFPS(TARGET_FPS);

    calculate_window_related_variables(&gameState);

    // Main game loop
    while(!WindowShouldClose()) {
        if (IsWindowResized()) {
            calculate_window_related_variables(&gameState);
            handle_window_resize(&gameState);
        }

        switch (gameState.gameMode) {
            case(unselected):
                SetExitKey(KEY_ESCAPE);
                render_main_menu(&gameState);
                break;

            case(ai):
                SetExitKey(KEY_NULL);
                if (gameState.isPaused) {
                    if (IsKeyPressed(KEY_ESCAPE)) {
                        gameState.gameMode = unselected;
                        reset_game(&gameState);
                        reset_score(&gameState);
                    }

                    render_pause_menu(&gameState);
                    continue;
                }

                // Game process

                double delta_a = GetFrameTime();

                // Player input        
                handle_player_input(&gameState, delta_a);
                handle_ai_move(&gameState, delta_a);

                // Calculations
                calc_ball_move(&gameState, delta_a);
                calc_ball_screen_collision(&gameState);
                calc_ball_racket_collision(&gameState);
                check_scoring(&gameState);

                if (gameState.isGoal) {
                    reset_game(&gameState);
                }

                // Render
                render_game(&gameState);
                break;

            case(normal):
                SetExitKey(KEY_NULL);
                if (gameState.isPaused) {
                    if (IsKeyPressed(KEY_ESCAPE)) {
                        gameState.gameMode = unselected;
                        reset_game(&gameState);
                        reset_score(&gameState);
                    }

                    render_pause_menu(&gameState);
                    continue;
                }

                // Game process

                double delta_n = GetFrameTime();

                // Player input        
                handle_player_input(&gameState, delta_n);

                // Calculations
                calc_ball_move(&gameState, delta_n);
                calc_ball_screen_collision(&gameState);
                calc_ball_racket_collision(&gameState);
                check_scoring(&gameState);

                if (gameState.isGoal) {
                    reset_game(&gameState);
                }

                // Render
                render_game(&gameState);
                break;
        }
    }

    CloseWindow();

    return 0;
}
