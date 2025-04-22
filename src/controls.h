#ifndef CONTROLS_H
#define CONTROLS_H

#define ECS_KEY_SPACE 32
#define ECS_KEY_A 65
#define ECS_KEY_D 68
#define ESC_KEY 256


enum keyState {PRESS, RELEASE, HELD};

int ecsGetKey(int key);

#endif
