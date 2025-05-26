#include "ecs.h"

#include <assert.h>
#include <stdlib.h>  // malloc

typedef struct {
  void *data;
  int dataCount;
  unsigned long nbytes;
} component_t;


// DONT USE ZERO INDEX
// 0 is special it is null key
entity_t *entitiesMap; // entity_t -> entity_t 
int entityCount;
int maxEntities;
unsigned int *signatures;

component_t *components;
int maxComponents;
int componentCount;

int ecsInit() {
  entityCount = 0;
  maxEntities = 4000;
  entitiesMap = (entity_t*)malloc(maxEntities * sizeof(entity_t));
  signatures = (unsigned int*)malloc(maxEntities * sizeof(unsigned int));
  for (int i = 0; i < maxEntities; i++) {
    entitiesMap[i] = 0;
    signatures[i] = 0;
  }

  maxComponents = 32;
  componentCount = 0;
  components = (component_t*)malloc(maxComponents * sizeof(component_t));
  for (int i = 0; i < maxComponents; i++) {
    components[i] = (component_t){.data = 0, .nbytes = 0, .dataCount = 0};
  }

  return 0;
}

void ecsTerminate() {
  free(entitiesMap);
  free(signatures);

  for (int i = 0; i < componentCount; i++) {
    free(components[i].data);
  }
  free(components);
}

entity_t ecsCreateEntity() {
  entity_t result = 0;

  // locate next free key
  for (int i = 1; i < maxEntities; i++) {
    if (entitiesMap[i] == 0) {
      result = i;
      entitiesMap[i] = ++entityCount;
      break;
    }
  }
  assert(result != 0);

  return result;
}


void ecsDeleteEntity(entity_t entity) {
  entity_t value = entitiesMap[entity];

  // compact data
  // dont move anything if last value in array
  if (value != (entity_t)entityCount) {

    entity_t key = 0;
    // find key that points to last value
    for (int i = 0; i < maxEntities; i++) {
      if (entitiesMap[i] == (entity_t)entityCount) {
        key = i;
        break;
      }
    }

    // actually move the data
    signatures[value] = signatures[entityCount];
    for (int i = 0; i < componentCount; i++) {
      unsigned long nbytes = components[i].nbytes;
      char *desData = (char*)ecsGetComponent(entity, i);
      char *srcData = (char*)ecsGetComponent(key, i);
      for (unsigned long byte = 0; byte < nbytes; byte++) {
        desData[byte] = srcData[byte];
        srcData[byte] = 0;
      }
    }

    // update the key
    entitiesMap[key] = value;
  }

  // clear map
  entitiesMap[entity] = 0;

  entityCount--;
}

void ecsAddComponent(entity_t entity, int component, void *data) {
  assert(components);
  assert(component >= 0 && component < maxComponents);
  assert(components[component].data);
  entity_t value = entitiesMap[entity];
  if (data) {
    char *componentData = (char*)components[component].data;
    componentData += value * components[component].nbytes;
    for (unsigned long i = 0; i < components[component].nbytes; i++) {
      componentData[i] = ((char*)data)[i];
    }
  }

  // update signature
  signatures[value] |= ecsGetSignature(component);
}

void *ecsGetComponent(entity_t entity, int component) {
  assert(components);
  assert(component >= 0 && component < maxComponents);
  entity_t value = entitiesMap[entity];

  char *componentData = (char*)components[component].data;
  componentData += value * components[component].nbytes;

  return (void*)componentData;
}



void ecsRegisterComponent(int component, unsigned long nbytes) {
  assert(components);
  assert(component >= 0 && component < maxComponents);

  void *data = malloc(maxEntities * nbytes);
  component_t c = {.dataCount = 0, .data = data, .nbytes = nbytes};

  components[component] = c;
  componentCount++;
}

entity_t *ecsQuery(unsigned int signature, int *count) {
  int total = 0;
  entity_t *list;

  // count how many
  for (int i = 0; i < maxEntities; i++) {
    entity_t key = entitiesMap[i];
    if (!key) continue; // key empty
    if ((signatures[key] & signature) == signature) {
      total++;
    }
  }

  list = (entity_t*)malloc(total * sizeof(entity_t));

  int next = 0;
  // populate
  for (int i = 1; i < maxEntities; i++) {
    entity_t key = entitiesMap[i];
    if (!key) continue; // key empty
    if ((signatures[key] & signature) == signature) {
      list[next++] = i;
    }
  }

  *count = total;
  return list;
}

int ecsHasComponent(entity_t entity, int component) {
  assert(components);
  assert(component >= 0 && component < maxComponents);
  entity_t value = entitiesMap[entity];

  char *componentData = (char*)components[component].data;
  componentData += value * components[component].nbytes;

  return componentData ? 1 : 0;
}


