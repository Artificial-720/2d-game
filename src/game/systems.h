#ifndef SYSTEMS_H
#define SYSTEMS_H

#include "ecs.h"
#include "camera.h"
#include "world.h"
#include "../platform/input.h"

void spriteSystem();
void physicsSystem(double dt);
void inputSystem(entity_t player, input_t *input, camera_t *camera, world_t *world);
void cameraSystem(camera_t *camera, entity_t player, input_t *input);
void uiSystem(camera_t *camera, input_t * input);

#endif
