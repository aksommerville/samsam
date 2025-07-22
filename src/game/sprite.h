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
  double fv[8];
  int iv[8];
};

void sprite_render(struct sprite *sprite);

struct sprite *sprite_new_bullet(double x,double y,double dx);
struct sprite *sprite_new_arrow(double x,double y,double dx);
struct sprite *sprite_new_log(double x,double y);

/* Caller prepares (x,y,decal). We do the rest.
 * (a,b,c) are the last three bytes of the map command spawning it.
 */
int sprite_init_per_map(struct sprite *sprite,uint8_t a,uint8_t b,uint8_t c);

#endif
