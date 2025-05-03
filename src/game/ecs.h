#ifndef ECS_H
#define ECS_H

#include "math.h"

typedef unsigned int entity_t;

int ecsInit();
void ecsTerminate();

entity_t ecsCreateEntity();
void ecsDeleteEntity(entity_t entity);
// Add a component to a entity with optional initial data
void ecsAddComponent(entity_t entity, int component, void *data);
void ecsRegisterComponent(int component, unsigned long nbytes);
void *ecsGetComponent(entity_t entity, int component);
int ecsEntityExists(entity_t entity);
static inline unsigned int ecsGetSignature(int component) {return 0x1 << component;}

int ecsGetCount();
int ecsHasComponent(entity_t entity, int component);
entity_t *ecsQuery(unsigned int signature, int *count);

#endif
