#include "game.h"

/* Render.
 */
 
void sprite_render(struct sprite *sprite) {
  if (sprite->render) {
    sprite->render(sprite);
  } else if (sprite->decal) {
    int w,h;
    if (sprite->xform&EGG_XFORM_SWAP) {
      w=sprite->decal->h;
      h=sprite->decal->w;
    } else {
      w=sprite->decal->w;
      h=sprite->decal->h;
    }
    int x=(int)sprite->x-(w>>1)-g.scrollx;
    int y=(int)sprite->y-(h>>1);
    graf_draw_decal(&g.graf,g.texid,x,y,sprite->decal->x,sprite->decal->y,sprite->decal->w,sprite->decal->h,sprite->xform);
  }
}

/* New sprite.
 */
 
static struct sprite *sprite_new(double x,double y) {
  if (g.spritec>=SPRITE_LIMIT) return 0;
  struct sprite *sprite=g.spritev+g.spritec++;
  memset(sprite,0,sizeof(struct sprite));
  sprite->x=x;
  sprite->y=y;
  return sprite;
}

/* Bullet.
 */
 
static void sprite_bullet_update(struct sprite *sprite,double elapsed) {
  sprite->x+=elapsed*sprite->fv[0];
  if ((sprite->x<-10.0)||(sprite->x>g.worldw+10.0)) sprite->defunct=1;
  //TODO damage
}
 
struct sprite *sprite_new_bullet(double x,double y,double dx) {
  struct sprite *sprite=sprite_new(x,y);
  if (sprite) {
    sprite->decal=decalv+NS_DECAL_speck;
    sprite->update=sprite_bullet_update;
    sprite->fv[0]=dx*400.0;
  }
  return sprite;
}

/* Arrow.
 */
 
static void sprite_arrow_update(struct sprite *sprite,double elapsed) {
  sprite->x+=elapsed*sprite->fv[0];
  if ((sprite->x<-10.0)||(sprite->x>g.worldw+10.0)) sprite->defunct=1;
  //TODO damage
  //TODO turn into a platform
}
 
struct sprite *sprite_new_arrow(double x,double y,double dx) {
  struct sprite *sprite=sprite_new(x,y);
  if (sprite) {
    sprite->decal=decalv+NS_DECAL_arrow;
    sprite->update=sprite_arrow_update;
    sprite->fv[0]=dx*300.0;
    // Decal points down naturally.
    if (dx<0.0) sprite->xform=EGG_XFORM_SWAP|EGG_XFORM_YREV;
    else sprite->xform=EGG_XFORM_SWAP;
  }
  return sprite;
}

/* Dog. Decorative. Looks at the hero.
 */
 
static void sprite_dog_update(struct sprite *sprite,double elapsed) {
  if (g.man.x<sprite->x) {
    sprite->xform=EGG_XFORM_XREV;
  } else {
    sprite->xform=0;
  }
}
 
static int sprite_dog_init(struct sprite *sprite,uint8_t a,uint8_t b,uint8_t c) {
  sprite->update=sprite_dog_update;
  return 0;
}

/* Door.
 */
 
static int sprite_door_init(struct sprite *sprite,uint8_t a,uint8_t b,uint8_t c) {
  sprite->iv[0]=(a<<8)|b; // mapid
  return 0;
}

/* Generic spawning from map.
 */
 
int sprite_init_per_map(struct sprite *sprite,uint8_t a,uint8_t b,uint8_t c) {
  switch (sprite->decal-decalv) {
    case NS_DECAL_dog: return sprite_dog_init(sprite,a,b,c);
    case NS_DECAL_door: return sprite_door_init(sprite,a,b,c);
  }
  return 0;
}
