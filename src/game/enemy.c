#include "enemy.h"
#include "physics.h"
#include "components.h"
#include "../platform/sprite.h"

void spawnSlime(entity_t target) {
  float width = 6.0f;
  float height = 2.0f;

  texture_t texture = loadTexture("assets/image.png");
  entity_t slime = ecsCreateEntity();
  sprite_t sprite = createSprite(0, 0, width, height, 0, texture.id);
  transform_t transform = {.pos = (vec2){10.0f, 100.0f}};
  physics_t p = {.body = 0, .isStatic = 0};
  p.body = createBody((vec2){5, 80}, (vec2){width, height});
  ai_t ai = {.target = target, .agroDis = 50.0f};
  ecsAddComponent(slime, SPRITE, (void*)&sprite);
  ecsAddComponent(slime, TRANSFORM, (void*)&transform);
  ecsAddComponent(slime, PHYSICS, (void*)&p);
  ecsAddComponent(slime, AI, (void*)&ai);

  cooldown_t c = {0};
  animationComponent_t ani = {0};
  ani.callback = slimeAnimation;
  ani.animations[ANIM_IDLE] = createAnimation(
    loadTexture("assets/slime/Slime Enemy/Idle/Sprite Sheet - Green Idle.png"),
    0, 0, 0, 0, 7, 96, 32, 0.3f);
  ani.animations[ANIM_JUMP_START] = createAnimation(
    loadTexture("assets/slime/Slime Enemy/Jump/Sprite Sheet - Green Jump Start-up.png"),
    0, 0, 0, 0, 7, 96, 32, 0.214f);
  ani.animations[ANIM_JUMP_UP] = createAnimation(
    loadTexture("assets/slime/Slime Enemy/Jump/Sprite Sheet - Green Jump Up.png"),
    0, 0, 0, 0, 1, 96, 32, 0.3f);
  ani.animations[ANIM_JUMP_FALL] = createAnimation(
    loadTexture("assets/slime/Slime Enemy/Jump/Sprite Sheet - Green Jump to Fall.png"),
    0, 0, 0, 0, 5, 96, 32, 0.1f);
  ani.animations[ANIM_JUMP_DOWN] = createAnimation(
    loadTexture("assets/slime/Slime Enemy/Jump/Sprite Sheet - Green Jump Down.png"),
    0, 0, 0, 0, 1, 96, 32, 0.3f);
  ani.animations[ANIM_JUMP_LAND] = createAnimation(
    loadTexture("assets/slime/Slime Enemy/Jump/Sprite Sheet - Green Jump Land.png"),
    0, 0, 0, 0, 6, 96, 32, 0.3f);
  ani.animations[ANIM_DEATH] = createAnimation(
    loadTexture("assets/slime/Slime Enemy/Death/Sprite Sheet - Green Death.png"),
    0, 0, 0, 0, 14, 96, 32, 0.2f);

  // ANIM_HURT,
  // ANIM_DEATH,
  // ANIM_COUNT

  ecsAddComponent(slime, ANIMATION, (void*)&ani);
  ecsAddComponent(slime, COOLDOWN, (void*)&c);

  health_t health = {.max = 100, .value = 100};
  ecsAddComponent(slime, HEALTH, (void*)&health);
}
