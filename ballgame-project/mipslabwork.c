
/* mipslabwork.c
   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall
   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):
   This file modified 2017-04-31 by Ture Teknolog
   Modified 2024-10-12 by Yunzhu Tao.
   For copyright and licensing, see file COPYING */


#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"
#include <string.h>
#include <stdio.h>

#define MAX_OBS 4              // Maximum number of obstacles
#define TEXTBUFFER_WIDTH 16    // Width of the text buffer
#define SPAWN_ROW1 1           // Row for upper obstacles
#define SPAWN_ROW3 3           // Row for lower obstacles
#define BALL_X_POS 1           // X position of the ball
#define BALL_Y_POS 3           // Initial Y position of the ball

int mytime = 0x0000;          // Time variable
char ball[] = ".";            // Symbol representing the ball
int score = 0;                // Player's score
int jump_timeout = 0;         // Timeout for jump logic
int button_press_timeout = 3; // Timeout for button press debounce
int game_over = 2;            // Game state: 0 - running, 1 - over, 2 - menu
int jumptime = 3;             // Time the ball stays in the air
int large_jump = 1;           // Flag for large jump
int spawn_delay_timer = 0;    // Timer for obstacle spawning delay
int nextSpawnTime = 8;        // Time until next obstacle spawn
int random_prime = 9999999;   // Seed for random number generation
int previous_random = 0;      // Previous random number
int hard_mode;                // 0: Easy Mode, 1: Hard Mode
int menu_state = 0;           // Menu state
char textbuffer[4][TEXTBUFFER_WIDTH]; // Display text buffer
static int ball_y = BALL_Y_POS;       // Current Y position of the ball

// Structure representing an obstacle
typedef struct {
    char* symbol;   // Symbol of the obstacle
    int xPos;       // X position
    int scored;     // Flag to check if the obstacle has been scored
    int type;       // 0 for lower obstacle, 1 for upper obstacle
} Obstacle;

Obstacle obstacles[MAX_OBS];  // Array of obstacles

/**
 * Function to spawn new obstacles at random intervals.
 * In hard mode, there's a 20% chance to spawn two obstacles.
 */
void spawn_obstacle(void) {
    if (game_over == 0) {
        int num_spawns;

        if (hard_mode == 1) {
            int rand_value = get_random(10); // Generate a random number between 0 and 9
            if (rand_value < 2) { // 20% chance
                num_spawns = 2;
            } else {
                num_spawns = 1;
            }
        } else {
            num_spawns = 1;
        }

        for (int n = 0; n < num_spawns; n++) {
            int availableSlotIndex = -1;

            // Find an available slot for the new obstacle
            for (int i = 0; i < MAX_OBS; i++) {
                if (obstacles[i].xPos == -1) {
                    availableSlotIndex = i;
                    break;
                }
            }

            if (availableSlotIndex != -1) {
                int obstacle_type = get_random(2);

                if (obstacle_type == 1) {
                    // Upper obstacle requiring a higher jump
                    obstacles[availableSlotIndex].symbol = "*";
                    obstacles[availableSlotIndex].type = 1;
                    obstacles[availableSlotIndex].xPos = TEXTBUFFER_WIDTH - 1;
                    obstacles[availableSlotIndex].scored = 0;
                    textbuffer[SPAWN_ROW1][obstacles[availableSlotIndex].xPos] = '*';
                } else {
                    // Lower obstacle requiring a lower jump
                    obstacles[availableSlotIndex].symbol = "_";
                    obstacles[availableSlotIndex].type = 0;
                    obstacles[availableSlotIndex].xPos = TEXTBUFFER_WIDTH - 1;
                    obstacles[availableSlotIndex].scored = 0;
                    textbuffer[SPAWN_ROW3][obstacles[availableSlotIndex].xPos] = '_';
                }

                // Set the time until the next obstacle spawns
                if (hard_mode == 1) {
                    nextSpawnTime = 3 + get_random(3); // Adjusted spawn interval in hard mode
                } else {
                    nextSpawnTime = 5 + get_random(5); // Spawn interval in easy mode
                }
            }
        }
    }
}

/**
 * Function to move the ball upwards.
 */
