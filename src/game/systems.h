#ifndef SYSTEMS_H
#define SYSTEMS_H

#include "ecs.h"
#include "world.h"
#include "ui.h"
#include "state.h"
#include "../platform/camera.h"
#include "../platform/input.h"
#include "../platform/output.h"

void physicsSystem(double dt);
void pickupItems(player_t *player);
void updateCameras(entity_t player, camera_t *camera, camera_t *cameraUi, input_t *input);

state_e inputPlaying(player_t *player, camera_t *camera, world_t *world, input_t *input, output_t *output);
state_e inputPause(input_t *input);

void drawEntities(camera_t *camera);

#endif
