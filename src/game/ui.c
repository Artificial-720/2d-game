#include "ui.h"

#include "ecs.h"
#include "texture.h"
#include "components.h"
#include "../platform/sprite.h"
#include "../platform/renderer2d.h"

#include <assert.h>
#include <stdlib.h>

static int previousSelected;
static entity_t hotbar[10];
static entity_t items[10];

void setupHud() {
  previousSelected = -1;

  unsigned int textureBar = loadTexture("assets/tile_0003.png");
  int size = 35;
  sprite_t barSprite = createSprite(0, 0, size, -size, 0, textureBar);
  for (int i = 0; i < 10; i++) {
    entity_t bar = ecsCreateEntity();
    ui_t barUi = {.pos = {10 + ((size + 2) * i), 10.0f}, .enabled = 1};
    ecsAddComponent(bar, SPRITE, (void*)&barSprite);
    ecsAddComponent(bar, UI, (void*)&barUi);
    hotbar[i] = bar;

    items[i] = ecsCreateEntity();
    sprite_t itemSprite = createSprite(0, 0, (size - 5), -(size - 5), 0, textureBar);
    ui_t itemUi = {.pos = {13 + ((size + 2) * i), 13.0f}, .enabled = 0};
    ecsAddComponent(items[i], SPRITE, (void*)&itemSprite);
    ecsAddComponent(items[i], UI, (void*)&itemUi);
  }
  unsigned int heartTexture = loadTexture("assets/heart.png");
  sprite_t heartSprite = createSprite(0, 0, size, -(size / (375.0f / 325.0f)), 0, heartTexture);
  for (int i = 0; i < 10; i++) {
    entity_t heart = ecsCreateEntity();
    ui_t heartUi = {.pos = {500 + ((size + 2) * i), 10.f}, .enabled = 1};
    ecsAddComponent(heart, SPRITE, (void*)&heartSprite);
    ecsAddComponent(heart, UI, (void*)&heartUi);
  }
}

void drawHud(camera_t *camera, item_t *inventory, int selected, input_t *input) {
  // refresh the hud
  for (int i = 0; i < 10; i++) {
    ui_t *ui = (ui_t*)ecsGetComponent(items[i], UI);
    if (inventory[i].item != ITEM_EMPTY) {
      sprite_t *sprite = (sprite_t*)ecsGetComponent(items[i], SPRITE);
      sprite->texture = getItemTextureId(inventory[i].item);
      ui->enabled = 1;
    } else {
      ui->enabled = 0;
    }
  }

  if (previousSelected != selected) {
    previousSelected = selected;
    for (int i = 0; i < 10; i++) {
      sprite_t *sprite = (sprite_t*)ecsGetComponent(hotbar[i], SPRITE);
      if (i == selected) {
        sprite->texture = getTexture("assets/tile_0022.png");
      } else {
        sprite->texture = getTexture("assets/tile_0003.png");
      }
    }
  }

  // draw the hud
  float h = input->windowHeight;
  float w = input->windowWidth;
  mat4 proj = orthographic(0, w, h, 0, 0, 1);
  mat4 view = mat4Init(1.0f);
  r2dSetView(view);
  r2dSetProjection(proj);

  int count = 0;
  unsigned long sig = ecsGetSignature(UI) | ecsGetSignature(SPRITE);
  entity_t *entities = ecsQuery(sig, &count);

  for (int i = 0; i < count; i++) {
    entity_t entity = entities[i];
    ui_t *ui = (ui_t*)ecsGetComponent(entity, UI);
    if (!ui->enabled) continue;
    sprite_t *sprite = (sprite_t*)ecsGetComponent(entity, SPRITE);

    sprite->x = ui->pos.x;
    sprite->y = ui->pos.y;
    r2dDrawSprite(*sprite);
  }

  free(entities);

  // set back to camera for world
  r2dSetView(camera->view);
  r2dSetProjection(camera->projection);
}
