#include "game.h"

#define MAN_LEG_SPEED 8.0
#define MAN_GRAVITY_RATE 450.0 /* px/s**2 */
#define MAN_TERMINAL_VELOCITY 300.0 /* px/s */
#define MAN_TERMINAL_VELOCITY_UMBRELLA 60.0 /* px/s */
#define MAN_JUMP_POWER_INITIAL 300.0 /* px/s */
#define MAN_JUMP_POWER_DECEL 700.0 /* px/s**2 */

/* Update animation.
 */
 
void man_update(struct man *man,double elapsed) {

  // Legs do a complex dance if walking.
  if (man->walking) {
    if ((man->dlleg>-0.5)&&(man->dlleg<0.5)&&(man->drleg>-0.5)&&(man->drleg<0.5)) {
      man->dlleg=1.0;
      man->drleg=0.0;
    }
    man->lleg+=man->dlleg*elapsed*MAN_LEG_SPEED;
    man->rleg+=man->drleg*elapsed*MAN_LEG_SPEED;
    if (man->lleg>=1.0) {
      man->lleg=1.0;
      man->dlleg=-1.0;
    } else if (man->lleg<0.0) {
      man->lleg=0.0;
      man->dlleg=0.0;
      man->drleg=1.0;
    } else if (man->rleg>=1.0) {
      man->rleg=1.0;
      man->drleg=-1.0;
    } else if (man->rleg<0.0) {
      man->rleg=0.0;
      man->drleg=0.0;
      man->dlleg=1.0;
    }
    
  // Not walking: Slide legs to the bottom if not there already.
  } else {
    if ((man->lleg-=elapsed*MAN_LEG_SPEED)<=0.0) man->lleg=0.0;
    if ((man->rleg-=elapsed*MAN_LEG_SPEED)<=0.0) man->rleg=0.0;
  }
  
  // Jumping?
  if (man->jumping) {
    man->seated=0;
    man->platform=0;
    man->y-=man->jumppower*elapsed;
    if ((man->jumppower-=elapsed*MAN_JUMP_POWER_DECEL)<=0.0) {
      man->jumppower=0.0;
      man->jumping=0;
    }
  
  // Check that we're still seated.
  } else if (man->seated) {
    man->gravity=0.0;
    if (man->platform) { // have we stepped off it?
      int lx=(int)man->x-(decalv[NS_DECAL_man].w>>1);
      int rx=lx+decalv[NS_DECAL_man].w;
      if ((rx<=man->platform->x)||(lx>=man->platform->x+man->platform->w)) {
        man->seated=0;
        man->platform=0;
      }
    }
    
  // Apply gravity.
  } else {
    man->gravity+=MAN_GRAVITY_RATE*elapsed;
    double termv=(man->carry_item==NS_DECAL_umbrella)?MAN_TERMINAL_VELOCITY_UMBRELLA:MAN_TERMINAL_VELOCITY;
    if (man->gravity>termv) man->gravity=termv;
    int pvy=(int)man->y+(decalv[NS_DECAL_man].h>>1);
    man->y+=elapsed*man->gravity;
    int ny=(int)man->y+(decalv[NS_DECAL_man].h>>1);
    double bottom=FBH;
    if (g.indoors) bottom-=decalv[NS_DECAL_dollar].h;
    if (ny>=bottom) {
      man->y=bottom-(decalv[NS_DECAL_man].h>>1);
      man->seated=1;
    } else {
      int lx=(int)man->x-(decalv[NS_DECAL_man].w>>1);
      int rx=lx+decalv[NS_DECAL_man].w;
      const struct platform *platform=g.platformv;
      int i=g.platformc;
      for (;i-->0;platform++) {
        if (platform->x>=rx) continue;
        if (platform->x+platform->w<=lx) continue;
        if (pvy>platform->y) continue;
        if (ny<platform->y) continue;
        man->y=platform->y-(decalv[NS_DECAL_man].h>>1);
        man->seated=1;
        man->gravity=0.0;
        man->platform=platform;
        break;
      }
    }
  }
}

/* Begin or end jump, the player controls.
 */
 
