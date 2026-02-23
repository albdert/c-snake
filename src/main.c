#include "raylib.h"
#include <stdio.h>

const int ROWS = 45;
const int COLS = 30;
const int SIZE = 20;

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;

typedef struct {
    Rectangle segments[50];
    Direction dir;
    int len;
} Snake;

void initGrid(Rectangle grid[][COLS]) {
    for (int i=0; i<ROWS; i++) {
        for (int j=0; j<COLS; j++) {
            grid[i][j] = (Rectangle){i*SIZE, j*SIZE, SIZE, SIZE};
        }
    }
}
void drawGrid(Rectangle grid[][COLS]) {
    for (int i=0; i<ROWS; i++) {
        for (int j=0; j<COLS; j++) {
            DrawRectangleRec(grid[i][j], BLUE);
        }
    }
 
}

void initSnake(Snake *snake) {
    snake->len = 0;
    snake->dir = UP;
    for (int i=0; i<5; i++) {
        snake->segments[i] = (Rectangle) {
            (SIZE*(ROWS/2)),            // pos x
            (SIZE*(COLS/2)) + (i*SIZE), // pos y
            SIZE,                       // width
            SIZE};                      // height
        snake->len++;
    }
}
void moveSnake(Snake *snake) {
    float prevx = snake->segments[0].x;
    float prevy = snake->segments[0].y;

    switch (snake->dir) {
        case UP   : snake->segments[0].y-=SIZE; break;
        case DOWN : snake->segments[0].y+=SIZE; break;
        case LEFT : snake->segments[0].x-=SIZE; break;
        case RIGHT: snake->segments[0].x+=SIZE; break;
    }

    if (snake->segments[0].y<0) { snake->segments[0].y=SIZE*COLS; }
    if (snake->segments[0].y>600) { snake->segments[0].y=0; }
    if (snake->segments[0].x<0) { snake->segments[0].x=SIZE*ROWS; }
    if (snake->segments[0].x>900) { snake->segments[0].x=0; }

    for (int i=0; i<snake->len; i++) {
        if (i==0) { continue; }
        float newx = snake->segments[i].x;
        float newy = snake->segments[i].y;
        snake->segments[i].x = prevx;
        snake->segments[i].y = prevy;
        prevx = newx;
        prevy = newy;
    }
}
void drawSnake(Snake *snake) {
    DrawRectangleRec(snake->segments[0], DARKGREEN);
    for (int i=1; i<snake->len; i++) {
        DrawRectangleRec(snake->segments[i], GREEN);
    }
}
void extendSnake(Snake *snake) {
    if (snake->len+1==50) { return; } 
    float diffx = snake->segments[snake->len-2].x - snake->segments[snake->len-1].x;
    float diffy = snake->segments[snake->len-2].y - snake->segments[snake->len-1].y;

    Direction dir;
    if (diffy>0) { dir = DOWN; }
    if (diffy<0) { dir = UP; }
    if (diffx>0) { dir = RIGHT; }
    if (diffx<0) { dir = LEFT; }

    float prevx = snake->segments[snake->len-1].x;
    float prevy = snake->segments[snake->len-1].y;
    Rectangle newSegment;
    switch (dir) {
        case UP   : newSegment = (Rectangle) {prevx, prevy+SIZE, SIZE, SIZE}; break;
        case DOWN : newSegment = (Rectangle) {prevx, prevy-SIZE, SIZE, SIZE}; break;
        case LEFT : newSegment = (Rectangle) {prevx+SIZE, prevy, SIZE, SIZE}; break;
        case RIGHT: newSegment = (Rectangle) {prevx-SIZE, prevy, SIZE, SIZE}; break;
    }
    snake->segments[snake->len] = newSegment;
    snake->len++;
}

void handleInput(Snake *snake) {
    switch (GetKeyPressed()) {
    case KEY_W: 
        if (snake->dir==DOWN) { break; }
        snake->dir = UP; break;
    case KEY_S:
        if (snake->dir==UP) { break; }
        snake->dir = DOWN; break;
    case KEY_A: 
        if (snake->dir==RIGHT) { break; }
        snake->dir = LEFT; break;
    case KEY_D: 
        if (snake->dir==LEFT) { break; }
        snake->dir = RIGHT; break;

    case KEY_SPACE:
        extendSnake(snake);
        break;
    default: break;
    }
}

int main(void) {
    Rectangle grid[ROWS][COLS];
    Snake snake;

    InitWindow(900, 600, "Hello world!");
    initGrid(grid);
    initSnake(&snake);

    SetTargetFPS(15);
    while ( !WindowShouldClose() ) {
        handleInput(&snake);
        moveSnake(&snake);

        BeginDrawing();
            ClearBackground(RAYWHITE);
            drawGrid(grid);
            drawSnake(&snake);
       EndDrawing();
    }

    CloseWindow();
    return 0;
}