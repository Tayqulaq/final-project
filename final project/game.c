#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 400
#define HEIGHT 400
#define BLOCK 20
#define MAX_LENGTH 1000

typedef struct {
    int x, y;
} Segment;

typedef struct {
    Segment parts[MAX_LENGTH];
    int length;
    int dx, dy;
} Snake;

typedef struct {
    int x, y;
} Food;

void spawn_food(Food *food) {
    food->x = (1 + rand() % ((WIDTH / BLOCK) - 2)) * BLOCK;
    food->y = (1 + rand() % ((HEIGHT / BLOCK) - 2)) * BLOCK;
}

void draw_walls(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect top = {0, 0, WIDTH, BLOCK};
    SDL_Rect bottom = {0, HEIGHT - BLOCK, WIDTH, BLOCK};
    SDL_Rect left = {0, 0, BLOCK, HEIGHT};
    SDL_Rect right = {WIDTH - BLOCK, 0, BLOCK, HEIGHT};
    SDL_RenderFillRect(renderer, &top);
    SDL_RenderFillRect(renderer, &bottom);
    SDL_RenderFillRect(renderer, &left);
    SDL_RenderFillRect(renderer, &right);
}

void move_snake(Snake *snake) {
    for (int i = snake->length - 1; i > 0; i--) {
        snake->parts[i] = snake->parts[i - 1];
    }
    snake->parts[0].x += snake->dx;
    snake->parts[0].y += snake->dy;
}

bool hit_self(Snake *snake) {
    for (int i = 1; i < snake->length; i++) {
        if (snake->parts[0].x == snake->parts[i].x && snake->parts[0].y == snake->parts[i].y)
            return true;
    }
    return false;
}

bool hit_wall(Snake *snake) {
    return (
        snake->parts[0].x < BLOCK ||
        snake->parts[0].x >= WIDTH - BLOCK ||
        snake->parts[0].y < BLOCK ||
        snake->parts[0].y >= HEIGHT - BLOCK
    );
}

void draw_snake(SDL_Renderer *renderer, Snake *snake) {
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    for (int i = 0; i < snake->length; i++) {
        SDL_Rect r = {snake->parts[i].x, snake->parts[i].y, BLOCK, BLOCK};
        SDL_RenderFillRect(renderer, &r);
    }
}

void draw_food(SDL_Renderer *renderer, Food *food) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect foodRect = {food->x, food->y, BLOCK, BLOCK};
    SDL_RenderFillRect(renderer, &foodRect);
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window *window = SDL_CreateWindow("UFAZ the Snake",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    TTF_Font *font = TTF_OpenFont("Arial.ttf", 24);
    if (!font) {
        printf("Font error: %s\n", TTF_GetError());
        return 1;
    }

    srand(time(0));
    bool running = true;
    SDL_Event event;
    int score = 0;

    Snake snake;
    snake.length = 1;
    snake.parts[0].x = WIDTH / 2;
    snake.parts[0].y = HEIGHT / 2;
    snake.dx = BLOCK;
    snake.dy = 0;

    Food food;
    spawn_food(&food);

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_UP: if (snake.dy == 0) { snake.dx = 0; snake.dy = -BLOCK; } break;
                    case SDLK_DOWN: if (snake.dy == 0) { snake.dx = 0; snake.dy = BLOCK; } break;
                    case SDLK_LEFT: if (snake.dx == 0) { snake.dx = -BLOCK; snake.dy = 0; } break;
                    case SDLK_RIGHT: if (snake.dx == 0) { snake.dx = BLOCK; snake.dy = 0; } break;
                }
            }
        }

        move_snake(&snake);

        if (hit_wall(&snake) || hit_self(&snake)) {
            running = false;
            break;
        }

        if (snake.parts[0].x == food.x && snake.parts[0].y == food.y) {
            if (snake.length < MAX_LENGTH) {
                snake.length++;
                score++;
            }
            spawn_food(&food);
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        draw_walls(renderer);
        draw_snake(renderer, &snake);
        draw_food(renderer, &food);
        SDL_RenderPresent(renderer);
        SDL_Delay(150);
    }

    // Terminal output
    printf("\n========== GAME OVER ==========\n");
    printf("Game Over Loser 🤪\n");
    printf("🐍 Final Score: %d\n", score);
    printf("🏁 Thanks for playing UFAZ the Snake!\n");
    printf("================================\n\n");

    // Show SDL message
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface *surface = TTF_RenderText_Solid(font, "Game Over Loser 🤪", white);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect messageRect;
    messageRect.w = surface->w;
    messageRect.h = surface->h;
    messageRect.x = (WIDTH - messageRect.w) / 2;
    messageRect.y = (HEIGHT - messageRect.h) / 2;

    SDL_FreeSurface(surface);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, &messageRect);
    SDL_RenderPresent(renderer);

    SDL_Delay(3000); // Show for 3 seconds

    SDL_DestroyTexture(texture);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}