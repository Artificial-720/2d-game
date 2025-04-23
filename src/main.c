#include <stdio.h>

#include "window.h"   // window and input
#include "ecs.h"      // Entity Component System
#include "render2d.h" // OpenGL

typedef struct {
  vec3 velocity;
  vec3 acceleration;
} rigidbody_t;

typedef struct {
  vec3 position;
  vec3 rotation;
  vec3 scale;
} transform_t;

enum componet_id {
  RIGIDBODY, TRANSFORM, SPRITE, GRAVITY, ANIMATION
};


window_t *window;
Render2d *renderer;


int initialize() {
  int screenWidth = 1028;
  int screenHeight = 720;
  window = windowInit(screenWidth, screenHeight, "2d Game!");
  if(!window) {
    printf("Failed to create a window\n");
    return 1;
  }

  if (ecsInit()) {
    windowTerminate(window);
    return 1;
  }

  renderer = r2dInit();
  if (!renderer) {
    return 1;
  }
  // TODO move this
  renderer->projection = orthographic(0, screenWidth, 0, screenHeight, 0, 1);
  renderer->view = translate(renderer->view, (vec3){screenWidth / 2.0f, screenHeight / 2.0f, 0.0f});

  return 0;
}

void terminate() {
  windowTerminate(window);
  r2dTerminate(renderer);
  ecsTerminate();
}


void update(double deltatime) {
  ecsUpdate(deltatime);

  // Update camera position

}


void gameInit() {
  unsigned int textureId;
  r2dCreateTexture(renderer, "assets/image.png", &textureId);

  // create 10 boxes in a row for "ground"
  for (int i = 0; i < 10; i++) {
    entity_t box = ecsCreateEntity();
    Sprite sprite = {.x = 0, .y = 0, .width = 50, .height = 50, .texture = textureId};
    transform_t transform = {.position = (vec3){50 * i, 50, 0}};
    rigidbody_t rb = {.velocity = (vec3){0, 0, 0}};

    ecsAddComponent(box, SPRITE, (void*)&sprite);
    ecsAddComponent(box, RIGIDBODY, (void*)&rb);
    ecsAddComponent(box, TRANSFORM, (void*)&transform);
  }

  entity_t player = ecsCreateEntity();
  Sprite sprite = {.x = 0, .y = 0, .width = 50, .height = 50, .texture = textureId};
  transform_t transform = {.position = (vec3){50, 150, 0}};
  rigidbody_t rb = {.velocity = (vec3){0, 0, 0}, .acceleration = (vec3){0, 0, 0}};
  float gravity = 150;
  ecsAddComponent(player, SPRITE, (void*)&sprite);
  ecsAddComponent(player, RIGIDBODY, (void*)&rb);
  ecsAddComponent(player, TRANSFORM, (void*)&transform);
  ecsAddComponent(player, GRAVITY, (void*)&gravity);
}


void physicsSystem(entity_t entity, double dt) {
  rigidbody_t *rb = (rigidbody_t*)ecsGetComponent(entity, RIGIDBODY);
  transform_t *transform = (transform_t*)ecsGetComponent(entity, TRANSFORM);
  float *g = (float*)ecsGetComponent(entity, GRAVITY);

  rb->acceleration.y -= (*g) * dt;

  rb->velocity.y += rb->acceleration.y * dt;
  rb->velocity.x += rb->acceleration.x * dt;

  // transform->position.y += rb->velocity.y * dt;
  // transform->position.x += rb->velocity.x * dt;
  float newy = transform->position.y + rb->velocity.y * dt;
  float newx = transform->position.x + rb->velocity.x * dt;

  // check for collision
  // if collision: move back
  // else: keep position
  unsigned int count = ecsGetCount();
  for (unsigned int i = 0; i < count; i++) {
    if (i != entity) {
      // TODO
      // right now everything has a transform but should check
      transform_t *tf2 = (transform_t*)ecsGetComponent(i, TRANSFORM);

      if (
        // top left
        (transform->position.x < tf2->position.x && transform->position.x  + 50 >= tf2->position.x &&
        transform->position.y < tf2->position.y && transform->position.y + 50 >= tf2->position.y)
        // top right
        ||
        (transform->position.x < tf2->position.x + 50 && transform->position.x  + 50 >= tf2->position.x + 50 &&
        transform->position.y < tf2->position.y && transform->position.y + 50 >= tf2->position.y)
        // bottom left
        ||
        (transform->position.x < tf2->position.x && transform->position.x  + 50 >= tf2->position.x &&
        transform->position.y < tf2->position.y + 50 && transform->position.y + 50 >= tf2->position.y + 50)
        // bottom right
        ||
        (transform->position.x < tf2->position.x + 50 && transform->position.x  + 50 >= tf2->position.x + 50 &&
        transform->position.y < tf2->position.y + 50 && transform->position.y + 50 >= tf2->position.y + 50)
      ) {
        printf("collision between %d and %d\n", entity, i);
        newy = transform->position.y;
        newx = transform->position.x;
      }
    }
  }

  transform->position.y = newy;
  transform->position.x = newx;
}

void renderSystem(entity_t entity, double dt) {
  (void)dt;

  transform_t *transform = (transform_t*)ecsGetComponent(entity, TRANSFORM);
  Sprite *sprite = (Sprite*)ecsGetComponent(entity, SPRITE);
  sprite->x = transform->position.x;
  sprite->y = transform->position.y;
  r2dDrawSprite(renderer, *sprite);
}


int main() {
  if (initialize()) {
    terminate();
    return 0;
  }

  ecsRegisterComponent(SPRITE, sizeof(Sprite));
  ecsRegisterComponent(RIGIDBODY, sizeof(rigidbody_t));
  ecsRegisterComponent(TRANSFORM, sizeof(transform_t));
  ecsRegisterComponent(GRAVITY, sizeof(float));
  ecsRegisterSystem(ecsGetSignature(TRANSFORM) | ecsGetSignature(RIGIDBODY) | ecsGetSignature(GRAVITY), physicsSystem);
  ecsRegisterSystem(ecsGetSignature(TRANSFORM) | ecsGetSignature(SPRITE), renderSystem);

  gameInit();

  double previousTime = getTime();
  while(!windowShouldClose(window)) {
    double now = getTime();
    double deltatime = now - previousTime;
    previousTime = now;

    pollInput();
    r2dClear();
    update(deltatime);
    windowSwapBuffers(window);
  }

  terminate();

  return 0;
}
