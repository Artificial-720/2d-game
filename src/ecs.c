#include "ecs.h"

#include <assert.h>
#include <stdlib.h>

typedef struct {
  int signature;
  systemCallback callback;
  // entity_t *entities;
  // int entitiesCount;
  // int entitiesMax;
} system_t;

typedef struct {
  void *data;
  int dataCount;
  unsigned long nbytes;
} component_t;


int *entities;
int entityCount;
int maxEntities;

system_t *systems;
int maxSystems;
int systemCount;

component_t *components;
int maxComponents;
int componentCount;


int ecsInit() {
  entityCount = 0;
  maxEntities = 100;
  entities = (int*)malloc(maxEntities * sizeof(int));

  maxSystems = 32;
  systemCount = 0;
  systems = (system_t*)malloc(maxSystems * sizeof(system_t));

  maxComponents = 8;
  componentCount = 0;
  components = (component_t*)malloc(maxComponents * sizeof(component_t));
  for (int i = 0; i < maxComponents; i++) {
    components[i] = (component_t){.data = 0, .nbytes = 0, .dataCount = 0};
  }

  return 0;
}

void ecsTerminate() {
  free(entities);
  free(systems);

  for (int i = 0; i < componentCount; i++) {
    free(components[i].data);
  }
  free(components);
}

entity_t ecsCreateEntity() {
  entity_t e = entityCount;
  entityCount++;
  entities[e] = 0;
  return e;
}

void ecsAddComponent(entity_t entity, int component, void *data) {
  assert(components);
  assert(component >= 0 && component < maxComponents);
  assert(components[component].data);
  char *componentData = (char*)components[component].data;
  componentData += entity * components[component].nbytes;
  for (unsigned long i = 0; i < components[component].nbytes; i++) {
    componentData[i] = ((char*)data)[i];
  }

  // update signature
  entities[entity] ^= ecsGetSignature(component);
}

void ecsRegisterComponent(int component, unsigned long nbytes) {
  assert(components);
  assert(component >= 0 && component < maxComponents);

  void *data = malloc(maxEntities * nbytes);
  component_t c = {.dataCount = 0, .data = data, .nbytes = nbytes};

  components[component] = c;
  componentCount++;
}

void ecsRegisterSystem(int signature, systemCallback system) {
  assert(systems);
  system_t s = {.signature = signature, .callback = system};
  systems[systemCount] = s;
  systemCount++;
}

void ecsUpdate(double deltatime) {
  // TODO update this
  for (int i = 0; i < systemCount; i++) {
    system_t system = systems[i];
    for (int j = 0; j < entityCount; j++) {
      if ((entities[j] & system.signature) == system.signature) {
        system.callback(j, deltatime);
      }
    }
  }
}

void *ecsGetComponent(entity_t entity, int component) {
  assert(components);
  assert(component >= 0 && component < maxComponents);

  char *componentData = (char*)components[component].data;
  componentData += entity * components[component].nbytes;

  return (void*)componentData;
}

int ecsGetCount() {
  return entityCount;
}
