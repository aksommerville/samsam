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

/* Log.
 */
 
struct sprite *sprite_new_log(double x,double y) {
  y-=6.0;
  struct sprite *sprite=sprite_new(x,y);
  if (sprite) {
    sprite->decal=decalv+NS_DECAL_log;
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
 * iv[0]=mapid
 */
 
static int sprite_door_init(struct sprite *sprite,uint8_t a,uint8_t b,uint8_t c) {
  sprite->iv[0]=(a<<8)|b; // mapid
  return 0;
}

/* Pine.
 * iv[0]=flagid
 */
 
static int sprite_pine_init(struct sprite *sprite,uint8_t a,uint8_t b,uint8_t c) {
  sprite->iv[0]=a;
  if (flag_get(sprite->iv[0])) {
    sprite->defunct=1;
  }
  return 0;
}

/* Burger.
 * fv[0..3]: Summoning range. (LTRB)
 * fv[4]: animclock
 * iv[0]: animframe(0,1)
 * iv[1]: appeased
 */
 
#define BURGER_SUMMON_SPEED 30.0
#define BURGER_AWAY_SPEED 40.0
 
static void sprite_burger_render(struct sprite *sprite) {
  int midx=(int)sprite->x-g.scrollx;
  int midy=(int)sprite->y;
  const struct decal *d_burger=decalv+NS_DECAL_burger;
  const struct decal *d_wing=decalv+NS_DECAL_paddle;
  uint8_t bxform=0,fxform=EGG_XFORM_XREV,nxform=0;
  int fdx=-8,ndx=8,fdy=0,ndy=0;
  if (g.man.x>sprite->x) {
    fxform=0;
    bxform=EGG_XFORM_XREV;
    nxform=EGG_XFORM_XREV;
    fdx=-fdx;
    ndx=-ndx;
  }
  if (sprite->iv[0]) {
    fxform|=EGG_XFORM_YREV;
    nxform|=EGG_XFORM_YREV;
    fdy=10;
    ndy=10;
  }
  graf_draw_decal(&g.graf,g.texid,midx+fdx-(d_wing->w>>1),midy+fdy-4-(d_wing->h>>1),d_wing->x,d_wing->y,d_wing->w,d_wing->h,fxform);
  graf_draw_decal(&g.graf,g.texid,midx-(d_burger->w>>1),midy-(d_burger->h>>1),d_burger->x,d_burger->y,d_burger->w,d_burger->h,bxform);
  if (flag_get(NS_flag_burgerking)) {
    const struct decal *crown=decalv+NS_DECAL_crown;
    graf_draw_decal(&g.graf,g.texid,midx-(crown->w>>1),midy-crown->h-3,crown->x,crown->y,crown->w,crown->h,0);
  }
  graf_draw_decal(&g.graf,g.texid,midx+ndx-(d_wing->w>>1),midy+ndy-4-(d_wing->h>>1),d_wing->x,d_wing->y,d_wing->w,d_wing->h,nxform);
}

static int sprite_burger_is_summoned(const struct sprite *sprite) {
  if (g.day>=3) return 0;
  if (g.day!=g.sky.day) return 0; // Transition in progress.
  if (g.sky.sunlevel<1.0) return 0; // ''
  if (sprite->iv[1]) return 0;
  if (g.man.x<sprite->fv[0]) return 0;
  if (g.man.x>sprite->fv[2]) return 0;
  if (g.man.y<sprite->fv[1]) return 0;
  if (g.man.y>sprite->fv[3]) return 0;
  return 1;
}

static void sprite_burger_update(struct sprite *sprite,double elapsed) {

  // Approach the hero if summoned.
  if (sprite_burger_is_summoned(sprite)) {
    double dstx=g.man.x;
    if (g.man.larm) dstx-=20.0; else dstx+=20.0;
    if (sprite->x<dstx-1.0) {
      sprite->x+=BURGER_SUMMON_SPEED*elapsed;
    } else if (sprite->x>dstx+1.0) {
      sprite->x-=BURGER_SUMMON_SPEED*elapsed;
    }
    double dsty=g.man.y-10.0;
    if (sprite->y<dsty-1.0) {
      sprite->y+=BURGER_SUMMON_SPEED*elapsed;
    } else if (sprite->y>dsty+1.0) {
      sprite->y-=BURGER_SUMMON_SPEED*elapsed;
    }
    // Once we're close, take his offering.
    double dx=sprite->x-dstx;
    double dy=sprite->y-dsty;
    if ((dx>-2.0)&&(dx<2.0)&&(dy>-2.0)&&(dy<2.0)&&g.man.carry_item) {
      egg_play_sound(RID_sound_offering);
      switch (g.man.carry_item) {
        case NS_DECAL_crown: flag_set(NS_flag_burgerking,1); break;
      }
      g.score+=decalv[g.man.carry_item].price;
      g.day++;
      g.man.larm=g.man.rarm=MAN_ARM_DOWN;
      g.man.carry_item=0;
      sprite->iv[1]=1;
    }
  
  // Fly away.
  } else {
    if (sprite->y>-20.0) {
      sprite->y-=BURGER_AWAY_SPEED*elapsed;
    } else {
      sprite->iv[1]=0;
    }
  }
  
  // Wings flap always.
  if ((sprite->fv[4]-=elapsed)<=0.0) {
    sprite->fv[4]+=0.200;
    if (++(sprite->iv[0])>=2) sprite->iv[0]=0;
  }
}

// First update, we establish the horizontal summoning range. Don't do this during init, since our fences might not exist yet.
static void sprite_burger_update_1(struct sprite *sprite,double elapsed) {
  sprite->update=sprite_burger_update;
  sprite->fv[0]=sprite->fv[2]=sprite->x;
  struct sprite *tree=g.spritev;
  int i=g.spritec;
  for (;i-->0;tree++) {
    if (tree->defunct) continue;
    if (tree->decal!=decalv+NS_DECAL_pottedplant) continue;
    if (tree->y<sprite->fv[1]) continue;
    if (tree->y>sprite->fv[3]) continue;
    if (tree->x<sprite->fv[0]) sprite->fv[0]=tree->x;
    if (tree->x>sprite->fv[2]) sprite->fv[2]=tree->x;
  }
}
 
static int sprite_burger_init(struct sprite *sprite,uint8_t a,uint8_t b,uint8_t c) {
  sprite->render=sprite_burger_render;
  sprite->update=sprite_burger_update_1;
  sprite->fv[1]=0.0;
  sprite->fv[3]=sprite->y;
  sprite->y=-20.0;
  return 0;
}

/* Ghost.
 */
 
static void sprite_ghost_update(struct sprite *sprite,double elapsed) {
  if (g.man.x<sprite->x) sprite->xform=EGG_XFORM_XREV;
  else sprite->xform=0;
}
 
static int sprite_ghost_init(struct sprite *sprite,uint8_t a,uint8_t b,uint8_t c) {
  sprite->update=sprite_ghost_update;
  return 0;
}

/* Coin.
 * iv[0]: Flag id.
 */
 
static void sprite_coin_update(struct sprite *sprite,double elapsed) {
  double dx=sprite->x-g.man.x;
  if ((dx<-8.0)||(dx>8.0)) return;
  double dy=sprite->y-g.man.y;
  if ((dy<-8.0)||(dy>8.0)) return;
  if (g.coinc>=999) return;
  g.coinc++;
  egg_play_sound(RID_sound_coin);
  flag_set(sprite->iv[0],1);
  sprite->defunct=1;
}
 
static int sprite_coin_init(struct sprite *sprite,uint8_t a,uint8_t b,uint8_t c) {
  sprite->iv[0]=a;
  if (flag_get(sprite->iv[0])) sprite->defunct=1;
  sprite->update=sprite_coin_update;
  return 0;
}

/* Generic spawning from map.
 */
 
int sprite_init_per_map(struct sprite *sprite,uint8_t a,uint8_t b,uint8_t c) {
  switch (sprite->decal-decalv) {
    case NS_DECAL_dog: return sprite_dog_init(sprite,a,b,c);
    case NS_DECAL_door: return sprite_door_init(sprite,a,b,c);
    case NS_DECAL_pine: return sprite_pine_init(sprite,a,b,c);
    case NS_DECAL_burger: return sprite_burger_init(sprite,a,b,c);
    case NS_DECAL_ghost: return sprite_ghost_init(sprite,a,b,c);
    case NS_DECAL_cent: return sprite_coin_init(sprite,a,b,c);
  }
  return 0;
}
