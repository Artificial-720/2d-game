#ifndef SYSTEMS_H
#define SYSTEMS_H

#include "ecs.h"
#include "camera.h"
#include "../platform/input.h"

void spriteSystem();
void physicsSystem(double dt);
void playerSystem(entity_t player, input_t *input);
void cameraSystem(camera_t *camera, entity_t player, input_t *input);

#endif