void man_jump(struct man *man) {
  if (man->jumping) return;
  if (!man->seated) return; // TODO Early jump forgiveness? Start a clock and trigger it when we hit the ground. Also some coyote time.
  if ((g.pvinput&EGG_BTN_DOWN)&&man->platform) {
    egg_play_sound(RID_sound_downjump);
    man->y+=2.0;
    man->seated=0;
    man->platform=0;
  } else {
    egg_play_sound(RID_sound_jump);
    man->jumping=1;
    man->jumppower=MAN_JUMP_POWER_INITIAL;
  }
}

void man_unjump(struct man *man) {
  if (!man->jumping) return;
  man->jumping=0;
}

/* Mattock: Break rocks.
 */
 
static void man_begin_mattock(struct man *man) {
  man->action=NS_DECAL_mattock;
  egg_play_sound(RID_sound_swipe);
  //TODO break rocks
}

/* Bomb: Break rocks and lose it.
 */
 
static void man_begin_bomb(struct man *man) {
  man->carry_item=0;
  man->larm=man->rarm=MAN_ARM_DOWN;
  egg_play_sound(RID_sound_explode);
  //TODO break rocks
}

/* Ax: Chop down trees.
 */
 
static void man_begin_ax(struct man *man) {
  man->action=NS_DECAL_ax;
  
  /* Any tree in range of (x) gets chopped, and track the lowest (y) among them.
   * In order to make horizontal blockages, we do stack trees on top of each other.
   * Don't place different trees near the same column.
   * Hmmmm this also means you can chop them down from way above. I think we can arrange for that not to come up.
   */
  double x=man->x,y=0.0,treex=x;
  if (man->larm) x-=12.0; else x+=12.0;
  struct sprite *tree=g.spritev;
  int i=g.spritec;
  for (;i-->0;tree++) {
    if (tree->defunct) continue;
    if (tree->decal!=decalv+NS_DECAL_pine) continue;
    double dx=tree->x-x;
    if ((dx<-24.0)||(dx>24.0)) continue;
    if (tree->y>y) y=tree->y;
    treex=tree->x;
    flag_set(tree->iv[0],1);
    tree->defunct=1;
  }
  
  // Did we chop something?
  if (y>0.0) {
    egg_play_sound(RID_sound_choptree);
    sprite_new_log(treex,y+(decalv[NS_DECAL_pine].h>>1));
  
  // Swing and a miss.
  } else {
    egg_play_sound(RID_sound_swipe);
  }
}

/* Sword: Chop down animals.
 */
 
static void man_begin_sword(struct man *man) {
  man->action=NS_DECAL_sword;
  egg_play_sound(RID_sound_swipe);
  //TODO deal damage
}

/* Shotgun: Fire a destructive projectile.
 */
 
static void man_begin_shotgun(struct man *man) {
  double y=man->y-9.0;
  double x=man->x;
  if (man->larm) x-=17.0; else x+=17.0;
  struct sprite *bullet=sprite_new_bullet(x,y,man->larm?-1.0:1.0);
  if (!bullet) return;
  egg_play_sound(RID_sound_shotgun);
}

/* Bow: Fire a projectile that turns into a platform.
 */
 
static void man_begin_bow(struct man *man) {
  double y=man->y-6.0;
  double x=man->x;
  if (man->larm) x-=10.0; else x+=10.0;
  struct sprite *arrow=sprite_new_arrow(x,y,man->larm?-1.0:1.0);
  if (!arrow) return;
  egg_play_sound(RID_sound_bow);
}

/* Hourglass: Freeze time.
 */
 
static void man_begin_hourglass(struct man *man) {
  egg_play_sound(RID_sound_hourglass);
  man->action=NS_DECAL_hourglass;
  //TODO freeze time
}

/* Magnifier: Search for clues.
 */
 
static void man_begin_magnifier(struct man *man) {
  //TODO clues
}

/* Trigger action.
 */
 
