#ifndef SYSTEMS_H
#define SYSTEMS_H

#include "ecs.h"
#include "camera.h"
#include "world.h"
#include "ui.h"
#include "../platform/input.h"

void physicsSystem(double dt);
void inputSystem(entity_t player, input_t *input, camera_t *camera, world_t *world, item_t *inventory, int *selected);
void cameraSystem(camera_t *camera, entity_t player, input_t *input);

void drawEntities();

#endif
