#include "physics.h"

#include <stdio.h>

void step(double deltatime) {
  (void)deltatime;
  // ApplyGravity();
  // ResolveCollisions(dt);
  // MoveObjects(dt);
}

// THIS IS FOR CIRCLE COLLISION DETECTION
// collision_t collisionDetection(collider_t *ac, transform_t *at, collider_t *bc, transform_t *bt) {
//   collision_t result = {0};

//   vec3 aCenter = vec3Add(at->position, ac->offset);
//   vec3 bCenter = vec3Add(bt->position, bc->offset);
//   float d = distance(aCenter, bCenter);

//   if (d < (ac->radius + bc->radius)) {
//     result.hasCollision = 1;

//     // float midpointdistance = d / 2;
//     vec3 ab = vec3Sub(aCenter, bCenter);
//     result.a = vec3Scaler(normalize(ab), (d/2));
//     vec3 ba = vec3Sub(bCenter, aCenter);
//     result.b = vec3Scaler(normalize(ba), (d/2));

//     result.normal = normalize(ba);

//   } else {
//     result.hasCollision = 0;
//   }

//   return result;
// }



// FOR QUAD
collision_t collisionDetection(collider_t *ac, transform_t *at, collider_t *bc, transform_t *bt) {
  collision_t result = {0};
  vec3 aCenter = vec3Add(at->position, ac->offset);
  vec3 bCenter = vec3Add(bt->position, bc->offset);
  float d = distance(aCenter, bCenter);

  // vec2 pointsA[4] = {
  //   {at->position.x, at->position.y},           // top left
  //   {at->position.x + 50, at->position.y},      // top right
  //   {at->position.x, at->position.y + 50},      // bottom left
  //   {at->position.x + 50, at->position.y + 50}  // bottom right
  // };
  vec2 pointsB[4] = {
    {bt->position.x, bt->position.y},           // top left
    {bt->position.x + bt->scale.x, bt->position.y},      // top right
    {bt->position.x, bt->position.y + bt->scale.y},      // bottom left
    {bt->position.x + bt->scale.x, bt->position.y + bt->scale.y}  // bottom right
  };

  // for each point check if inside the other
  for (int i = 0; i < 4; i++) {
    if ((at->position.x <= pointsB[i].x && pointsB[i].x < at->position.x + at->scale.x) && 
        (at->position.y <= pointsB[i].y && pointsB[i].y < at->position.y + at->scale.y)
    ) {
      result.hasCollision = 1;

      vec3 ab = vec3Sub(aCenter, bCenter);
      result.a = vec3Scaler(normalize(ab), (d/2));


      break;
    }
  }


  return result;
}