void ball_up(void) {
    if (ball_y > 0) {
        textbuffer[ball_y][BALL_X_POS] = ' ';  // Clear current position
        ball_y--;                              // Move up
        textbuffer[ball_y][BALL_X_POS] = *ball; // Draw ball at new position
        display_update();
    }
}

/**
 * Function to move the ball downwards.
 */
void ball_down(void) {
    if (ball_y < BALL_Y_POS) {
        textbuffer[ball_y][BALL_X_POS] = ' ';  // Clear current position
        ball_y++;                              // Move down
        textbuffer[ball_y][BALL_X_POS] = *ball; // Draw ball at new position
        display_update();
    }
}

/**
 * Function to move obstacles and handle their interactions with the ball.
 * Obstacles move left across the screen and check for collisions.
 */
void move_obstacles(void) {
    if (game_over != 0) return;

    static int move_counter = 0;
    int move_threshold;

    if (hard_mode == 1) {
        move_threshold = 2; // Obstacles move faster in hard mode
    } else {
        move_threshold = 3; // Obstacles move at normal speed
    }

    move_counter++;
    if (move_counter < move_threshold) return;

    move_counter = 0;
    spawn_delay_timer++;

    if (spawn_delay_timer >= nextSpawnTime) {
        spawn_obstacle();
        spawn_delay_timer = 0;
    }

    for (int i = 0; i < MAX_OBS; i++) {
        if (obstacles[i].xPos >= 0) {
            int prevPos = obstacles[i].xPos;
            obstacles[i].xPos--;  // Move obstacle to the left

            // Clear previous position
            if (prevPos < TEXTBUFFER_WIDTH) {
                if (obstacles[i].type == 1) {
                    textbuffer[SPAWN_ROW1][prevPos] = ' ';
                } else {
                    textbuffer[SPAWN_ROW3][prevPos] = ' ';
                }
            }

            // Draw obstacle at new position
            if (obstacles[i].xPos >= 0 && obstacles[i].xPos < TEXTBUFFER_WIDTH) {
                if (obstacles[i].type == 1) {
                    textbuffer[SPAWN_ROW1][obstacles[i].xPos] = '*';
                } else {
                    textbuffer[SPAWN_ROW3][obstacles[i].xPos] = '_';
                }
            }

            // Check for collision with the ball
            if (obstacles[i].xPos == BALL_X_POS) {
                check_collision();
                if (game_over != 0) return;
            }

            // Remove obstacle if it moves off-screen
            if (obstacles[i].xPos < 0) {
                obstacles[i].xPos = -1;
                obstacles[i].symbol = "";
                obstacles[i].scored = 0;
            }
        }
    }

    display_update();
}

/**
 * Function to check for collisions between the ball and obstacles.
 * If a collision is detected, the game ends.
 */
void check_collision(void) {
    if (game_over != 0) return;

    for (int i = 0; i < MAX_OBS; i++) {
        if (obstacles[i].xPos == BALL_X_POS || obstacles[i].xPos == BALL_X_POS + 1) {
            if (obstacles[i].type == 1) {
                // Collision logic for upper obstacles
                if (ball_y != 1 && ball_y != 2) {
                    display_string(1, "Game Over!");
                    display_string(2, "Press BTN2");
                    display_update();
                    game_over = 1;
                    return;
                } else {
                    score += 1;
                    obstacles[i].scored = 1;
                }
            } else if (obstacles[i].type == 0) {
                // Collision logic for lower obstacles
                if (ball_y == SPAWN_ROW3 && obstacles[i].xPos == BALL_X_POS) {
                    display_string(1, "Game Over!");
                    display_string(2, "Press BTN2");
                    display_update();
                    game_over = 1;
                    return;
                } else {
                    score += 1;
                    obstacles[i].scored = 1;
                }
            }
        }
    }
}

/**
 * Function to reset the game state to its initial condition.
 */
void reset_game(void) {
    game_over = 0;
    score = 0;
    ball_y = BALL_Y_POS;
    jumptime = 3;
    large_jump = 1;
    spawn_delay_timer = 0;
    jump_timeout = 0;
    mytime = 0x0000;
    menu_state = 0;

    for (int i = 0; i < MAX_OBS; i++) {
        obstacles[i].symbol = "";
        obstacles[i].xPos = -1;
        obstacles[i].scored = 0;
    }

    clear_display_buffer();
    textbuffer[ball_y][BALL_X_POS] = *ball;
    display_update();
}

