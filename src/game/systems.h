#ifndef SYSTEMS_H
#define SYSTEMS_H

#include "ecs.h"
#include "camera.h"
#include "world.h"
#include "ui.h"
#include "../platform/input.h"

void physicsSystem(double dt);
void inputSystem(player_t *player, camera_t *camera, world_t *world, input_t *input);
void cameraSystem(camera_t *camera, entity_t player, input_t *input);
void pickupItems(player_t *player);

void drawEntities();

#endif
