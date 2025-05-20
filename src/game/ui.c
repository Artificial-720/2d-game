#include "ui.h"

#include "assets.h"
#include "../platform/renderer2d.h"
#include "item.h"
#include "texture.h"




static int previousSelected;
static sprite_t boxes[10];
static sprite_t items[10];
static sprite_t numbersHigh[10];
static sprite_t numbersLow[10];

static int convertToDecimal(int count, int *high, int *low) {
  if (count > 99) {
    return 1;
  }

  *low = count % 10;
  *high = count / 10;

  return 0;
}

void setupHud() {
  int size = 35;
  int itemSize = size - 5;
  unsigned int textureBar = loadTexture("assets/tile_0003.png");
  for (int i = 0; i < 10; i++) {
    // create inventory slots
    boxes[i] = createSprite(10 + ((size + 2) * i), 10.0f, size, -size, 0, textureBar);
    items[i] = createSprite(13 + ((size + 2) * i), 13.0f, itemSize, -itemSize, 0, textureBar);

    numbersHigh[i] = createSprite(18 + ((size + 2) * i), 25.0f,
                                 itemSize / 4.0f, -itemSize / 2.0f,
                                 0, textureBar);

    numbersLow[i] = createSprite(19 + ((size + 2) * i) + itemSize / 4.0f, 25.0f,
                                 itemSize / 4.0f, -itemSize / 2.0f,
                                 0, textureBar);
  }

  previousSelected = -1;
}

void drawHud(player_t *player, camera_t *camera) {
  const slot_t *inventory = player->inventory;
  const int selected = player->selected;

  // refresh saved data to match inventory data
  for (int i = 0; i < 10; i++) {
    if (inventory[i].item != ITEM_EMPTY) {
      items[i].texture = getItemTextureId(inventory[i].item);
    }
  }

  // update selected
  if (previousSelected != selected) {
    previousSelected = selected;
    for (int i = 0; i < 10; i++) {
      if (i == selected) {
        boxes[i].texture = getTexture("assets/tile_0022.png");
      } else {
        boxes[i].texture = getTexture("assets/tile_0003.png");
      }
    }
  }

  for (int i = 0; i < 10; i++) {
    r2dDrawSprite(camera, boxes[i]);
    if (inventory[i].item != ITEM_EMPTY) {
      r2dDrawSprite(camera, items[i]);
    }
  }

  // print count
  for (int i = 0; i < 10; i++) {
    if (inventory[i].item != ITEM_EMPTY) {
      int high, low;
      int outOfBounds = convertToDecimal(inventory[i].count, &high, &low);
      if (!outOfBounds) {
        numbersLow[i].texture = getNumberTextureId(low);
        r2dDrawSprite(camera, numbersLow[i]);

        if (high != 0) {
          numbersHigh[i].texture = getNumberTextureId(high);
          r2dDrawSprite(camera, numbersHigh[i]);
        }
      }
    }
  }
}

void drawPauseScreen(camera_t *camera) {
  // draw faded color
  unsigned int t = getTexture(BLACK_TEXTURE);
  sprite_t back = createSprite(0, 0, camera->width, -camera->height, 0.0f, t);
  back.a = 0.5f;
  r2dDrawSprite(camera, back);

  // draw pause icon
  float width = 100;
  float height = 100;
  float x = (camera->width - width) / 2.0f;
  float y = (camera->height - height) / 2.0f;
  unsigned int texture = getTexture(PAUSE_SCREEN);
  sprite_t sprite = createSprite(x, y, width, -height, 0, texture);
  r2dDrawSprite(camera, sprite);
}
