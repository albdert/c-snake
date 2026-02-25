#include "raylib.h"
#include <stdio.h>
#include <time.h>

const char *TITLE = "Snake";
const int SCREEN_W = 600;
const int SCREEN_H = 600;
const int SIZE = 20;
const int ROWS = SCREEN_W/SIZE;
const int COLS = SCREEN_H/SIZE;

const int FPS = 60;
int tickrate = 6;
int score = 0;

// flags
bool menu = true;
bool dead = false;
bool pause = false;
bool quit = false;

// settings
bool warp = false;
bool fast = false;

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;

typedef enum {
    PLAY,
    QUIT,
    RESUME
} ButtonID;

typedef struct {
    Rectangle bounds;
    Color c1;
    Color c2;
    int border;
    char *text;
} Checkbox;

typedef struct {
    Rectangle bounds;
    Color c1;
    Color c2;
    int border;
    char *text;
    ButtonID id;
} Button;

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
    //TODO: fikse rutenett eller liknende for bakgrunn
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

    if (warp) {
        if (snake->segments[0].y<0) { snake->segments[0].y=SIZE*(COLS-1); }
        if (snake->segments[0].y>=SCREEN_H) { snake->segments[0].y=0; }
        if (snake->segments[0].x<0) { snake->segments[0].x=SIZE*(ROWS-1); }
        if (snake->segments[0].x>=SCREEN_W) { snake->segments[0].x=0; }
    }

    for (int i=1; i<snake->len; i++) {
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

void initFood(Rectangle *food) {
    food->width = SIZE;
    food->height = SIZE;
    food->x = GetRandomValue(0, ROWS-1) * SIZE;
    food->y = GetRandomValue(0, COLS-1) * SIZE;
}
void moveFood(Rectangle *food) {
    int rndx = GetRandomValue(0, ROWS-1);
    int rndy = GetRandomValue(0, COLS-1);
    food->x = rndx*SIZE;
    food->y = rndy*SIZE;
}
void drawFood(Rectangle *food) {
    Vector2 pos = {food->x+(SIZE/2), food->y+(SIZE/2)};
    DrawCircleV(pos, SIZE/2, RED);
}
void checkFood(Snake *snake, Rectangle *food) {
    Rectangle head = snake->segments[0];
    if (CheckCollisionRecs(head, *food)) {
        extendSnake(snake);
        moveFood(food);
    }
}

void initButtons(Button buttons[]) {
    int offset = SIZE/2;

    Button play = {
        .bounds = {
            (SCREEN_W/2)-(SIZE*4),
            (SCREEN_H/2)-(SIZE*4),
            SIZE*7,
            SIZE*2
        }, 
        .c1 = GRAY,
        .c2 = DARKGRAY,
        .border = 2,
        .text = "Play [>]",
        .id = PLAY
    };

    Button resume = play;
    resume.text = "Resume [>]";
    resume.id = RESUME;

    Button quit = play;
    quit.bounds.y = (SCREEN_H/2)-(SIZE*2)+offset;
    quit.text = "Quit [x]";
    quit.id = QUIT;

    buttons[PLAY] = play;
    buttons[QUIT] = quit;
    buttons[RESUME] = resume;
}
void drawButtons(Button buttons[], int indexes[], int n) {
    for (int i=0; i<n; i++) {
        Button *b = &(buttons[indexes[i]]);
        DrawRectangleRec(b->bounds, b->c1);
        DrawRectangleLinesEx(b->bounds, b->border, b->c2);
        DrawText(b->text, b->bounds.x+SIZE, b->bounds.y+SIZE/2, SIZE,b->c2);
    }
}

void initGame(Button b[], Rectangle g[][COLS], Snake *s, Rectangle *f) {
    initButtons(b);
    initGrid(g);
    initSnake(s);
    initFood(f);
}
void drawGame(Rectangle g[][COLS], Snake *s, Rectangle *f) {
    ClearBackground(RAYWHITE);
    drawGrid(g);
    drawFood(f);
    drawSnake(s);
}

void mainmenu(Button b[], int i[], int n, char *text) {
    int textOffsetX = MeasureText(text, SIZE*4)/2;
    ClearBackground(RAYWHITE);
    DrawText(text, (SCREEN_W/2)-(textOffsetX), (SCREEN_H/6), SIZE*4, DARKGRAY);
    drawButtons(b, i, 2);
}
void pausemenu(Button b[], int i[], int n) {
    int textOffsetX = MeasureText("Paused", SIZE*4)/2;
    DrawRectangle(0,0,SCREEN_W,SCREEN_H, (Color){255,255,255,128});
    DrawText("Paused", (SCREEN_W/2)-(textOffsetX), (SCREEN_H/6), SIZE*4, DARKGRAY);
    drawButtons(b, i, n); 
}
void settings(Button b[], int i[], int n) {
    int textOffsetX = MeasureText("Settings", SIZE*4)/2;
    DrawRectangle(0,0,SCREEN_W,SCREEN_H, (Color){255,255,255,128});
    DrawText("Settings", (SCREEN_W/2)-(textOffsetX), (SCREEN_H/6), SIZE*4, DARKGRAY);
    drawButtons(b, i, n); 
}

void tick(int *tickCnt) {
    (*tickCnt) = ((*tickCnt)+1) % tickrate;
}
void handleInput(Snake *snake) {
    switch ( GetKeyPressed() ) {
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
    case KEY_ESCAPE:
        if (pause) { pause = false; }
        else { pause = true; }
        break;
    case KEY_SPACE:
        extendSnake(snake);
        break;
    default: break;
    }
}
void handleButtons(Button buttons[], int indexes[], int n) {
    Vector2 mouse = GetMousePosition();
    int action;
    for (int i=0; i<n; i++) {
        Button *b = &(buttons[indexes[i]]); 
        if (CheckCollisionPointRec(mouse, b->bounds)) {
            if (IsMouseButtonPressed(0)) { action = b->id; }
            b->c1 = LIGHTGRAY;
        } else { b->c1 = GRAY; }
    }

    switch (action) {
    case PLAY: menu=false; dead=false; break;
    case QUIT: quit=true; break;
    case RESUME: pause = false; break;
    default: break;
    }
}
void checkCollision(Snake *snake) {
    Rectangle head = snake->segments[0];
    for (int i=1; i<snake->len; i++) {
        if (CheckCollisionRecs(head, snake->segments[i])) {
            dead = true;
            menu = true;
        }
        if ( !warp && (head.x<0 || head.x>=SCREEN_W || head.y<0 || head.y>=SCREEN_H) ) {
            dead = true;
            menu = true;
        }
    }
}

int main(void) {
    Button buttons[8];
    Rectangle grid[ROWS][COLS];
    Rectangle food;
    Snake snake;

    int menuButtons[]  = {PLAY, QUIT};
    int pauseButtons[] = {RESUME, QUIT};
    char *menuText = (char*)TITLE;

    int ticks = 0;

    InitWindow(SCREEN_W, SCREEN_H, TITLE);
    SetExitKey(KEY_NULL);
    SetRandomSeed(time(NULL));
    initGame(buttons, grid, &snake, &food);

    SetTargetFPS(FPS);
    while ( !WindowShouldClose() && !quit ) {
        if (menu) {
            if (dead) { menuText = "Game Over"; }
            BeginDrawing();
            mainmenu(buttons, menuButtons, 2, menuText);
            EndDrawing();
            handleButtons(buttons, menuButtons, 2);
            continue;
        }
        if (pause) {
            BeginDrawing();
            drawGame(grid, &snake, &food);
            pausemenu(buttons, pauseButtons, 2);
            EndDrawing();
            handleButtons(buttons, pauseButtons, 2);
            continue;
        }

        handleInput(&snake);
        tick(&ticks);
        if (ticks==0) {  
            moveSnake(&snake);
            checkFood(&snake, &food);
            checkCollision(&snake);
        }

        BeginDrawing();
        drawGame(grid, &snake, &food);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}