/**
 * Interrupt Service Routine to handle game updates and button presses.
 * Manages the jumping logic and obstacle movement.
 */
void user_isr(void) {
    int btns = getbtns();

    // Handle jump input when the game is running
    if (btns && game_over == 0) {
        if (btns & 0x2 && ball_y == BALL_Y_POS) {
            ball_up();
            jump_timeout = 0;
            // Set jump time based on game mode
            if (hard_mode == 1) {
                jumptime = 2; // Faster jump in hard mode
            } else {
                jumptime = 3; // Normal jump time in easy mode
            }
            large_jump = 0;
        }
    }

    // Timer interrupt for game updates
    if (IFS(0) & 0x00000100) {
        IFSCLR(0) = 0x00000100;
        move_obstacles();
        button_press_timeout--;

        // Handle button press to return to menu
        if (btns & 0x1 && button_press_timeout < 0) {
            button_press_timeout = 5;
            if (game_over == 0 || game_over == 1) {
                game_over = 2;
            }
        }

        if (game_over == 0) {
            update_clock();
            display_score();
            display_update();
        }

        seed += jumptime + jump_timeout + score * 2;
        jump_timeout++;

        // Manage the ball's jumping and falling
        if (jump_timeout == 2 && large_jump != 1 && game_over == 0) {
            large_jump = 1;
            ball_up();
            // Adjust jump time after large jump based on game mode
            if (hard_mode == 1) {
                jumptime = 3; // Faster descent in hard mode
            } else {
                jumptime = 4; // Normal descent time in easy mode
            }
            jump_timeout = 0;
        } else if (jump_timeout == jumptime && game_over == 0) {
            if (ball_y == 1) {
                jump_timeout = 0;
                // Adjust jump time if ball is at the top
                jumptime = 2;
            }
            ball_down();
        }
    }
}

/**
 * Main game loop function.
 * Displays the game menu when the game is over (game_over != 0),
 * waits for the player to select a game mode and start the game,
 * and runs the game logic when the game is running (game_over == 0).
 */
void labwork(void)
{
    // Show menu and wait for BTN2 press when game_over != 0
    while (game_over != 0) {
        // Reset obstacles and clear the display buffer
        for (int i = 0; i < MAX_OBS; i++) {
            obstacles[i].symbol = "";
            obstacles[i].xPos = -1;
            obstacles[i].scored = 0;
        }
        clear_display_buffer();

        // Reset ball position and display menu
        textbuffer[ball_y][1] = *ball;

        if (menu_state == 0) {
            // Display game mode selection menu
            display_string(0, "Play Game!");
            display_string(1, "BTN3: Easy");
            display_string(2, "BTN4: Hard");
            display_update();  // Update the display

            int btns = getbtns();
            if (btns & 0x2) {
                hard_mode = 0;
                menu_state = 1;
            } else if (btns & 0x4) {
                hard_mode = 1;
                menu_state = 1;
            }
        } else if (menu_state == 1) {
            // Display start prompt
            display_string(1, "BTN2 to start");
            display_update();

            // Wait for BTN2 press to start the game
            if (getbtns() & 0x1) {  // Check for BTN2 press
                // Display countdown before game starts
                clear_display_buffer();
                display_string(0, "Ready?");
                display_string(1, "BTN3 to JUMP");
                display_update();
                delay(700);  // Wait 700 ms

                clear_display_buffer();
                display_string(0, "Set!");
                display_string(1, "BTN3 to JUMP");
                display_update();
                delay(700);  // Wait 700 ms

                clear_display_buffer();
                display_string(0, "Go!!!");
                display_string(1, "BTN3 to JUMP");
                display_update();
                delay(700);  // Wait 700 ms

                reset_game();  // Reset the game state
                game_over = 0; // Start the game
                menu_state = 0;
            }
        }
    }

    // If the game is running, continue executing game logic
    if (game_over == 0) {
        random_prime = nextprime(random_prime);  // Example logic during game run
        display_update();  // Update display
    }
}