void man_action(struct man *man) {
  switch (man->carry_item) {
    case NS_DECAL_mattock: man_begin_mattock(man); break;
    case NS_DECAL_sword: man_begin_sword(man); break;
    case NS_DECAL_shotgun: man_begin_shotgun(man); break;
    case NS_DECAL_bomb: man_begin_bomb(man); break;
    case NS_DECAL_bow: man_begin_bow(man); break;
    case NS_DECAL_ax: man_begin_ax(man); break;
    case NS_DECAL_hourglass: man_begin_hourglass(man); break;
    case NS_DECAL_magnifier: man_begin_magnifier(man); break;
    default: egg_play_sound(RID_sound_reject); break; // Important to do something, so the player knows she's pressing a live button.
  }
}

void man_unaction(struct man *man) {
  switch (man->action) {
    //TODO
  }
  man->action=0;
}

/* Render.
 */
 
void man_render(struct man *man) {

  // Get the decal and also some additional constant geometry.
  const struct decal *decal=decalv+NS_DECAL_man;
  const int armw=3; // Does not include the column separating arm from body.
  const int armh=14; // Includes 2 pixels of shoulder.
  const int trunkh=22; // Top to crotch.
  const int legh=10; // Crotch to bottom.
  const int shouldery=8; // Top to shoulder.
  
  // For now at least, man's position are just framebuffer pixels.
  int dstx=(int)(man->x)-(decal->w>>1)-g.scrollx;
  int dsty=(int)(man->y)-(decal->h>>1);
  
  // Legs first. We don't slice them exactly; just take half of the decal's width.
  graf_draw_decal(&g.graf,g.texid,dstx,dsty+trunkh-(int)(man->lleg*legh),decal->x,decal->y+trunkh,decal->w>>1,legh,0);
  graf_draw_decal(&g.graf,g.texid,dstx+(decal->w>>1),dsty+trunkh-(int)(man->rleg*legh),decal->x+(decal->w>>1),decal->y+trunkh,decal->w>>1,legh,0);
  
  // Arms.
  switch (man->larm) {
    case MAN_ARM_DOWN: graf_draw_decal(&g.graf,g.texid,dstx,dsty+shouldery,decal->x,decal->y+shouldery,armw,armh,0); break;
    case MAN_ARM_SIDE: graf_draw_decal(&g.graf,g.texid,dstx+armw-armh+4,dsty+shouldery,decal->x,decal->y+shouldery,armw+1,armh,EGG_XFORM_SWAP|EGG_XFORM_YREV); break;
    case MAN_ARM_UP:   graf_draw_decal(&g.graf,g.texid,dstx,dsty+shouldery-armh+5,decal->x,decal->y+shouldery,armw+1,armh,EGG_XFORM_YREV); break;
  }
  switch (man->rarm) {
    case MAN_ARM_DOWN: graf_draw_decal(&g.graf,g.texid,dstx+decal->w-armw,dsty+shouldery,decal->x+decal->w-armw,decal->y+shouldery,armw,armh,0); break;
    case MAN_ARM_SIDE: graf_draw_decal(&g.graf,g.texid,dstx+decal->w-armw-4,dsty+shouldery,decal->x,decal->y+shouldery,armw+1,armh,EGG_XFORM_SWAP); break;
    case MAN_ARM_UP:   graf_draw_decal(&g.graf,g.texid,dstx+decal->w-armw-1,dsty+shouldery-armh+5,decal->x+decal->w-armw-1,decal->y+shouldery,armw+1,armh,EGG_XFORM_YREV); break;
  }
  
  // Trunk.
  graf_draw_decal(&g.graf,g.texid,dstx+armw,dsty,decal->x+armw,decal->y,decal->w-(armw<<1),trunkh,0);
  
  // Are we carring something?
  if ((man->carry_item>0)&&(man->carry_item<DECAL_COUNT)) {
    const struct decal *idecal=decalv+man->carry_item;
    int ix,iy,xform=0,dx=idecal->holddx,dy=idecal->holddy;
    if ((man->larm==MAN_ARM_UP)&&(man->rarm==MAN_ARM_UP)) { // Both arms up: carry it high and centered.
      ix=dstx+(decal->w>>1);
      iy=dsty-(idecal->h>>2)-3;
      dx=dy=0;
    } else if (man->larm==MAN_ARM_SIDE) { // Left arm out?
      ix=dstx-armh+4;
      iy=dsty+shouldery;
      if (idecal->flop<0) xform=EGG_XFORM_XREV;
      dx=-dx;
    } else if (man->rarm==MAN_ARM_SIDE) { // Right arm out?
      ix=dstx+decal->w+armh-4;
      iy=dsty+shouldery;
      if (idecal->flop>0) xform=EGG_XFORM_XREV;
      dx--;
    } else if (man->larm==MAN_ARM_UP) { // Left arm up?
      ix=dstx+2;
      iy=dsty-(idecal->h>>2);
      if (idecal->flop<0) xform=EGG_XFORM_XREV;
      dx=-dx;
    } else if (man->rarm==MAN_ARM_UP) { // Right arm up?
      ix=dstx+decal->w-2;
      iy=dsty-(idecal->h>>2);
      if (idecal->flop>0) xform=EGG_XFORM_XREV;
    } else { // No arms in play; do not render the item.
      idecal=0;
    }
    if (idecal) {
      // Some items, when in use, acquire a new transform.
      switch (man->action) {
        case NS_DECAL_mattock: xform|=EGG_XFORM_YREV; dy+=6; break;
        case NS_DECAL_sword: xform|=EGG_XFORM_YREV; dy+=10; break;
        case NS_DECAL_ax: if (man->larm) {
            xform=EGG_XFORM_SWAP;
            dx-=11;
            dy+=12;
          } else {
            xform=EGG_XFORM_SWAP|EGG_XFORM_YREV;
            dx-=4;
            dy+=12;
          } break;
      }
      graf_draw_decal(&g.graf,g.texid,ix-(idecal->w>>1)+dx,iy-(idecal->h>>1)+dy,idecal->x,idecal->y,idecal->w,idecal->h,xform);
    }
  }
}

