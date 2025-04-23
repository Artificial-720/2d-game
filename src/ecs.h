#ifndef ECS_H
#define ECS_H

#include "math.h"

typedef unsigned int entity_t;
typedef void (*systemCallback)(entity_t, double);

int ecsInit();
void ecsTerminate();

entity_t ecsCreateEntity();
void ecsAddComponent(entity_t entity, int component, void *data);
void ecsRegisterComponent(int component, unsigned long nbytes);
void ecsRegisterSystem(int signature, systemCallback system);
void ecsUpdate(double deltatime);
void *ecsGetComponent(entity_t entity, int component);
static inline unsigned int ecsGetSignature(int component) {return 0x1 << component;}

#endif
