#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 800
#define HEIGHT 800
#define BLOCK 20
#define MAX_LENGTH 1000

// Define the parts of the Snake
typedef struct {
    int x, y;
} Segment;

// Snake structure 
typedef struct {
    Segment parts[MAX_LENGTH];
    int length;
    int dx, dy;
} Snake;

// Define the Food structure
typedef struct {
    int x, y;
} Food;

// Function to spawn food at random
void spawn_food(Food *food) {
    food->x = (1 + rand() % ((WIDTH / BLOCK) - 2)) * BLOCK; // Prevent food from being placed on the wall
    food->y = (1 + rand() % ((HEIGHT / BLOCK) - 2)) * BLOCK;
}

// Function to draw the walls
void draw_walls(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White color for walls

    SDL_Rect top = {0, 0, WIDTH, BLOCK};
    SDL_Rect bottom = {0, HEIGHT - BLOCK, WIDTH, BLOCK};
    SDL_Rect left = {0, 0, BLOCK, HEIGHT};
    SDL_Rect right = {WIDTH - BLOCK, 0, BLOCK, HEIGHT};

    SDL_RenderFillRect(renderer, &top);
    SDL_RenderFillRect(renderer, &bottom);
    SDL_RenderFillRect(renderer, &left);
    SDL_RenderFillRect(renderer, &right);
}

// Function to update the snake's position
void move_snake(Snake *snake) {
    for (int i = snake->length - 1; i > 0; i--) {
        snake->parts[i] = snake->parts[i - 1]; // Move each segment forward
    }
    snake->parts[0].x += snake->dx;
    snake->parts[0].y += snake->dy;
}

// Function to check if the snake runs into itself
bool hit_self(Snake *snake) {
    for (int i = 1; i < snake->length; i++) {
        if (snake->parts[0].x == snake->parts[i].x && snake->parts[0].y == snake->parts[i].y)
            return true;
    }
    return false;
}

// Function to check if the snake hits the walls
bool hit_wall(Snake *snake) {
    return (
        snake->parts[0].x < BLOCK || // Hit the left wall
        snake->parts[0].x >= WIDTH - BLOCK || // Hit the right wall
        snake->parts[0].y < BLOCK || // Hit the top wall
        snake->parts[0].y >= HEIGHT - BLOCK // Hit the bottom wall
    );
}

// Function to draw the snake
void draw_snake(SDL_Renderer *renderer, Snake *snake) {
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green color for the snake

    for (int i = 0; i < snake->length; i++) {
        SDL_Rect r = {snake->parts[i].x, snake->parts[i].y, BLOCK, BLOCK};
        SDL_RenderFillRect(renderer, &r);
    }
}

// Function to draw the food
void draw_food(SDL_Renderer *renderer, Food *food) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red color for food

    SDL_Rect foodRect;
    foodRect.x = food->x;
    foodRect.y = food->y;
    foodRect.w = BLOCK;
    foodRect.h = BLOCK;

    SDL_RenderFillRect(renderer, &foodRect); // Draw the food
}

int main() {
    SDL_Init(SDL_INIT_VIDEO); // Initialize SDL2 video subsystem

    SDL_Window *window = SDL_CreateWindow("UFAZ the Snake",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    srand(time(0)); // Initialize random seed
    bool running = true;
    SDL_Event event;
    int score = 0;

    // Initialize the snake
    Snake snake;
    snake.length = 1;
    snake.parts[0].x = WIDTH / 2;
    snake.parts[0].y = HEIGHT / 2;
    snake.dx = BLOCK;
    snake.dy = 0;

    // Initialize the food
    Food food;
    spawn_food(&food);

    // Main game loop
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

        move_snake(&snake); // Update snake's position

        if (hit_wall(&snake) || hit_self(&snake)) {
            running = false; // Game over if snake hits a wall or itself
            break;
        }

        // Check if snake eats food
        if (snake.parts[0].x == food.x && snake.parts[0].y == food.y) {
            if (snake.length < MAX_LENGTH) {
                snake.length++;
                score++; // Increase score
            }
            spawn_food(&food); // Spawn new food
        }

        // Clear screen and redraw everything
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
        SDL_RenderClear(renderer);

        draw_walls(renderer); // Draw the walls
        draw_snake(renderer, &snake); // Draw the snake
        draw_food(renderer, &food); // Draw the food

        SDL_RenderPresent(renderer); // Display the new frame
        SDL_Delay(150); // Delay to control the speed of the game
    }

    // Game over screen
    printf("\n========== GAME OVER ==========\n");
    printf("🐍 Final Score: %d\n", score);
    printf("🏁 Thanks for playing UFAZ the Snake!\n");
    printf("================================\n\n");

    // Clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
