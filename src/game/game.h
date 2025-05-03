#ifndef GAME_H
#define GAME_H

// Setup the game called once at start
int gameInit();

// Handle frame called many times
int gameFrame(double dt);

// Handle clean up called once at end
void gameTerminate();

#endif
