#include <stdio.h>

#include "math.h"
#include "window.h"   // window and input
#include "ecs.h"      // Entity Component System
#include "render2d.h" // OpenGL
#include "physics.h"  // handles 2d physics and collisions resolution
#include "camera.h"   // handles camera position and movement


enum componet_id {
  RIGIDBODY, TRANSFORM, SPRITE, GRAVITY, ANIMATION, COLLIDER
};


window_t *window;
Render2d *renderer;
camera_t camera;
entity_t player;


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

  float h = 30;
  float w = h * ((float)screenWidth / screenHeight);

  // TODO move this
  // renderer->projection = orthographic(0, screenWidth, 0, screenHeight, 0, 1);
  renderer->projection = orthographic(0, w, 0, h, 0, 1);
  // renderer->view = translate(renderer->view, (vec3){screenWidth / 2.0f, screenHeight / 2.0f, 0.0f});

  camera = cameraInit();
  // camera.offset.x = screenWidth / 2.0f; // todo include half of player size
  // camera.offset.y = screenHeight / 2.0f;
  camera.offset.x = w / 2.0;
  camera.offset.y = h / 2.0;

  return 0;
}

void terminate() {
  windowTerminate(window);
  r2dTerminate(renderer);
  ecsTerminate();
}



void update(double deltatime) {
  // player input
  rigidbody_t *rb = (rigidbody_t*)ecsGetComponent(player, RIGIDBODY);
  if (getKey(window, KEY_A) == PRESS) {
    rb->velocity.x = -2.0f;
  } else if (getKey(window, KEY_D) == PRESS) {
    rb->velocity.x = 2.0f;
  }
  if (getKey(window, KEY_SPACE) == PRESS) {
    if (rb->velocity.y < 0.01)
    rb->force.y = 300;
  }

  // physics(deltatime);

  ecsUpdate(deltatime);

  // Update camera position
  transform_t *tf = (transform_t*)ecsGetComponent(player, TRANSFORM);
  cameraUpdatePosition(&camera, tf->position.x, tf->position.y);
}


void gameInit() {
  unsigned int textureId;
  r2dCreateTexture(renderer, "assets/image.png", &textureId);

  // TODO move this to window
  // for alpha
  // glEnable(GL_BLEND);
  // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  // for lines
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // create 10 boxes in a row for "ground"
  for (int i = 0; i < 10; i++) {
    entity_t box = ecsCreateEntity();
    Sprite sprite = {.x = 0, .y = 0, .width = 1, .height = 1, .texture = textureId};
    transform_t transform = {.position = (vec3){1 * i, 1, 0}, .scale = (vec3){1.0f, 1.0f, 1.0f}};
    rigidbody_t rb = {.velocity = (vec3){0, 0, 0}};
    collider_t collider = {.offset = (vec3){0, 0, 0}, .radius = 0.5};

    ecsAddComponent(box, SPRITE, (void*)&sprite);
    ecsAddComponent(box, RIGIDBODY, (void*)&rb);
    ecsAddComponent(box, TRANSFORM, (void*)&transform);
    ecsAddComponent(box, COLLIDER, (void*)&collider);
  }

  player = ecsCreateEntity();
  Sprite sprite = {.x = 0, .y = 0, .width = 1, .height = 1, .texture = textureId};
  transform_t transform = {.position = (vec3){1, 2.0f, 0}, .scale = (vec3){1.0f, 1.0f, 1.0f}};
  rigidbody_t rb = {.velocity = (vec3){0, 0, 0}, .force = (vec3){0, 0, 0}, .mass = 1.0f};
  collider_t collider = {.offset = (vec3){0, 0, 0}, .radius = 0.5};
  float gravity = 9.81f;
  ecsAddComponent(player, SPRITE, (void*)&sprite);
  ecsAddComponent(player, RIGIDBODY, (void*)&rb);
  ecsAddComponent(player, TRANSFORM, (void*)&transform);
  ecsAddComponent(player, GRAVITY, (void*)&gravity);
  ecsAddComponent(player, COLLIDER, (void*)&collider);
}


void physicsSystem(entity_t e, double dt) {
  rigidbody_t *rb = (rigidbody_t*)ecsGetComponent(e, RIGIDBODY);
  transform_t *tf = (transform_t*)ecsGetComponent(e, TRANSFORM);
  collider_t *c = (collider_t*)ecsGetComponent(e, COLLIDER);
  float *g = (float*)ecsGetComponent(e, GRAVITY);

  // apply forces to object in this case just gravity
  rb->force.y += rb->mass * -(*g);

  // i think collision detection goes here
  // then resolve collisions here pass dt
  collision_t collision;
  if (c) {
    unsigned int count = ecsGetCount();
    for (unsigned int i = 0; i < count; i++) {
      if (i == e) continue;
      collider_t *bc = (collider_t*)ecsGetComponent(i, COLLIDER);
      transform_t *bt = (transform_t*)ecsGetComponent(i, TRANSFORM);
      if (!bc) continue;
      // check for collision between two colliders
      collision = collisionDetection(c, tf, bc, bt);
      if (collision.hasCollision) {
        break;
        printf("we have a collision between %d %d\n", e, i);
        // rb->force = vec3Scaler(vec3Add(rb->force, collision.a), -50);
        printf("collsion: %f %f\n", collision.a.x, collision.a.y);

      }
    }
  }


  // calculate new position
  rb->velocity.y += rb->force.y / rb->mass * dt;
  rb->velocity.x += rb->force.x / rb->mass * dt;
  tf->position.y += rb->velocity.y * dt;
  tf->position.x += rb->velocity.x * dt;

  if (collision.hasCollision) {
    // stop velocity?
    rb->velocity = (vec3){0, 0, 0};
    // move position
    printf("\npos: %f %f\n", tf->position.x, tf->position.y);
    collision.a.x = 0;
    collision.a.y = 0.5 - collision.a.y;
    printf("hasCollision %f %f\n", collision.a.x, collision.a.y);
    // tf->position = vec3Add(tf->position, collision.a);
    tf->position = vec3Add(tf->position, collision.a);
    printf("pos: %f %f\n\n", tf->position.x, tf->position.y);


  }

  // clear force
  rb->force.y = 0.0f;


  // apply gravity | which is just a downwards force
  // resolve collisions(dt)
  // move objects(dt);
}




void renderSystem(entity_t entity, double dt) {
  (void)dt;

  renderer->view = camera.view;

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
  ecsRegisterComponent(COLLIDER, sizeof(collider_t));
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