/* Are we colliding with a tree?
 */
 
static const struct sprite*man_find_tree(const struct man *man) {
  struct sprite *tree=g.spritev;
  int i=g.spritec;
  for (;i-->0;tree++) {
    if (tree->defunct) continue;
    if (tree->decal!=decalv+NS_DECAL_pine) continue;
    double dx=man->x-tree->x;
    if ((dx<-10.0)||(dx>10.0)) continue;
    double dy=man->y-tree->y;
    if ((dy<-20.0)||(dy>20.0)) continue;
    return tree;
  }
  return 0;
}

/* Shift item to leading arm.
 * Enforce horizontal constraints.
 */
 
void man_face_left(struct man *man) {
  double edge=decalv[NS_DECAL_man].w>>1;
  if (g.indoors) edge+=decalv[NS_DECAL_dollar].w;
  if (man->x<edge) man->x=edge;
  
  const struct sprite *tree=man_find_tree(man);
  if (tree) {
    man->x=tree->x+(decalv[NS_DECAL_man].w>>1);
  }
  
  if (!man->carry_item) return;
  if (man->larm!=MAN_ARM_DOWN) return;
  if (man->rarm==MAN_ARM_SIDE) {
    man->rarm=MAN_ARM_DOWN;
    man->larm=MAN_ARM_SIDE;
  } else if (man->rarm==MAN_ARM_UP) {
    man->rarm=MAN_ARM_DOWN;
    man->larm=MAN_ARM_UP;
  }
}

void man_face_right(struct man *man) {
  double edge=g.worldw-(decalv[NS_DECAL_man].w>>1);
  if (g.indoors) edge-=decalv[NS_DECAL_dollar].w;
  if (man->x>edge) man->x=edge;
  
  const struct sprite *tree=man_find_tree(man);
  if (tree) {
    man->x=tree->x-(decalv[NS_DECAL_man].w>>1);
  }
  
  if (!man->carry_item) return;
  if (man->rarm!=MAN_ARM_DOWN) return;
  if (man->larm==MAN_ARM_SIDE) {
    man->larm=MAN_ARM_DOWN;
    man->rarm=MAN_ARM_SIDE;
  } else if (man->larm==MAN_ARM_UP) {
    man->larm=MAN_ARM_DOWN;
    man->rarm=MAN_ARM_UP;
  }
}
