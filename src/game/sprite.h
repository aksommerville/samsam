/* sprite.h
 * Man and platforms are not sprites, they're special.
 */
 
#ifndef SPRITE_H
#define SPRITE_H

struct sprite {
  const struct decal *decal;
  double x,y; // World pixels at the decal's center.
  uint8_t xform;
  void (*render)(struct sprite *sprite);
  void (*update)(struct sprite *sprite,double elapsed);
  int defunct;
  double fv[4];
  int iv[4];
};

void sprite_render(struct sprite *sprite);

struct sprite *sprite_new_bullet(double x,double y,double dx);
struct sprite *sprite_new_arrow(double x,double y,double dx);

#endif
