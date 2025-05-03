#ifndef GAME_H
#define GAME_H

#include "../platform/input.h"
#include "../platform/output.h"

// Setup the game called once at start
int gameInit();

// Handle frame called many times
int gameFrame(double dt, input_t *input, output_t *output);

// Handle clean up called once at end
void gameTerminate();

#endif
