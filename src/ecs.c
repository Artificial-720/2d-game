#include "ecs.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define ENTITY_NULL 9999999

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


int *entitiesValues;
int *entitiesKeys;
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
  maxEntities = 1000;
  entitiesKeys = (int*)malloc(maxEntities * sizeof(int));
  entitiesValues = (int*)malloc(maxEntities * sizeof(int));
  for (int i = 0; i < maxEntities; i++) {
    entitiesKeys[i] = ENTITY_NULL;
    entitiesValues[i] = 0;
  }

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
  free(entitiesKeys);
  free(entitiesValues);
  free(systems);

  for (int i = 0; i < componentCount; i++) {
    free(components[i].data);
  }
  free(components);
}

entity_t ecsCreateEntity() {
  // find next free key
  printf("create entity\n");
  for (int i = 0; i < maxEntities; i++) {
    if (entitiesKeys[i] == ENTITY_NULL) {
      printf("found key %d\n", i);
      entitiesKeys[i] = entityCount;
      entityCount++;

      return i;
    }
  }
  return 0;
}

void ecsDeleteEntity(entity_t entityKey) {
  entity_t actual = entitiesKeys[entityKey];
  printf("deleting entity key %d which points to actual %d\n", entityKey, actual);
  entityCount--;

  // for each component, copy data from entityCount to actual

  entitiesValues[actual] = entitiesValues[entityCount]; // signature
  entitiesValues[entityCount] = 0;
  for (int i = 0; i < componentCount; i++) {
    unsigned long nbytes = components[i].nbytes;
    char *data1 = (char*)ecsGetComponent(actual, i);
    char *data2 = (char*)ecsGetComponent(entityCount, i);

    for (unsigned long byte = 0; byte < nbytes; byte++) {
      data1[byte] = data2[byte];
      data2[byte] = 0; // zero out the old stuff
    }
  }
  // entitiesKeys[entityCount] = entityKey;
  for (int i = 0; i < maxEntities; i++) {
    if (entitiesKeys[i] == entityCount) {
      entitiesKeys[i] = actual;
      break;
    }
  }

  entitiesKeys[entityKey] = ENTITY_NULL;


  printf("after delete entitiesKey[%d] = %d\n", entityKey, entitiesKeys[entityKey]);
  printf("after delete entitiesKey[%d] = %d\n", entityCount, entitiesKeys[entityCount]);

  printf("\n");
  for (int i = 0; i < 5; i++) {
    printf("%d->%d ", i, entitiesKeys[i]);
  }
  printf("\nsigs:\n");
  for (int i = 0; i < 5; i++) {
    printf("%d ", entitiesValues[i]);
  }
  printf("\n");
}

int ecsEntityExists(entity_t entity) {
  entity_t real = entitiesKeys[entity];
  return (real == ENTITY_NULL) ? 0 : 1;
}

void ecsAddComponent(entity_t entityKey, int component, void *data) {
  entity_t entity = entitiesKeys[entityKey];
  assert(components);
  assert(component >= 0 && component < maxComponents);
  assert(components[component].data);
  if (data) {
    char *componentData = (char*)components[component].data;
    componentData += entity * components[component].nbytes;
    for (unsigned long i = 0; i < components[component].nbytes; i++) {
      componentData[i] = ((char*)data)[i];
    }
  }

  // update signature
  entitiesValues[entity] ^= ecsGetSignature(component);
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
    for (int j = 0; j < maxEntities; j++) {
      if (!ecsEntityExists(j)) continue;
      entity_t e = entitiesKeys[j];
      if ((entitiesValues[e] & system.signature) == system.signature) {
        system.callback(j, deltatime);
      }
    }
  }
}

void *ecsGetComponent(entity_t entityKey, int component) {
  entity_t entity = entitiesKeys[entityKey];
  assert(components);
  assert(component >= 0 && component < maxComponents);

  char *componentData = (char*)components[component].data;
  componentData += entity * components[component].nbytes;

  return (void*)componentData;
}

int ecsGetCount() {
  return entityCount;
}
