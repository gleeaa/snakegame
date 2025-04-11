/* Simple Snake Game in C for Linux using ncurses
 * Compile with: gcc -o snake snake.c -lncurses
By Alifna and Team
 */

#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

#define SNAKE_INITIAL_LENGTH 3
#define DELAY 100000 // Game speed (microseconds)

// Directions
#define UP    1
#define DOWN  2
#define LEFT  3
#define RIGHT 4

// Game elements
#define EMPTY_SPACE ' '
#define SNAKE_BODY '='
#define SNAKE_HEAD 'O'
#define FOOD '*'
#define BORDER '#'

typedef struct {
    int x, y;
} Position;

typedef struct {
    Position position[100]; // Maximum snake length
    int length;
    int direction;
    int score;
} Snake;

// Function prototypes
void initialize_game(Snake *snake, Position *food);
void generate_food(Position *food, Snake *snake);
bool is_collision(Snake *snake);
void draw_game(Snake *snake, Position *food);
void update_snake(Snake *snake);
bool is_food_eaten(Snake *snake, Position *food);

int main() {
    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);         // Hide cursor
    timeout(0);          // Non-blocking getch()
   
    // Initialize random seed
    srand(time(NULL));
   
    Snake snake;
    Position food;
   
    initialize_game(&snake, &food);
   
    bool game_over = false;
   
    // Game loop
    while (!game_over) {
        // Get user input
        int ch = getch();
       
        // Update direction based on input
        switch (ch) {
            case KEY_UP:
                if (snake.direction != DOWN)
                    snake.direction = UP;
                break;
            case KEY_DOWN:
                if (snake.direction != UP)
                    snake.direction = DOWN;
                break;
            case KEY_LEFT:
                if (snake.direction != RIGHT)
                    snake.direction = LEFT;
                break;
            case KEY_RIGHT:
                if (snake.direction != LEFT)
                    snake.direction = RIGHT;
                break;
            case 'q':
                game_over = true;
                break;
        }
       
        // Update game state
        update_snake(&snake);
       
        // Check for collision
        if (is_collision(&snake)) {
            game_over = true;
        }
       
        // Check if food is eaten
        if (is_food_eaten(&snake, &food)) {
            snake.length++;
            snake.score += 10;
            generate_food(&food, &snake);
        }
       
        // Draw game
        draw_game(&snake, &food);
       
        // Sleep to control game speed
        usleep(DELAY);
    }
   
    // Game over screen
    clear();
    mvprintw(LINES/2, (COLS-20)/2, "Game Over! Score: %d", snake.score);
    mvprintw(LINES/2 + 1, (COLS-22)/2, "Press any key to exit...");
    refresh();
    timeout(-1);  // Switch back to blocking input
    getch();
   
    // Clean up ncurses
    endwin();
   
    return 0;
}

void initialize_game(Snake *snake, Position *food) {
    // Initialize the screen
    clear();
   
    // Initialize the snake
    snake->length = SNAKE_INITIAL_LENGTH;
    snake->direction = RIGHT;
    snake->score = 0;
   
    // Set initial snake position (middle of the screen)
    int center_x = COLS / 2;
    int center_y = LINES / 2;
   
    // Head position
    snake->position[0].x = center_x;
    snake->position[0].y = center_y;
   
    // Body positions
    for (int i = 1; i < snake->length; i++) {
        snake->position[i].x = center_x - i;
        snake->position[i].y = center_y;
    }
   
    // Generate initial food
    generate_food(food, snake);
}

void generate_food(Position *food, Snake *snake) {
    bool valid_position;
   
    do {
        valid_position = true;
       
        // Generate random position
        food->x = rand() % (COLS - 2) + 1;
        food->y = rand() % (LINES - 2) + 1;
       
        // Check if food is on the snake
        for (int i = 0; i < snake->length; i++) {
            if (snake->position[i].x == food->x && snake->position[i].y == food->y) {
                valid_position = false;
                break;
            }
        }
    } while (!valid_position);
}

bool is_collision(Snake *snake) {
    // Check for collisions with walls
    if (snake->position[0].x <= 0 || snake->position[0].x >= COLS - 1 ||
        snake->position[0].y <= 0 || snake->position[0].y >= LINES - 1) {
        return true;
    }
   
    // Check for collisions with itself
    for (int i = 1; i < snake->length; i++) {
        if (snake->position[0].x == snake->position[i].x &&
            snake->position[0].y == snake->position[i].y) {
            return true;
        }
    }
   
    return false;
}

void draw_game(Snake *snake, Position *food) {
    clear();
   
    // Draw border
    for (int i = 0; i < COLS; i++) {
        mvaddch(0, i, BORDER);
        mvaddch(LINES - 1, i, BORDER);
    }
   
    for (int i = 0; i < LINES; i++) {
        mvaddch(i, 0, BORDER);
        mvaddch(i, COLS - 1, BORDER);
    }
   
    // Draw snake
    mvaddch(snake->position[0].y, snake->position[0].x, SNAKE_HEAD);
   
    for (int i = 1; i < snake->length; i++) {
        mvaddch(snake->position[i].y, snake->position[i].x, SNAKE_BODY);
    }
   
    // Draw food
    mvaddch(food->y, food->x, FOOD);
   
    // Draw score
    mvprintw(0, 5, " Score: %d ", snake->score);
   
    refresh();
}

void update_snake(Snake *snake) {
    // Move the snake body (follow the head)
    for (int i = snake->length - 1; i > 0; i--) {
        snake->position[i].x = snake->position[i-1].x;
        snake->position[i].y = snake->position[i-1].y;
    }
   
    // Move the head according to the direction
    switch (snake->direction) {
        case UP:
            snake->position[0].y--;
            break;
        case DOWN:
            snake->position[0].y++;
            break;
        case LEFT:
            snake->position[0].x--;
            break;
        case RIGHT:
            snake->position[0].x++;
            break;
    }
}

bool is_food_eaten(Snake *snake, Position *food) {
    return (snake->position[0].x == food->x && snake->position[0].y == food->y);
}