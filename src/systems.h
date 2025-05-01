#ifndef SYSTEMS_H
#define SYSTEMS_H

#include "ecs.h"

void physicsSystem(entity_t e, double dt);
void renderSystem(entity_t e, double dt);

// loads in new chunks
void chunkLoadSystem(entity_t e, double dt);
// unloads chunks
void chunkUnloadSystem(entity_t e, double dt);
// makes sure chunk data and entity data match
void chunkUpdateSystem(entity_t e, double dt);

#endif
