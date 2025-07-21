#include "game.h"

#define MAN_LEG_SPEED 8.0

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
}

/* Render.
 */
 
void man_render(struct man *man) {

  // Get the decal and also some additional constant geometry.
  const struct decal *decal=decalv+DECAL_man;
  const int armw=3; // Does not include the column separating arm from body.
  const int armh=14; // Includes 2 pixels of shoulder.
  const int trunkh=22; // Top to crotch.
  const int legh=10; // Crotch to bottom.
  const int shouldery=8; // Top to shoulder.
  
  // For now at least, man's position are just framebuffer pixels.
  int dstx=(int)(man->x)-(decal->w>>1);
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
  if ((man->carry_item>0)&&(man->carry_item<51)) {
    const struct decal *idecal=decalv+man->carry_item;
    int ix,iy;
    if ((man->larm==MAN_ARM_UP)&&(man->rarm==MAN_ARM_UP)) { // Both arms up: carry it high and centered.
      ix=dstx+(decal->w>>1);
      iy=dsty-(idecal->h>>2)-3;
    } else if (man->larm==MAN_ARM_SIDE) { // Left arm out?
      ix=dstx-armh+4;
      iy=dsty+shouldery;
    } else if (man->rarm==MAN_ARM_SIDE) { // Right arm out?
      ix=dstx+decal->w+armh-4;
      iy=dsty+shouldery;
    } else if (man->larm==MAN_ARM_UP) { // Left arm up?
      ix=dstx+2;
      iy=dsty-(idecal->h>>2);
    } else if (man->rarm==MAN_ARM_UP) { // Right arm arm?
      ix=dstx+decal->w-2;
      iy=dsty-(idecal->h>>2);
    } else { // No arms in play; do not render the item.
      idecal=0;
    }
    if (idecal) {
      graf_draw_decal(&g.graf,g.texid,ix-(idecal->w>>1),iy-(idecal->h>>1),idecal->x,idecal->y,idecal->w,idecal->h,0);
    }
  }
}
