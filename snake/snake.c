#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <curses.h>

#define WIDTH 40
#define HEIGHT 20
#define DELAY 50 // Milliseconds between frames

typedef struct {
    int x, y;
} Point;

typedef struct {
    Point body[WIDTH * HEIGHT];
    int length;
    int dx, dy;
} Snake;

void init(Snake *snake, Point *food, int *score, clock_t *start_time) {
    snake->length = 3;
    snake->body[0] = (Point){WIDTH / 2, HEIGHT / 2};
    snake->dx = 1;
    snake->dy = 0;
    *food = (Point){rand() % (WIDTH - 2) + 1, rand() % (HEIGHT - 2) + 1};
    *score = 0;
    *start_time = clock();
}

void draw(Snake *snake, Point *food, int score, clock_t start_time) {
    clear();
    mvprintw(0, 0, "Score: %d | Time: %.1fs | P: Pause/A: Quit", score, (double)(clock() - start_time) / CLOCKS_PER_SEC);
    
    for (int i = 0; i <= HEIGHT; i++) {
        for (int j = 0; j <= WIDTH; j++) {
            if (i == 0 || i == HEIGHT || j == 0 || j == WIDTH) {
                mvprintw(i, j, "#");
            }
        }
    }
    
    mvprintw(food->y, food->x, "O");
    for (int i = 0; i < snake->length; i++) {
        mvprintw(snake->body[i].y, snake->body[i].x, "#");
    }
    refresh();
}

int update(Snake *snake, Point *food, int *score) {
    Point next = {snake->body[0].x + snake->dx, snake->body[0].y + snake->dy};
    if (next.x <= 0 || next.x >= WIDTH || next.y <= 0 || next.y >= HEIGHT) return 0;
    for (int i = 0; i < snake->length; i++) {
        if (snake->body[i].x == next.x && snake->body[i].y == next.y) return 0;
    }
    if (next.x == food->x && next.y == food->y) {
        snake->length++;
        (*score)++;
        *food = (Point){rand() % (WIDTH - 2) + 1, rand() % (HEIGHT - 2) + 1};
    }
    for (int i = snake->length - 1; i > 0; i--) {
        snake->body[i] = snake->body[i - 1];
    }
    snake->body[0] = next;
    return 1;
}

void handle_input(Snake *snake, int *paused) {
    int ch = getch();
    if (ch == 'a') exit(0);
    if (ch == 'p') *paused = !*paused;
    if (!*paused) {
        if ((ch == KEY_UP || ch == 'z')&& snake->dy == 0) { snake->dx = 0; snake->dy = -1; }
        if ((ch == KEY_DOWN || ch == 's')&& snake->dy == 0) { snake->dx = 0; snake->dy = 1; }
        if ((ch == KEY_LEFT || ch == 'q')&& snake->dx == 0) { snake->dx = -1; snake->dy = 0; }
        if ((ch == KEY_RIGHT || ch == 'd')&& snake->dx == 0) { snake->dx = 1; snake->dy = 0; }
    }
}

int main() {
    initscr();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    timeout(DELAY);
    srand(time(NULL));
    
    Snake snake;
    Point food;
    int score, paused = 0;
    clock_t start_time;
    init(&snake, &food, &score, &start_time);
    
    while (1) {
        handle_input(&snake, &paused);
        if (!paused && !update(&snake, &food, &score)) break;
        draw(&snake, &food, score, start_time);
        napms(DELAY);
    }
    
    endwin();
    printf("Game Over! Score: %d\n", score);
    return 0;
}

/*

gcc snake.c -o snake.exe -I C:\PDCurses-3.9 -L C:\PDCurses-3.9\wincon -l:pdcurses.a

snake.exe

*/